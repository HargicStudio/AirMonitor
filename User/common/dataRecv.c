#include "format.h"
#include "dataRecv.h"
#include "AaInclude.h"
#include "crc.h"
#include "gsmCtrl.h"
#include "dataRecord.h"
#include "gpsAnalyser.h"
#include "gps.h"
#include "cfg.h"
#include "dataHandler.h"
//#include "rtc.h"
#include "config.h"


void HandleGsmRecv(u8 *buf, u16 len)
{
    u16 parmLen = 0;
    u16 parmCrc = 0;
    u32 parmCmd = 0;
    u32 parmAddr = 0;
    u16 crc = 0;
    
    if (len < LEN_HEAD + 1 || !(buf[0] == 'C' && buf[1] == 'C'))
    {
        GSM_LOG_P2("Invild data: len: %d, %s", len, buf);
        return;
    }

    /* 检查是否是本机地址 */
    parmAddr = stringToInt(buf + OFFSET_ADDR, LEN_ADDR);
    if (parmAddr != ConfigGetAddr())
    {
        GSM_LOG_P4("Invild Addr: len: %d, Addr: %d:%d, %s", len, parmAddr, ConfigGetAddr(), buf);
        return;
    }
    /* 获取长度 */
    memcpy(&parmLen, (u8 *)buf + OFFSET_LEN, LEN_PARM_LEN);
    parmLen = nhtons(parmLen);
    
    if (len < parmLen + LEN_HEAD)
    {
        GSM_LOG_P3("Invild Len: len: %d, parmLen: %d:%d", len, parmLen, buf);
        return;
    }
    
    /* 计算CRC */
    memcpy(&parmCrc, (u8 *)buf + OFFSET_CRC, LEN_CRC);
    parmCrc = nhtons(parmCrc);
    crc = usMBCRC16(buf + LEN_HEAD, parmLen);
    if (parmCrc != crc)
    {
        GSM_LOG_P3("Invild Crc, parmCrc: %d:%d %s", parmCrc, crc, buf);
        return;
    }
    
    /* 解析命令 */
    parmCmd = stringToInt(buf + OFFSET_CMD, LEN_CMD);
    
    GSM_LOG_P4("RECV: addr: %d, len:%d, crc: %d, cmd:%d", parmAddr, parmLen, parmCrc, parmCmd);
    //GsmWaitCloseFlagClear();
    ProcessRecvData(buf + LEN_HEAD, parmCmd);
}

void ProcessRecvData(u8 *buf, int cmd)
{
    switch (cmd)
    {
      /* 服务器应答上报数据 */
    case CMD_SER_REPORT_DATA_RSP_V:
      ProcessServerResp(buf);
      break;
      /* 服务器呼叫 */
    case CMD_SER_CHECK_ONLINE_V:
      ProcessServerCall(buf);
      break;
      /* 服务端校时 */
    case CMD_SER_CORRECT_TIME_V:
      ProcessServerTime(buf);
      break;
      /* 掉电模式 */
    case CMD_SER_POWER_SAVE_MODE_V:
      ProcessSavePower(buf);
      break;
      /* 重启 */
    case CMD_SER_REBOOT_V:
      ProcessReboot(buf);
      break;
      /* 获取站端信息 */
    case CMD_SER_STATION_INFO_V:
      ProcessSerGetStationInfo(buf);
      break;
      /* 配置站端 */
    case CMD_SER_CFG_STATION_V:
      ProcessSerConfigStation(buf);
      break;
      /* 传感器配置 */
    case CMD_SER_CFG_SENSOR_V:
      ProcessAdjust(buf);
      break;
      /* 回调请求 */
    case CMD_SER_RECALL_DATA_REQ_V:
      ProcessRecall(buf);
      break;
      /* 修改站号 */
    case 7:
      ProcessChangeAddr(buf);
      break;
      /* 修改采集间隔时间 */
    case 11:
      ProcessChangeSimpleInterval(buf);
      break;
      /* 修改上报时间 */
    case 13:
      ProcessChangeReportInterval(buf);
      break;
      /* 请求软件版本号 */
    case 19:
      ProcessGetSoftVersion(buf);
      break;
      /* 请求硬件版本号 */
    case 33:
      ProcessGetHardVersion(buf);
      break;
      /* 应答处理 */
    case 500:
      ProcessServerResp(buf);
      break;
      /* 校准参数 */
    case 501:
      ProcessAdjust(buf);
      break;
      /* 配置参数 */
    case 503:
      ProcessConfig(buf);
      break;
      /* 请求经纬度 */
    case 505:
      ProcessGetPosition(buf);
      break;
      /* 回调 */
    case 215:
      ProcessRecall(buf);
      break;
    case 217:
      ProcessStopRecall(buf);
      break;
        
    }
}



void ProcessServerTime(u8 *buf)
{
    u8 offset = LEN_ADDR + LEN_CMD;
    u8 y, m, d, h, min, s;
    
    y = stringToInt(buf + offset, 2);
    offset += 2;
    m = stringToInt(buf + offset, 2);
    offset += 2;
    d = stringToInt(buf + offset, 2);
    offset += 2;
    h = stringToInt(buf + offset, 2);
    offset += 2;
    min = stringToInt(buf + offset, 2);
    offset += 2;
    s = stringToInt(buf + offset, 2);
    offset += 2;
    
    GSM_LOG_P3("Server to config Date: %02d.%02d.%02d", y, m, d);
    GSM_LOG_P3("Server to config time: %02d:%02d:%02d", h, min, s);
    
    if (!(y >= 16 && 1 <= m && m <= 12 && 1 <= d && d <= 31 
          && h <= 23 && min <= 59 && s <= 59))
    {
        GSM_LOG_P0("Error time format!");
        return;
    }
    
    gps.utc.strTime[0] = '2';
    gps.utc.strTime[1] = '0';
    memcpy(gps.utc.strTime+2, buf + LEN_ADDR + LEN_CMD, 12);
    
    gps.utc.year = y + 2000;
    gps.utc.month = m;
    gps.utc.date = d;
    gps.utc.hour = h;
    gps.utc.min = min;
    gps.utc.sec = s;
    
    /* 设置时间到RTC */
    if (0 != ConfigSetRTCTime(gps.utc.year, m, d, h, min, s))
    {
        GSM_LOG_P0("Time config error!");
        return;
    }
    
    SetClockSynced(1);
    
    GSM_LOG_P1("time synced with Server : %s", gps.utc.strTime);
    
    ConstructResponse(CMD_CLI_CORRECT_TIME_RSP, buf, 0xff);
}

