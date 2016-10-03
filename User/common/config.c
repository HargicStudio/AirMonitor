#include "config.h"
#include <string.h>
#include "gpsAnalyser.h"
#include "common.h"
#include "cfg.h"
#include "rtc_dev.h"


/* 用于计算和保存信息 */
extern gps_process_data gps;

CONFIG_t g_config;

char _cfgBuf[20];

/* 读取配置是否改变 */
u8 IsConfigUpdated()
{
    return g_config.updateFlag;
}

void ConfigSetUpdate(u8 flag)
{
    g_config.updateFlag = flag;
}
/*
*  配置项 80000 作为保留地址 
*
*/
void ConfigInit(void)
{
    int temp32 = 0;
    
    if (-1 != ReadCfg(C_ADDR, _cfgBuf))
    {
        ConfigSetStrAddr((u8 *)_cfgBuf);
    }
    else
    {
        ConfigSetStrAddr((u8 *)"80000");
    }
    
    if (-1 != ReadCfgInt(C_ADDR, &temp32))
    {
        ConfigSetAddr(temp32);
    }
    else
    {
        ConfigSetAddr(80000);
    }
    
    if (-1 != ReadCfgInt(C_SOFTVER, &temp32))
    {
        ConfigSetSoftVer(temp32);
    }
    else
    {
        ConfigSetSoftVer(301);
    }
    
    if (-1 != ReadCfgInt(C_HARDVER, &temp32))
    {
        ConfigSetHardVer(temp32);
    }
    else
    {
        ConfigSetHardVer(401);
    }

    if (-1 != ReadCfg(C_SERVERIP, _cfgBuf))
    {
        ConfigSetServerIp((u8 *)_cfgBuf, strlen(_cfgBuf));
    }
    else
    {
        ConfigSetServerIp("120.27.26.208", 13);
    }

    if (-1 != ReadCfgInt(C_SERVERPORT, &temp32))
    {
        ConfigSetServerPort(temp32);
    }
    else
    {
        ConfigSetServerPort(21006);
    }
    
    if (-1 != ReadCfgInt(C_SIMPLEINT, &temp32))
    {
        ConfigSetSimpleInterval(temp32);
    }
    else
    {
        ConfigSetSimpleInterval(5);
    }
    
    if (-1 != ReadCfgInt(C_REPORTINT, &temp32))
    {
        ConfigSetReportInterval(temp32);
    }
    else
    {
        ConfigSetReportInterval(5);
    }
    
    if (-1 != ReadCfgInt(C_K25, &temp32))
    {
        ConfigSetpm25K(temp32);
    }
    else
    {
        ConfigSetpm25K(1);
    }
    
    if (-1 != ReadCfgInt(C_B25, &temp32))
    {
        ConfigSetpm25B(temp32);
    }
    else
    {
        ConfigSetpm25B(1);
    }
    
    if (-1 != ReadCfgInt(C_K10, &temp32))
    {
        ConfigSetpm10K(temp32);
    }
    else
    {
        ConfigSetpm10K(1);
    }
    
    if (-1 != ReadCfgInt(C_B10, &temp32))
    {
        ConfigSetpm10B(temp32);
    }
    else
    {
        ConfigSetpm10B(1);
    }
    
    if (-1 != ReadCfgInt(C_PM10BaseV, &temp32))
    {
        ConfigSetpm10BaseV(temp32);
    }
    else
    {
        ConfigSetpm10BaseV(1000);
    }
    
    if (-1 != ReadCfgInt(C_PM10BaseC, &temp32))
    {
        ConfigSetpm10BaseC(temp32);
    }
    else
    {
        ConfigSetpm10BaseC(250);
    }
    
    if (-1 != ReadCfgInt(C_PM10N, &temp32))
    {
        ConfigSetpm10N(temp32);
    }
    else
    {
        ConfigSetpm10N(60);
    }
    
    if (-1 != ReadCfgInt(C_COVw, &temp32))
    {
        ConfigSetcoVw(temp32);
    }
    else
    {
        ConfigSetcoVw(400);
    }
    
    if (-1 != ReadCfgInt(C_COVa, &temp32))
    {
        ConfigSetcoVa(temp32);
    }
    else
    {
        ConfigSetcoVa(300);
    }
    
    if (-1 != ReadCfgInt(C_coS, &temp32))
    {
        ConfigSetcoS(temp32);
    }
    else
    {
        ConfigSetcoS(300);
    }
    
    if (-1 != ReadCfgInt(C_SO2Vw, &temp32))
    {
        ConfigSetso2Vw(temp32);
    }
    else
    {
        ConfigSetso2Vw(400);
    }
    
    if (-1 != ReadCfgInt(C_SO2Va, &temp32))
    {
        ConfigSetso2Va(temp32);
    }
    else
    {
        ConfigSetso2Va(300);
    }
    
    if (-1 != ReadCfgInt(C_so2S, &temp32))
    {
        ConfigSetso2S(temp32);
    }
    else
    {
        ConfigSetso2S(300);
    }
    
    if (-1 != ReadCfgInt(C_O3Vw, &temp32))
    {
        ConfigSeto3Vw(temp32);
    }
    else
    {
        ConfigSeto3Vw(400);
    }
    
    if (-1 != ReadCfgInt(C_O3Va, &temp32))
    {
        ConfigSeto3Va(temp32);
    }
    else
    {
        ConfigSeto3Va(300);
    }
    
    if (-1 != ReadCfgInt(C_o3S, &temp32))
    {
        ConfigSeto3S(temp32);
    }
    else
    {
        ConfigSeto3S(300);
    }
    
    if (-1 != ReadCfgInt(C_NO2Vw, &temp32))
    {
        ConfigSetno2Vw(temp32);
    }
    else
    {
        ConfigSetno2Vw(400);
    }
    
    if (-1 != ReadCfgInt(C_NO2Va, &temp32))
    {
        ConfigSetno2Va(temp32);
    }
    else
    {
        ConfigSetno2Va(300);
    }
    
    if (-1 != ReadCfgInt(C_no2S, &temp32))
    {
        ConfigSetno2S(temp32);
    }
    else
    {
        ConfigSetno2S(400);
    }
    
    /* Ĭ�ϵ�ʱ�� */
    ConfigSetTime();
    
    ConfigPrint();
    
}

