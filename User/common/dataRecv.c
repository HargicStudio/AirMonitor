#include "format.h"
#include "dataRecv.h"
#include "AaInclude.h"
#include "crc.h"
#include "gsmCtrl.h"


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
      /* 服务器呼叫 */
    case 1:
      ProcessServerCall(buf);
      break;
      /* 掉电模式 */
    case 5:
      ProcessSavePower(buf);
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
    case 12:
      ProcessChangeReportInterval(buf);
      break;
      /* 读取传感器状态 */
    case 15:
      ProcessChangeGetModuleStatus(buf);
      break;
      /* 请求软件版本号 */
    case 19:
      ProcessGetSoftVersion(buf);
      break;
      /* 请求硬件版本号 */
    case 33:
      ProcessGetHardVersion(buf);
      /* 应答处理 */
    case 500:
      ProcessServerResp(buf);
      break;
      /* 校准参数 */
    case 501:
      ProcessAdjust(buf);
      break;
      /* 配置参数 */
    case 502:
      ProcessConfig(buf);
      break;
        
    }
}

u32 myPow(u32 val, u32 times)
{
    u32 rst = 0;
    if (times == 0)
      return 1;
    
    rst = val;
    while(--times)
    {
        rst *= val;  
    }
    
    return rst;
}

void ProcessServerCall(u8 *buf)
{
    GSM_LOG_P0("Recv Server Call!");
    ConstructResponse("002", buf, 0xff);
}

void ProcessSavePower(u8 *buf)
{
    GSM_LOG_P0("Recv Save Power!");
    ConstructResponse("006", buf, 0xff);
    GsmWaitCloseFlagSet();
}

void ProcessChangeAddr(u8 *buf)
{
    u32 paramAddr = 0;
    
    GSM_LOG_P0("Recv Change ADDR!");
    
    /* 更新config变量 */
    ConfigSetStrAddr(buf + LEN_ADDR + LEN_CMD);
    paramAddr = stringToInt((u8 *)(buf + LEN_ADDR + LEN_CMD), LEN_ADDR);
    ConfigSetAddr(paramAddr);
    /* 更新到配置文件 */
    
    /* 回应 */
    ConstructResponse("008", buf, 0xff);
}

void ProcessChangeSimpleInterval(u8 *buf)
{
    u8 timeLen = 0;
    
    GSM_LOG_P0("Recv change simple interval!");
    /* 更新config变量 */
    timeLen = (u8)stringToInt(buf + LEN_ADDR + LEN_CMD, 2);
    ConfigSetSimpleInterval(timeLen);
    /* 回应 */
    ConstructResponse("012", buf, 0xff);
}

void ProcessChangeReportInterval(u8 *buf)
{
    u8 timeLen = 0;
    
    GSM_LOG_P0("Recv change report interval!");
    
    /* 更新config变量 */
    timeLen = (u8)stringToInt(buf + LEN_ADDR + LEN_CMD, 2);
    ConfigSetReportInterval(timeLen);
    /* 回应 */
    ConstructResponse("014", buf, 0xff);
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

/*
*  上行消息的应答需要设置应答成功标志，发送thread接到成功指示才不会关闭
*
*/
void ProcessServerResp(u8 *buf)
{
    u8 reason = buf[LEN_ADDR + LEN_CMD];
    
    switch(reason)
    {
    case REPLY_501:
    case REPLY_502:
      if (GetAtStatus() == AT_WAIT_SEND_OK)
      {
          SetAtStatus(AT_SUSS);
      }
      break;
    default:
      break;
    }
}

void ProcessAdjust(u8 *buf)
{
    u16 temp = 0;
    u16 offset = LEN_ADDR + LEN_CMD;
    memcpy(&temp, buf + offset, 2);
    offset += 2;
    ConfigSetpm25K(nhtons(temp));
    
    memcpy(&temp, buf + offset, 2);
    offset += 2;
    ConfigSetpm25B(nhtons(temp));
    
    memcpy(&temp, buf + offset, 2);
    offset += 2;
    ConfigSetpm10K(nhtons(temp));
    
    memcpy(&temp, buf + offset, 2);
    offset += 2;
    ConfigSetpm10B(nhtons(temp));
    
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
    
    ConstructResponse("500", buf, REPLY_501);
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
    if (interval != 0)
    {
        ConfigSetSimpleInterval(nhtons(interval));
    }
    offset += 2;
    
    memcpy(&interval, buf + offset, 2);
    if (interval != 0)
    {
        ConfigSetSimpleInterval(nhtons(interval));
    }
    offset += 2;
    
    /* IP 地址 */
    memcpy(&temp, buf + offset, 4);
    temp = nhtonl(temp);
    ConfigSetServerIpInt(temp);
    offset += 4;
    
    /* 端口号 */
    memcpy(&interval, buf + offset, 2);
    if (interval != 0)
    {
        ConfigSetServerPort(nhtons(interval));
    }
    offset += 2;
    
    ConstructResponse("500", buf, REPLY_502);
}

/*
*  Construct response.
*  For Type != 0xff, will be put into buffer
*  所有的回应用下发时的地址，避免服务器修改地址后用新的地址回应
*/ 
void ConstructResponse(u8 *cmd, u8* addr, u8 type)
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