void ProcessServerCall(u8 *buf)
{
    GSM_LOG_P0("Recv Server Call!");
    ConstructResponse(CMD_CLI_CHECK_ONLINE_RSP, buf, 0xff);
}

void ProcessSavePower(u8 *buf)
{
    GSM_LOG_P0("Recv Save Power!");
    // 他们太菜了，做不到，我不给你们回应了
    //ConstructResponse(CMD_CLI_POWER_SAVE_MODE_RSP, buf, 0xff);
    GsmWaitCloseFlagSet();
}

void ProcessChangeAddr(u8 *buf)
{
    u32 paramAddr = 0;
    
    /* 更新config变量 */
    ConfigSetStrAddr(buf + LEN_ADDR + LEN_CMD);
    paramAddr = stringToInt((u8 *)(buf + LEN_ADDR + LEN_CMD), LEN_ADDR);
    ConfigSetAddr(paramAddr);
    GSM_LOG_P1("Recv Change ADDR! %d", paramAddr);
    /* 更新到配置文件 */
    ConfigSetUpdate(1);
    
    /* 回应 */
    ConstructResponse("008", buf, 0xff);
}

void ProcessChangeSimpleInterval(u8 *buf)
{
    u8 timeLen = 0;
    
    
    /* 更新config变量 */
    timeLen = (u8)stringToInt(buf + LEN_ADDR + LEN_CMD, 2);
    ConfigSetSimpleInterval(timeLen);
    
    /* 更新到配置文件 */
    ConfigSetUpdate(1);
    
    GSM_LOG_P1("Recv change simple interval! New: %d", timeLen);
    
    /* 回应 */
    ConstructResponse("012", buf, 0xff);
}

void ProcessChangeReportInterval(u8 *buf)
{
    u8 timeLen = 0;
    
    /* 更新config变量 */
    timeLen = (u8)stringToInt(buf + LEN_ADDR + LEN_CMD, 2);
    ConfigSetReportInterval(timeLen);
    
    GSM_LOG_P1("Recv change report interval! new: %d", timeLen);
    
    /* 更新到配置文件 */
    ConfigSetUpdate(1);
    
    /* 回应 */
    ConstructResponse("014", buf, 0xff);
}

void ProcessSerConfigStation(u8 *buf)
{
    u8 timeLen = 0;
    u8 timeLen1 = 0;
    
    /* 更新config变量 */
    timeLen = (u8)stringToInt(buf + LEN_ADDR + LEN_CMD, 2);
    ConfigSetSimpleInterval(timeLen);
      
    /* 更新config变量 */
    timeLen1 = (u8)stringToInt(buf + LEN_ADDR + LEN_CMD + 2, 2);
    ConfigSetReportInterval(timeLen1);
    
    GSM_LOG_P2("Recv change report and simple interval! new: %d, %d", timeLen, timeLen1);
    
    /* 更新到配置文件 */
    ConfigSetUpdate(1);
    
    /* 回应 */
    ConstructResponse(CMD_CLI_CFG_STATION_RSP, buf, 0xff);
}

void ProcessChangeGetModuleStatus(u8 *buf)
{
    GSM_LOG_P0("Recv Get Module Status! Don't support!\r\n");
    
    /* 回应 */
    // ConstructResponse("016", buf + LEN_HEAD, 0xff);
}

void ProcessGetSoftVersion(u8 *buf)
{
    GSM_LOG_P0("Recv Get soft version!");
    u16 offset = 0;
    u32 crc = 0;
    u16 ver = 0;
    
    SEND_RESPONSE_FLAG_CLEAR();
    
    offset = LEN_HEAD;
    memcpy((s8 *)SEND_RESPONSE_OFFSET(offset), buf, MAX_ADDR_LEN);
    offset += MAX_ADDR_LEN;
    /* cmd */
    memcpy((s8 *)SEND_RESPONSE_OFFSET(offset), "020", LEN_CMD);
    offset += LEN_CMD;
    
    /* version */
    ver = ConfigGetSoftVer();
    ver = nhtons(ver);
    memcpy((s8 *)SEND_RESPONSE_OFFSET(offset), &ver, 2);
    offset += 2;
    
    /* HEAD */
    /* CRC */
    crc = usMBCRC16( (u8 *)SEND_RESPONSE_OFFSET(LEN_HEAD) , offset - LEN_HEAD );
    
    FormatHead(crc, offset - LEN_HEAD, (u8 *)SEND_RESPONSE_OFFSET(0));
    
    SEND_RESPONSE_SET_BYTE('\r', offset);
    SEND_RESPONSE_SET_BYTE('\n', offset + 1);
    
    /* 设置包含回车的长度，用于发送 */
    SEND_RESPONSE_SET_LEN(offset+2);
    
    /* 设置发送标志，发送程序开始发送 */
    SEND_RESPONSE_FLAG_SET();
    
    /* 设置是否需要回应 */
    SEND_RESPONSE_RESP_FALG_SET(0);
}