void ConfigUpdate(void)
{
    FIL pf;
    s32 ret;
    
    ret = OpenCfgFile(FA_READ | FA_WRITE | FA_CREATE_ALWAYS, &pf);
    if (ret != FR_OK)
    {
        GSM_LOG_P0("create cfg file error!");
        return;
    }
    
    f_printf(&pf, "%s=%d\r\n", default_key[C_ADDR], g_config.myAddr);
    
    f_printf(&pf, "%s=%d\r\n", default_key[C_SOFTVER], g_config.softVer);
    
    f_printf(&pf, "%s=%d\r\n", default_key[C_HARDVER], g_config.hardVer);
    
    f_printf(&pf, "%s=%s\r\n", default_key[C_SERVERIP], g_config.serverIp);
    
    f_printf(&pf, "%s=%d\r\n", default_key[C_SERVERPORT], g_config.serverPort);
    
    f_printf(&pf, "%s=%d\r\n", default_key[C_SIMPLEINT], g_config.simpleInterval);
    
    f_printf(&pf, "%s=%d\r\n", default_key[C_REPORTINT], g_config.reportInterval);
    
    f_printf(&pf, "%s=%d\r\n", default_key[C_K25], g_config.pm25K);
    
    f_printf(&pf, "%s=%d\r\n", default_key[C_B25], g_config.pm25B);
    
    f_printf(&pf, "%s=%d\r\n", default_key[C_K10], g_config.pm10K);
    
    f_printf(&pf, "%s=%d\r\n", default_key[C_B10], g_config.pm10B);
    
    f_printf(&pf, "%s=%d\r\n", default_key[C_COVw], g_config.coVw);
    
    f_printf(&pf, "%s=%d\r\n", default_key[C_COVa], g_config.coVa);
    
    f_printf(&pf, "%s=%d\r\n", default_key[C_coS], g_config.coS);
    
    f_printf(&pf, "%s=%d\r\n", default_key[C_SO2Vw], g_config.so2Vw);
    
    f_printf(&pf, "%s=%d\r\n", default_key[C_SO2Va], g_config.so2Va);
    
    f_printf(&pf, "%s=%d\r\n", default_key[C_so2S], g_config.so2S);
    
    f_printf(&pf, "%s=%d\r\n", default_key[C_O3Vw], g_config.o3Vw);
    
    f_printf(&pf, "%s=%d\r\n", default_key[C_O3Va], g_config.o3Va);
    
    f_printf(&pf, "%s=%d\r\n", default_key[C_o3S], g_config.o3S);
    
    f_printf(&pf, "%s=%d\r\n", default_key[C_NO2Vw], g_config.no2Vw);
    
    f_printf(&pf, "%s=%d\r\n", default_key[C_NO2Va], g_config.no2Va);
    
    f_printf(&pf, "%s=%d\r\n", default_key[C_no2S], g_config.no2S);
    
    
    OSA_fileSync(&pf);
    
    CloseCfgFile(&pf);
    
    ConfigPrint();
    
}
void ConfigPrint(void)
{
    GSM_LOG_P2("Config Addr: %d, SADDR: %s\r\n", g_config.myAddr, g_config.strAddr);
    GSM_LOG_P2("SOFT VER: %d, HARD VER: %d\r\n", g_config.softVer, g_config.hardVer);
    GSM_LOG_P2("Cloud: %s, PORT: %d\r\n", g_config.serverIp, g_config.serverPort);
    GSM_LOG_P2("SIMPLE: %d, REPORT: %d\r\n", g_config.simpleInterval, g_config.reportInterval);
    GSM_LOG_P4("2.5 K,B : %d,%d; 10K,B : %d,%d\r\n", g_config.pm25K, g_config.pm25B,
               g_config.pm10K, g_config.pm10B);
    GSM_LOG_P3("PM10: BaseV(mv): %d, BaseC(/10): %d, N(/100): %d",
               g_config.pm10BaseV, g_config.pm10BaseC, g_config.pm10N);
    GSM_LOG_P4("O3 W,A : %d,%d; NO2 W,A : %d,%d\r\n", g_config.o3Vw, g_config.o3Va,
               g_config.no2Vw, g_config.no2Va);
    GSM_LOG_P4("COS: %d, SO2S: %d, O3S: %d, NO2S: %d", g_config.coS, g_config.so2S,
               g_config.o3S, g_config.no2S);
}