void ProcessGetHardVersion(u8 *buf)
{
    GSM_LOG_P0("Recv Get Hard version!");
    
    u16 offset = 0;
    u32 crc = 0;
    u16 ver = 0;
    
    SEND_RESPONSE_FLAG_CLEAR();
    
    offset = LEN_HEAD;
    memcpy((s8 *)SEND_RESPONSE_OFFSET(offset), buf, MAX_ADDR_LEN);
    offset += MAX_ADDR_LEN;
    /* cmd */
    memcpy((s8 *)SEND_RESPONSE_OFFSET(offset), "020", LEN_CMD);
    offset += LEN_CMD;
    
    /* version */
    ver = ConfigGetHardVer();
    ver = nhtons(ver);
    memcpy((s8 *)SEND_RESPONSE_OFFSET(offset), &ver, 2);
    offset += 2;
    
    /* HEAD */
    /* CRC */
    crc = usMBCRC16( (u8 *)SEND_RESPONSE_OFFSET(LEN_HEAD) , offset - LEN_HEAD );
    
    FormatHead(crc, offset - LEN_HEAD, (u8 *)SEND_RESPONSE_OFFSET(0));
    
    SEND_RESPONSE_SET_BYTE('\r', offset);
    SEND_RESPONSE_SET_BYTE('\n', offset + 1);
    
    /* 设置包含回车的长度，用于发送 */
    SEND_RESPONSE_SET_LEN(offset+2);
    
    /* 设置发送标志，发送程序开始发送 */
    SEND_RESPONSE_FLAG_SET();
    
    /* 需要回应 */
    SEND_RESPONSE_RESP_FALG_SET(0);
}

void ProcessReboot(u8 *buf)
{
    GSM_LOG_P0("Reboot!");
    
    u16 offset = 0;
    u32 crc = 0;
    //u16 ver = 0;
    
    SEND_RESPONSE_FLAG_CLEAR();
    
    offset = LEN_HEAD;
    memcpy((s8 *)SEND_RESPONSE_OFFSET(offset), buf, MAX_ADDR_LEN);
    offset += MAX_ADDR_LEN;
    /* cmd */
    memcpy((s8 *)SEND_RESPONSE_OFFSET(offset), CMD_CLI_REBOOT_RSP, LEN_CMD);
    offset += LEN_CMD;
    
    /* HEAD */
    /* CRC */
    crc = usMBCRC16( (u8 *)SEND_RESPONSE_OFFSET(LEN_HEAD) , offset - LEN_HEAD );
    
    FormatHead(crc, offset - LEN_HEAD, (u8 *)SEND_RESPONSE_OFFSET(0));
    
    SEND_RESPONSE_SET_BYTE('\r', offset);
    SEND_RESPONSE_SET_BYTE('\n', offset + 1);
    
    /* 设置包含回车的长度，用于发送 */
    SEND_RESPONSE_SET_LEN(offset+2);
    
    /* 设置发送标志，发送程序开始发送 */
    SEND_RESPONSE_FLAG_SET();
    
    /* 需要回应 */
    SEND_RESPONSE_RESP_FALG_SET(0);
    
    /* 重启 */
    SEDN_RESPONSE_SET_RESET_SYSTEM_FLAG();
}

/*
*  上行消息的应答需要设置应答成功标志，发送thread接到成功指示才不会关闭
*
*/
void ProcessServerResp(u8 *buf)
{
    if (GetAtStatus() == AT_WAIT_SEND_OK)
    {
        SetAtStatus(AT_SUSS);
    }
}

void ProcessAdjust(u8 *buf)
{
    u16 temp = 0;
    u16 offset = LEN_ADDR + LEN_CMD;
    memcpy(&temp, buf + offset, 2);
    offset += 2;
    ConfigSetpm25K(nhtons(temp));
    
    GSM_LOG_P1("ProcessAdjust: PM25K:%d", nhtons(temp));
    
    memcpy(&temp, buf + offset, 2);
    offset += 2;
    ConfigSetpm10K(nhtons(temp));
    
    GSM_LOG_P1("ProcessAdjust: PM10K:%d", nhtons(temp));
    
    memcpy(&temp, buf + offset, 2);
    offset += 2;
    ConfigSetCoK(nhtons(temp));
    
    GSM_LOG_P1("ProcessAdjust: COK:%d", nhtons(temp));
    
    memcpy(&temp, buf + offset, 2);
    offset += 2;
    ConfigSetSo2K(nhtons(temp));
    
    GSM_LOG_P1("ProcessAdjust: SO2K:%d", nhtons(temp));
    
    memcpy(&temp, buf + offset, 2);
    offset += 2;
    ConfigSetNo2K(nhtons(temp));
    
    GSM_LOG_P1("ProcessAdjust: NO2K:%d", nhtons(temp));
    
    memcpy(&temp, buf + offset, 2);
    offset += 2;
    ConfigSetO3K(nhtons(temp));
    
    GSM_LOG_P1("ProcessAdjust: O3K:%d", nhtons(temp));
    
    memcpy(&temp, buf + offset, 2);
    offset += 2;
    ConfigSetTmpK(nhtons(temp));
    
    GSM_LOG_P1("ProcessAdjust: TMPK:%d", nhtons(temp));
    
    // B value
    memcpy(&temp, buf + offset, 2);
    offset += 2;
    ConfigSetpm25B(nhtons(temp));
    
    GSM_LOG_P1("ProcessAdjust: PM25B:%d", nhtons(temp));

    memcpy(&temp, buf + offset, 2);
    offset += 2;
    ConfigSetpm10B(nhtons(temp));
    
    GSM_LOG_P1("ProcessAdjust: PM10B:%d", nhtons(temp));
    
    memcpy(&temp, buf + offset, 2);
    offset += 2;
    ConfigSetCoB(nhtons(temp));
    
    GSM_LOG_P1("ProcessAdjust: COB:%d", nhtons(temp));
    
    memcpy(&temp, buf + offset, 2);
    offset += 2;
    ConfigSetSo2B(nhtons(temp));
    
    GSM_LOG_P1("ProcessAdjust: SO2B:%d", nhtons(temp));
    
    memcpy(&temp, buf + offset, 2);
    offset += 2;
    ConfigSetNo2B(nhtons(temp));
    
    GSM_LOG_P1("ProcessAdjust: NO2B:%d", nhtons(temp));

    memcpy(&temp, buf + offset, 2);
    offset += 2;
    ConfigSetO3B(nhtons(temp));
    
    GSM_LOG_P1("ProcessAdjust: O3B:%d", nhtons(temp));
    
    memcpy(&temp, buf + offset, 2);
    offset += 2;
    ConfigSetTmpB(nhtons(temp));
    
    GSM_LOG_P1("ProcessAdjust: TMPB:%d", nhtons(temp));
    /* Add End */
    
    
#if 0
    /* pm10 基准电压 */
    memcpy(&temp, buf + offset, 2);
    offset += 2;
    ConfigSetpm10BaseV(nhtons(temp));
    /* pm10 基准温度 */
    memcpy(&temp, buf + offset, 2);
    offset += 2;
    ConfigSetpm10BaseC(nhtons(temp));
    /* pm10 N */
    memcpy(&temp, buf + offset, 2);
    offset += 2;
    ConfigSetpm10N(nhtons(temp));
    
    memcpy(&temp, buf + offset, 2);
    offset += 2;
    ConfigSetcoVw(nhtons(temp));
    
    memcpy(&temp, buf + offset, 2);
    offset += 2;
    ConfigSetcoVa(nhtons(temp));
    
    memcpy(&temp, buf + offset, 2);
    offset += 2;
    ConfigSetso2Vw(nhtons(temp));
    
    memcpy(&temp, buf + offset, 2);
    offset += 2;
    ConfigSetso2Va(nhtons(temp));
    
    memcpy(&temp, buf + offset, 2);
    offset += 2;
    ConfigSeto3Vw(nhtons(temp));
    
    memcpy(&temp, buf + offset, 2);
    offset += 2;
    ConfigSeto3Va(nhtons(temp));
    
    memcpy(&temp, buf + offset, 2);
    offset += 2;
    ConfigSetno2Vw(nhtons(temp));
    
    memcpy(&temp, buf + offset, 2);
    offset += 2;
    ConfigSetno2Va(nhtons(temp));
    
    memcpy(&temp, buf + offset, 2);
    offset += 2;
    ConfigSetcoS(nhtons(temp));
    
    memcpy(&temp, buf + offset, 2);
    offset += 2;
    ConfigSetso2S(nhtons(temp));
    
    memcpy(&temp, buf + offset, 2);
    offset += 2;
    ConfigSeto3S(nhtons(temp));
    
    memcpy(&temp, buf + offset, 2);
    offset += 2;
    ConfigSetno2S(nhtons(temp));
#endif
    
    /* 更新到配置文件 */
    ConfigSetUpdate(1);
    
    ConstructResponse(CMD_CLI_CFG_SERSOR_RSP, buf, 0xff);
}