void ConfigSetTime()
{
    gps.utc.year = 2016;
    gps.utc.month = 8;
    gps.utc.date = 7;
    gps.utc.hour = 16;
    gps.utc.min = 16;
    gps.utc.sec = 16;
    memcpy(gps.utc.strTime, "20160807161616", 15);
}

u8 ConfigSetRTCTime(u16 y, u8 m, u8 d, u8 h, u8 min, u8 s)
{
    /* ����ʱ�䵽RTC */
    gps.time.tm_year = y;
    gps.time.tm_mon = m;
    gps.time.tm_mday = d;
    gps.time.tm_wday = 0;
    gps.time.tm_hour = h;
    gps.time.tm_min = min;
    gps.time.tm_sec = s;
  
    return RTC_SetCalendar(gps.time);
}

void ConfigSetAddr(u32 addr)
{
    g_config.myAddr = addr;
}

u32 ConfigGetAddr(void)
{
    return g_config.myAddr;
}

void ConfigSetStrAddr(u8 *buf)
{
    memcpy(g_config.strAddr, buf, MAX_ADDR_LEN);
}

u8 *ConfigGetStrAddr(void)
{
    return g_config.strAddr;
}

u8 ByteToNetStr(u8 val, u8 *buf)
{
    u8 temp = 0;
    u8 offset = 0;
    u8 flag = 0;
    
    temp = val / 100;
    if (temp)
    {
        flag = 1;
        buf[offset] = temp + '0';
        offset++;
    }
    
    temp = (val % 100) / 10;
    if (temp || flag == 1)
    {
        buf[offset] = temp + '0';
        offset++;
    }
    
    temp = (val % 10);
    buf[offset] = temp + '0';
    offset++;
    
    return offset;
}

void ConfigSetServerIpInt(s32 ip)
{
    u8 temp = 0;
    u8 offset = 0;
    u8 ret = 0;
    
    temp = (ip >> 24) & 0xff;
    ret = ByteToNetStr(temp, g_config.serverIp + offset);
    offset += ret;
    g_config.serverIp[offset] = '.';
    offset += 1;
    
    temp = (ip >> 16) & 0xff;
    ret = ByteToNetStr(temp, g_config.serverIp + offset);
    offset += ret;
    g_config.serverIp[offset] = '.';
    offset += 1;
    
    temp = (ip >> 8) & 0xff;
    ret = ByteToNetStr(temp, g_config.serverIp + offset);
    offset += ret;
    g_config.serverIp[offset] = '.';
    offset += 1;
    
    temp = ip & 0xff;
    ret = ByteToNetStr(temp, g_config.serverIp + offset);
    offset += ret;
    g_config.serverIp[offset] = 0;
    
}

void ConfigSetServerIp(u8 *buf, u8 len)
{
    memset((s8 *)g_config.serverIp, 0, sizeof(g_config.serverIp));
    memcpy((s8 *)g_config.serverIp, (s8 *)buf, (len > MAX_IP_LEN) ? MAX_IP_LEN : len);
}

u8 *ConfigGetServerIp(void)
{
    return g_config.serverIp;
}

void ConfigSetServerPort(u16 port)
{
    g_config.serverPort = port;
}

u16 ConfigGetServerPort(void)
{
    return g_config.serverPort;
}