void ProcessConfig(u8 *buf)
{
    u16 interval = 0;
    s32 temp = 0;
    u16 offset = LEN_ADDR + LEN_CMD;
    
    temp = stringToInt(buf + offset, LEN_ADDR);
    if (temp != 0)
    {
        ConfigSetAddr(temp);
        ConfigSetStrAddr(buf + offset);
    }
    offset += LEN_ADDR;
    
    memcpy(&interval, buf + offset, 2);
    interval = nhtons(interval);
    if (interval != 0)
    {
        ConfigSetSimpleInterval(interval);
    }
    offset += 2;
    
    memcpy(&interval, buf + offset, 2);
    interval = nhtons(interval);
    if (interval != 0)
    {
        ConfigSetReportInterval(interval);
    }
    offset += 2;
    
    /* IP 地址 */
    memcpy(&temp, buf + offset, 4);
    temp = nhtonl(temp);
    if (temp != 0)
    {
        ConfigSetServerIpInt(temp);
    }
    
    offset += 4;
    
    /* 端口号 */
    memcpy(&interval, buf + offset, 2);
    interval = nhtons(interval);
    if (interval != 0)
    {
        ConfigSetServerPort(interval);
    }
    offset += 2;
    
    GSM_LOG_P2("NEW SERVER IP: %s, port: %d", ConfigGetServerIp(), ConfigGetServerPort());
    
    /* 更新到配置文件 */
    ConfigSetUpdate(1);
    
    ConstructResponse(CMD_CLI_CFG_STATION_RSP, buf, 0xff);
}
#if 0
// version of own
void ProcessSerGetStationInfo(u8 *buf)
{
    u16 offset = 0;
    u32 crc = 0;
    s16 sVal = 0;
    u16 uVal = 0;
    
    GSM_LOG_P0("Request station info!");
    
    SEND_RESPONSE_FLAG_CLEAR();
    
    offset = LEN_HEAD;
    memcpy((s8 *)SEND_RESPONSE_OFFSET(offset), buf, MAX_ADDR_LEN);
    offset += MAX_ADDR_LEN;
    /* cmd */
    memcpy((s8 *)SEND_RESPONSE_OFFSET(offset), CMD_CLI_STATION_INFO_RSP, LEN_CMD);
    offset += LEN_CMD;
    
    /* 采集间隔 */ 
    uVal = ConfigGetSimpleInterval();
    uVal = nhtons(uVal);
    memcpy((s8 *)SEND_RESPONSE_OFFSET(offset), &uVal, 2);
    offset += 2;
    
    /* 上报间隔 */
    uVal = ConfigGetReportInterval();
    uVal = nhtons(uVal);
    memcpy((s8 *)SEND_RESPONSE_OFFSET(offset), &uVal, 2);
    offset += 2;
    
    /* soft version */
    uVal = ConfigGetSoftVer();
    uVal = nhtons(uVal);
    memcpy((s8 *)SEND_RESPONSE_OFFSET(offset), &uVal, 2);
    offset += 2;
    
    /* hard version */
    uVal = ConfigGetHardVer();
    uVal = nhtons(uVal);
    memcpy((s8 *)SEND_RESPONSE_OFFSET(offset), &uVal, 2);
    offset += 2;
    
    /* PM2.5 K */
    sVal = ConfigGetpm25K();
    sVal = nhtons(sVal);
    memcpy((s8 *)SEND_RESPONSE_OFFSET(offset), &sVal, 2);
    offset += 2;
    
    /* PM2.5 B */
    sVal = ConfigGetpm25B();
    sVal = nhtons(sVal);
    memcpy((s8 *)SEND_RESPONSE_OFFSET(offset), &sVal, 2);
    offset += 2;
    
    /* PM10 k */
    sVal = ConfigGetpm10K();
    sVal = nhtons(sVal);
    memcpy((s8 *)SEND_RESPONSE_OFFSET(offset), &sVal, 2);
    offset += 2;
    
    /* PM10 b */
    sVal = ConfigGetpm10B();
    sVal = nhtons(sVal);
    memcpy((s8 *)SEND_RESPONSE_OFFSET(offset), &sVal, 2);
    offset += 2;
    
    /* Add */
    /* CO k */
    sVal = ConfigGetCoK();
    sVal = nhtons(sVal);
    memcpy((s8 *)SEND_RESPONSE_OFFSET(offset), &sVal, 2);
    offset += 2;
    
    /* CO b */
    sVal = ConfigGetCoB();
    sVal = nhtons(sVal);
    memcpy((s8 *)SEND_RESPONSE_OFFSET(offset), &sVal, 2);
    offset += 2;
    
    /* SO2 k */
    sVal = ConfigGetSo2K();
    sVal = nhtons(sVal);
    memcpy((s8 *)SEND_RESPONSE_OFFSET(offset), &sVal, 2);
    offset += 2;
    
    /* SO2 b */
    sVal = ConfigGetSo2B();
    sVal = nhtons(sVal);
    memcpy((s8 *)SEND_RESPONSE_OFFSET(offset), &sVal, 2);
    offset += 2;
    
    /* O3 k */
    sVal = ConfigGetO3K();
    sVal = nhtons(sVal);
    memcpy((s8 *)SEND_RESPONSE_OFFSET(offset), &sVal, 2);
    offset += 2;
    
    /* O3 b */
    sVal = ConfigGetO3B();
    sVal = nhtons(sVal);
    memcpy((s8 *)SEND_RESPONSE_OFFSET(offset), &sVal, 2);
    offset += 2;
    
    /* NO2 k */
    sVal = ConfigGetNo2K();
    sVal = nhtons(sVal);
    memcpy((s8 *)SEND_RESPONSE_OFFSET(offset), &sVal, 2);
    offset += 2;
    
    /* NO2 b */
    sVal = ConfigGetNo2B();
    sVal = nhtons(sVal);
    memcpy((s8 *)SEND_RESPONSE_OFFSET(offset), &sVal, 2);
    offset += 2;
    
    /* Tmp k */
    sVal = ConfigGetTmpK();
    sVal = nhtons(sVal);
    memcpy((s8 *)SEND_RESPONSE_OFFSET(offset), &sVal, 2);
    offset += 2;
    
    /* Tmp b */
    sVal = ConfigGetTmpB();
    sVal = nhtons(sVal);
    memcpy((s8 *)SEND_RESPONSE_OFFSET(offset), &sVal, 2);
    offset += 2;
    /* Add end */
    
    /* PM10BaseV */
    uVal = ConfigGetpm10BaseV();
    uVal = nhtons(uVal);
    memcpy((s8 *)SEND_RESPONSE_OFFSET(offset), &uVal, 2);
    offset += 2;
    
    /* PM10BaseC */
    uVal = ConfigGetpm10BaseC();
    uVal = nhtons(uVal);
    memcpy((s8 *)SEND_RESPONSE_OFFSET(offset), &uVal, 2);
    offset += 2;
    
    /* PM10N */
    uVal = ConfigGetpm10N();
    uVal = nhtons(uVal);
    memcpy((s8 *)SEND_RESPONSE_OFFSET(offset), &uVal, 2);
    offset += 2;
    
    /* PM10N */
    uVal = ConfigGetpm10N();
    uVal = nhtons(uVal);
    memcpy((s8 *)SEND_RESPONSE_OFFSET(offset), &uVal, 2);
    offset += 2;
    
    /* coVw */
    uVal = ConfigGetcoVw();
    uVal = nhtons(uVal);
    memcpy((s8 *)SEND_RESPONSE_OFFSET(offset), &uVal, 2);
    offset += 2;
    
    /* coVa */
    uVal = ConfigGetcoVa();
    uVal = nhtons(uVal);
    memcpy((s8 *)SEND_RESPONSE_OFFSET(offset), &uVal, 2);
    offset += 2;
    
    /* so2Vw */
    uVal = ConfigGetso2Vw();
    uVal = nhtons(uVal);
    memcpy((s8 *)SEND_RESPONSE_OFFSET(offset), &uVal, 2);
    offset += 2;
    
    /* so2Va */
    uVal = ConfigGetso2Va();
    uVal = nhtons(uVal);
    memcpy((s8 *)SEND_RESPONSE_OFFSET(offset), &uVal, 2);
    offset += 2;
    
    /* o3Vw */
    uVal = ConfigGeto3Vw();
    uVal = nhtons(uVal);
    memcpy((s8 *)SEND_RESPONSE_OFFSET(offset), &uVal, 2);
    offset += 2;
    
    /* o3Va */
    uVal = ConfigGeto3Va();
    uVal = nhtons(uVal);
    memcpy((s8 *)SEND_RESPONSE_OFFSET(offset), &uVal, 2);
    offset += 2;
    
    /* no2Vw */
    uVal = ConfigGetno2Vw();
    uVal = nhtons(uVal);
    memcpy((s8 *)SEND_RESPONSE_OFFSET(offset), &uVal, 2);
    offset += 2;
    
    /* no2Va */
    uVal = ConfigGetno2Va();
    uVal = nhtons(uVal);
    memcpy((s8 *)SEND_RESPONSE_OFFSET(offset), &uVal, 2);
    offset += 2;
    
    /* coS */
    sVal = ConfigGetcoS();
    sVal = nhtons(sVal);
    memcpy((s8 *)SEND_RESPONSE_OFFSET(offset), &sVal, 2);
    offset += 2;
    
    /* so2S */
    sVal = ConfigGetso2S();
    sVal = nhtons(sVal);
    memcpy((s8 *)SEND_RESPONSE_OFFSET(offset), &sVal, 2);
    offset += 2;
    
    /* o3S */
    sVal = ConfigGeto3S();
    sVal = nhtons(sVal);
    memcpy((s8 *)SEND_RESPONSE_OFFSET(offset), &sVal, 2);
    offset += 2;
    
    /* no2S */
    sVal = ConfigGetno2S();
    sVal = nhtons(sVal);
    memcpy((s8 *)SEND_RESPONSE_OFFSET(offset), &sVal, 2);
    offset += 2;
    
    /* HEAD */
    /* CRC */
    crc = usMBCRC16( (u8 *)SEND_RESPONSE_OFFSET(LEN_HEAD) , offset - LEN_HEAD );
    
    FormatHead(crc, offset - LEN_HEAD, (u8 *)SEND_RESPONSE_OFFSET(0));
    
    SEND_RESPONSE_SET_BYTE('\r', offset);
    SEND_RESPONSE_SET_BYTE('\n', offset + 1);
    
    /* 设置包含回车的长度，用于发送 */
    SEND_RESPONSE_SET_LEN(offset+2);
    
    /* 设置发送标志，发送程序开始发送 */
    SEND_RESPONSE_FLAG_SET();
    
    /* 需要回应 */
    SEND_RESPONSE_RESP_FALG_SET(0);
}
#endif

void ProcessSerGetStationInfo(u8 *buf)
{
    u16 offset = 0;
    u32 crc = 0;
    s16 sVal = 0;
    u16 uVal = 0;
    
    GSM_LOG_P0("Request station info!");
    
    SEND_RESPONSE_FLAG_CLEAR();
    
    offset = LEN_HEAD;
    memcpy((s8 *)SEND_RESPONSE_OFFSET(offset), buf, MAX_ADDR_LEN);
    offset += MAX_ADDR_LEN;
    /* cmd */
    memcpy((s8 *)SEND_RESPONSE_OFFSET(offset), CMD_CLI_STATION_INFO_RSP, LEN_CMD);
    offset += LEN_CMD;
    
    /* 采集间隔 */ 
    uVal = ConfigGetSimpleInterval();
    uVal = nhtons(uVal);
    memcpy((s8 *)SEND_RESPONSE_OFFSET(offset), &uVal, 2);
    offset += 2;
    
    /* 上报间隔 */
    uVal = ConfigGetReportInterval();
    uVal = nhtons(uVal);
    memcpy((s8 *)SEND_RESPONSE_OFFSET(offset), &uVal, 2);
    offset += 2;
    
    /* soft version */
    uVal = ConfigGetSoftVer();
    uVal = nhtons(uVal);
    memcpy((s8 *)SEND_RESPONSE_OFFSET(offset), &uVal, 2);
    offset += 2;
    
    /* hard version */
    uVal = ConfigGetHardVer();
    uVal = nhtons(uVal);
    memcpy((s8 *)SEND_RESPONSE_OFFSET(offset), &uVal, 2);
    offset += 2;
    
    /* PM2.5 K */
    sVal = ConfigGetpm25K();
    sVal = nhtons(sVal);
    memcpy((s8 *)SEND_RESPONSE_OFFSET(offset), &sVal, 2);
    offset += 2;
    
    /* PM10 k */
    sVal = ConfigGetpm10K();
    sVal = nhtons(sVal);
    memcpy((s8 *)SEND_RESPONSE_OFFSET(offset), &sVal, 2);
    offset += 2;
    
    /* Add */
    /* CO k */
    sVal = ConfigGetCoK();
    sVal = nhtons(sVal);
    memcpy((s8 *)SEND_RESPONSE_OFFSET(offset), &sVal, 2);
    offset += 2;
    
    /* SO2 k */
    sVal = ConfigGetSo2K();
    sVal = nhtons(sVal);
    memcpy((s8 *)SEND_RESPONSE_OFFSET(offset), &sVal, 2);
    offset += 2;
    
    /* NO2 k */
    sVal = ConfigGetNo2K();
    sVal = nhtons(sVal);
    memcpy((s8 *)SEND_RESPONSE_OFFSET(offset), &sVal, 2);
    offset += 2;
    
    /* O3 k */
    sVal = ConfigGetO3K();
    sVal = nhtons(sVal);
    memcpy((s8 *)SEND_RESPONSE_OFFSET(offset), &sVal, 2);
    offset += 2;
    
    /* Tmp k */
    sVal = ConfigGetTmpK();
    sVal = nhtons(sVal);
    memcpy((s8 *)SEND_RESPONSE_OFFSET(offset), &sVal, 2);
    offset += 2;
    
    /* PM2.5 B */
    sVal = ConfigGetpm25B();
    sVal = nhtons(sVal);
    memcpy((s8 *)SEND_RESPONSE_OFFSET(offset), &sVal, 2);
    offset += 2;
    
    /* PM10 b */
    sVal = ConfigGetpm10B();
    sVal = nhtons(sVal);
    memcpy((s8 *)SEND_RESPONSE_OFFSET(offset), &sVal, 2);
    offset += 2;

    /* CO b */
    sVal = ConfigGetCoB();
    sVal = nhtons(sVal);
    memcpy((s8 *)SEND_RESPONSE_OFFSET(offset), &sVal, 2);
    offset += 2;
    
    /* SO2 b */
    sVal = ConfigGetSo2B();
    sVal = nhtons(sVal);
    memcpy((s8 *)SEND_RESPONSE_OFFSET(offset), &sVal, 2);
    offset += 2;
    
    /* NO2 b */
    sVal = ConfigGetNo2B();
    sVal = nhtons(sVal);
    memcpy((s8 *)SEND_RESPONSE_OFFSET(offset), &sVal, 2);
    offset += 2;
    
    /* O3 b */
    sVal = ConfigGetO3B();
    sVal = nhtons(sVal);
    memcpy((s8 *)SEND_RESPONSE_OFFSET(offset), &sVal, 2);
    offset += 2;
    
    /* Tmp b */
    sVal = ConfigGetTmpB();
    sVal = nhtons(sVal);
    memcpy((s8 *)SEND_RESPONSE_OFFSET(offset), &sVal, 2);
    offset += 2;
    /* Add end */
    
    /* HEAD */
    /* CRC */
    crc = usMBCRC16( (u8 *)SEND_RESPONSE_OFFSET(LEN_HEAD) , offset - LEN_HEAD );
    
    FormatHead(crc, offset - LEN_HEAD, (u8 *)SEND_RESPONSE_OFFSET(0));
    
    SEND_RESPONSE_SET_BYTE('\r', offset);
    SEND_RESPONSE_SET_BYTE('\n', offset + 1);
    
    /* 设置包含回车的长度，用于发送 */
    SEND_RESPONSE_SET_LEN(offset+2);
    
    /* 设置发送标志，发送程序开始发送 */
    SEND_RESPONSE_FLAG_SET();
    
    /* 需要回应 */
    SEND_RESPONSE_RESP_FALG_SET(0);
}