void ConfigSetSoftVer(u16 ver)
{
    g_config.softVer = ver;
}

u16 ConfigGetSoftVer(void)
{
    return g_config.softVer;
}

void ConfigSetHardVer(u16 ver)
{
    g_config.hardVer = ver;
}

u16 ConfigGetHardVer(void)
{
    return g_config.hardVer;
}

void ConfigSetSimpleInterval(u8 interval)
{
    g_config.simpleInterval = interval;
}

s8 ConfigGetSimpleInterval(void)
{
    return g_config.simpleInterval;
}

void ConfigSetReportInterval(u8 interval)
{
    g_config.reportInterval = interval;
}

s8 ConfigGetReportInterval(void)
{
    return g_config.reportInterval;
}

void ConfigSetpm25K(s16 val)
{
    g_config.pm25K = val;
}

u16 ConfigGetpm25K(void)
{
    return g_config.pm25K;
}

void ConfigSetpm25B(s16 val)
{
    g_config.pm25B = val;
}

u16 ConfigGetpm25B(void)
{
    return g_config.pm25B;
}

void ConfigSetpm10K(s16 val)
{
    g_config.pm10K = val;
}

u16 ConfigGetpm10K(void)
{
    return g_config.pm10K;
}

void ConfigSetpm10B(s16 val)
{
    g_config.pm10B = val;
}

u16 ConfigGetpm10B(void)
{
    return g_config.pm10B;
}

void ConfigSetpm10BaseV(s16 val)
{
    g_config.pm10BaseV = val;
}

u16 ConfigGetpm10BaseV(void)
{
    return g_config.pm10BaseV;
}

void ConfigSetpm10BaseC(s16 val)
{
    g_config.pm10BaseC = val;
}

u16 ConfigGetpm10BaseC(void)
{
    return g_config.pm10BaseC;
}

void ConfigSetpm10N(s16 val)
{
    g_config.pm10N = val;
}

u16 ConfigGetpm10N(void)
{
    return g_config.pm10N;
}

void ConfigSetcoVw(s16 val)
{
    g_config.coVw = val;
}

u16 ConfigGetcoVw(void)
{
    return g_config.coVw;
}

void ConfigSetcoVa(s16 val)
{
    g_config.coVa = val;
}

u16 ConfigGetcoVa(void)
{
    return g_config.coVa;
}

void ConfigSetcoS(s16 val)
{
    g_config.coS = val;
}

void ConfigSetso2S(s16 val)
{
    g_config.so2S = val;
}

void ConfigSeto3S(s16 val)
{
    g_config.o3S = val;
}

void ConfigSetno2S(s16 val)
{
    g_config.no2S = val;
}

void ConfigSetso2Va(s16 val)
{
    g_config.so2Va = val;
}

u16 ConfigGetso2Va(void)
{
    return g_config.so2Va;
}

void ConfigSetso2Vw(s16 val)
{
    g_config.so2Vw = val;
}

u16 ConfigGetso2Vw(void)
{
    return g_config.so2Vw;
}

void ConfigSeto3Vw(s16 val)
{
    g_config.o3Vw = val;
}

u16 ConfigGeto3Vw(void)
{
    return g_config.o3Vw;
}

void ConfigSeto3Va(s16 val)
{
    g_config.o3Va = val;
}

u16 ConfigGeto3Va(void)
{
    return g_config.o3Va;
}

void ConfigSetno2Vw(s16 val)
{
    g_config.no2Vw = val;
}

u16 ConfigGetno2Vw(void)
{
    return g_config.no2Vw;
}

void ConfigSetno2Va(s16 val)
{
    g_config.no2Va = val;
}

u16 ConfigGetno2Va(void)
{
    return g_config.no2Va;
}

void GetCoZero(s16 *Vw, s16 *Va, s16 *S)
{
    *Vw = g_config.coVw;
    *Va = g_config.coVa;
    *S = g_config.coS;
}

void GetSo2Zero(s16 *Vw, s16 *Va, s16 *S)
{
    *Vw = g_config.so2Vw;
    *Va = g_config.so2Va;
    *S = g_config.so2S;
}

void GetO3Zero(s16 *Vw, s16 *Va, s16 *S)
{
    *Vw = g_config.o3Vw;
    *Va = g_config.o3Va;
    *S = g_config.o3S;
}

void GetNo2Zero(s16 *Vw, s16 *Va, s16 *S)
{
    *Vw = g_config.no2Vw;
    *Va = g_config.no2Va;
    *S = g_config.no2S;
}


/* 配置改变时，在写文件的线程操作文件，因为打开文件需要比较大的栈 */
/* END */