void ProcessGetPosition(u8 *buf)
{
    u16 offset = 0;
    u32 crc = 0;
    u32 val = 0;
    
    GSM_LOG_P0("Request Position!");
    
    SEND_RESPONSE_FLAG_CLEAR();
    
    offset = LEN_HEAD;
    memcpy((s8 *)SEND_RESPONSE_OFFSET(offset), buf, MAX_ADDR_LEN);
    offset += MAX_ADDR_LEN;
    /* cmd */
    memcpy((s8 *)SEND_RESPONSE_OFFSET(offset), "506", LEN_CMD);
    offset += LEN_CMD;
    
    /* version */
    val = GetCoordLong();
    val = nhtonl(val);
    memcpy((s8 *)SEND_RESPONSE_OFFSET(offset), &val, 4);
    offset += 4;
    
    val = GetCoordLati();
    val = nhtonl(val);
    memcpy((s8 *)SEND_RESPONSE_OFFSET(offset), &val, 4);
    offset += 4;
    
    /* HEAD */
    /* CRC */
    crc = usMBCRC16( (u8 *)SEND_RESPONSE_OFFSET(LEN_HEAD) , offset - LEN_HEAD );
    
    FormatHead(crc, offset - LEN_HEAD, (u8 *)SEND_RESPONSE_OFFSET(0));
    
    SEND_RESPONSE_SET_BYTE('\r', offset);
    SEND_RESPONSE_SET_BYTE('\n', offset + 1);
    
    /* 设置包含回车的长度，用于发送 */
    SEND_RESPONSE_SET_LEN(offset+2);
    
    /* 设置发送标志，发送程序开始发送 */
    SEND_RESPONSE_FLAG_SET();
    
    /* 需要回应 */
    SEND_RESPONSE_RESP_FALG_SET(0);
}

/*
typedef struct RECALL_INFO_t
{
    u8 flag;                    / 是否需要回调的标志 /
    u8 continueFlag;            / 对于天回调，要构造多次数据 /
    u16 type;                   / 回调类型 /
    u8 startTime[15];           / 1608100828 /
    u8 endTime[15];             / 1608100829 /
    u16 startyear;
    u8 startmon;
    u8 startday;
    u8 starthour;
    u16 endyear;
    u8 endmon;
    u8 endday;
    u8 endhour;
    
}RECALL_INFO_t;
*/
void ProcessRecall(u8 *buf)
{
    u8 offset = 0;
    u16 cnt = 0;
    u16 y, m;
    
    if (IsInRecall())
    {
        GSM_LOG_P1("Recall In Process! Ignored! %s", buf);
        return;
    }
    
    memset(&g_recallInfo, 0, sizeof(g_recallInfo));
    
    offset = LEN_ADDR + LEN_CMD;
    memcpy(g_recallInfo.startTime, buf + offset, 14);
    offset += 14;
    
    memcpy(g_recallInfo.endTime, buf + offset, 14);
    offset += 14;
    
    // start time
    offset = LEN_ADDR + LEN_CMD;
    g_recallInfo.startyear = (u16)stringToInt(buf + offset, 4);
    offset += 4;
    g_recallInfo.startmon = (u8)stringToInt(buf + offset, 2);
    offset += 2;
    g_recallInfo.startday = (u8)stringToInt(buf + offset, 2);
    offset += 2;
    g_recallInfo.starthour = (u8)stringToInt(buf + offset, 2);
    offset += 2;
    
    // now time
    g_recallInfo.nowyear = g_recallInfo.startyear;
    g_recallInfo.nowmon = g_recallInfo.startmon;
    g_recallInfo.nowday = g_recallInfo.startday;
    g_recallInfo.nowhour = g_recallInfo.starthour;
    
    // end time
    offset += 4;
    g_recallInfo.endyear = (u16)stringToInt(buf + offset, 4);
    offset += 4;
    g_recallInfo.endmon = (u8)stringToInt(buf + offset, 2);
    offset += 2;
    g_recallInfo.endday = (u8)stringToInt(buf + offset, 2);
    offset += 2;
    g_recallInfo.endhour = (u8)stringToInt(buf + offset, 2);
    
    /* 最多回调6个月的数据 */
    m = g_recallInfo.startmon;
    
    for (y = g_recallInfo.startyear; y < g_recallInfo.endyear; y++)
    {
        for (; m <= 12; m++)
        {
            cnt++;
        }
        
        m = 1;
    }
    
    /* 跨年的情况 */
    if (cnt)
    {
        m = 1;
    }
    
    for (; m <= g_recallInfo.endmon; m++)
    {
        cnt++;
    }
    
    /* 只计算月，不计算天，实际会多于6个月数据 */
    if (cnt > 6)
    {
        ConstructCommonResponse(CMD_CLI_RECALL_DATA_RSP, buf, CODE_RECALL_ERR2, 2);
        return;
    }
    
    g_recallInfo.flag = 1;
    g_recallInfo.continueFlag = 0;
    
    GSM_LOG_P1("Recall startTime: %s", g_recallInfo.startTime);
    GSM_LOG_P1("Recall EndTime: %s", g_recallInfo.endTime);
    GSM_LOG_P4("StartValue: %04d%02d%02d %02d", 
               g_recallInfo.startyear, g_recallInfo.startmon, 
               g_recallInfo.startday, g_recallInfo.starthour);
    GSM_LOG_P4("EndValue: %04d%02d%02d %02d", 
               g_recallInfo.endyear, g_recallInfo.endmon, 
               g_recallInfo.endday, g_recallInfo.endhour);
   
}

/* 关闭回调 */
void ProcessStopRecall(u8 *buf)
{
    if (!IsInRecall())
    {
        GSM_LOG_P1("There is no recall! Ignored! %s", buf);
        return;
    }
    
    g_recallInfo.stopflag = 1;
}

/*
*  Construct response.
*  For Type != 0xff, will be put into buffer
*  所有的回应用下发时的地址，避免服务器修改地址后用新的地址回应
*/ 
void ConstructResponse(u8 *cmd, u8* addr, u16 type)
{
    u16 offset = 0;
    u32 crc = 0;
    
    SEND_RESPONSE_FLAG_CLEAR();
    
    offset = LEN_HEAD;
    memcpy((s8 *)SEND_RESPONSE_OFFSET(offset), addr, MAX_ADDR_LEN);
    offset += MAX_ADDR_LEN;
    /* cmd */
    memcpy((s8 *)SEND_RESPONSE_OFFSET(offset), cmd, LEN_CMD);
    offset += LEN_CMD;
    if (type != 0xff)
    {
        SEND_RESPONSE_SET_BYTE(type, offset);
        offset += 1;
    }
    
    /* HEAD */
    /* CRC */
    crc = usMBCRC16( (u8 *)SEND_RESPONSE_OFFSET(LEN_HEAD) , offset - LEN_HEAD );
    
    FormatHead(crc, offset - LEN_HEAD, (u8 *)SEND_RESPONSE_OFFSET(0));
    
    SEND_RESPONSE_SET_BYTE('\r', offset);
    SEND_RESPONSE_SET_BYTE('\n', offset + 1);
    
    /* 设置包含回车的长度，用于发送 */
    SEND_RESPONSE_SET_LEN(offset+2);
    
    /* 设置发送标志，发送程序开始发送 */
    SEND_RESPONSE_FLAG_SET();
    
    /* 设置是否需要回应 */
    SEND_RESPONSE_RESP_FALG_SET(0);
}

/*
*  构造回应数据。 
*  CC **** XX CC 60001 216 01
*
*/
void ConstructCommonResponse(u8 *cmd, u8* addr, u8 *opt, u8 optLen)
{
    u16 offset = 0;
    u32 crc = 0;
    
    SEND_RESPONSE_FLAG_CLEAR();
    
    offset = LEN_HEAD;
    memcpy((s8 *)SEND_RESPONSE_OFFSET(offset), addr, MAX_ADDR_LEN);
    offset += MAX_ADDR_LEN;
    /* cmd */
    memcpy((s8 *)SEND_RESPONSE_OFFSET(offset), cmd, LEN_CMD);
    offset += LEN_CMD;
    
    /* opt */
    memcpy((s8 *)SEND_RESPONSE_OFFSET(offset), opt, optLen);
    offset += optLen;
    
    /* HEAD */
    /* CRC */
    crc = usMBCRC16( (u8 *)SEND_RESPONSE_OFFSET(LEN_HEAD) , offset - LEN_HEAD );
    
    FormatHead(crc, offset - LEN_HEAD, (u8 *)SEND_RESPONSE_OFFSET(0));
    
    SEND_RESPONSE_SET_BYTE('\r', offset);
    SEND_RESPONSE_SET_BYTE('\n', offset + 1);
    
    /* 设置包含回车的长度，用于发送 */
    SEND_RESPONSE_SET_LEN(offset+2);
    
    /* 设置发送标志，发送程序开始发送 */
    SEND_RESPONSE_FLAG_SET();
    
    /* 设置是否需要回应 */
    SEND_RESPONSE_RESP_FALG_SET(0);
}

