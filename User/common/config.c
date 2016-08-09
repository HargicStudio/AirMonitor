#include "config.h"
#include <string.h>
#include "gpsAnalyser.h"
#include "common.h"
#include "cfg.h"


/* 用于计算和保存信息 */
extern gps_process_data gps;

CONFIG_t g_config;

u8 _cfgBuf[20];

/*
*  配置项
*
*/
void ConfigInit(void)
{
    s32 temp32 = 0;
    
    if (-1 != ReadCfg(C_ADDR, _cfgBuf))
    {
        ConfigSetStrAddr(_cfgBuf);
    }
    
    if (-1 != ReadCfgInt(C_ADDR, &temp32))
    {
        ConfigSetAddr(temp32);
    }
    
    if (-1 != ReadCfgInt(C_SOFTVER, &temp32))
    {
        ConfigSetSoftVer(temp32);
    }
    
    if (-1 != ReadCfgInt(C_HARDVER, &temp32))
    {
        ConfigSetHardVer(temp32);
    }

    if (-1 != ReadCfg(C_SERVERIP, _cfgBuf))
    {
        ConfigSetServerIp(_cfgBuf, strlen(_cfgBuf));
    }

    if (-1 != ReadCfgInt(C_SERVERPORT, &temp32))
    {
        ConfigSetServerPort(temp32);
    }
    
    if (-1 != ReadCfgInt(C_SIMPLEINT, &temp32))
    {
        ConfigSetSimpleInterval(temp32);
    }
    
    if (-1 != ReadCfgInt(C_REPORTINT, &temp32))
    {
        ConfigSetReportInterval(temp32);
    }
    
    if (-1 != ReadCfgInt(C_K25, &temp32))
    {
        ConfigSetpm25K(temp32);
    }
    
    if (-1 != ReadCfgInt(C_B25, &temp32))
    {
        ConfigSetpm25B(temp32);
    }
    
    if (-1 != ReadCfgInt(C_K10, &temp32))
    {
        ConfigSetpm10K(temp32);
    }
    
    if (-1 != ReadCfgInt(C_B10, &temp32))
    {
        ConfigSetpm10B(temp32);
    }
    
    if (-1 != ReadCfgInt(C_COVw, &temp32))
    {
        ConfigSetcoVw(temp32);
    }
    
    if (-1 != ReadCfgInt(C_COVa, &temp32))
    {
        ConfigSetcoVa(temp32);
    }
    
    if (-1 != ReadCfgInt(C_SO2Vw, &temp32))
    {
        ConfigSetso2Vw(temp32);
    }
    
    if (-1 != ReadCfgInt(C_SO2Va, &temp32))
    {
        ConfigSetso2Va(temp32);
    }
    
    if (-1 != ReadCfgInt(C_O3Vw, &temp32))
    {
        ConfigSeto3Vw(temp32);
    }
    
    if (-1 != ReadCfgInt(C_O3Va, &temp32))
    {
        ConfigSeto3Va(temp32);
    }
    
    if (-1 != ReadCfgInt(C_NO2Vw, &temp32))
    {
        ConfigSetno2Vw(temp32);
    }
    
    if (-1 != ReadCfgInt(C_NO2Va, &temp32))
    {
        ConfigSetno2Va(temp32);
    }
    
    /* 默认的时间 */
    ConfigSetTime();
    
    ConfigPrint();
    
}

void ConfigPrint(void)
{
    printf("Config Addr: %d, SADDR: %s\r\n", g_config.myAddr, g_config.strAddr);
    printf("SOFT VER: %d, HARD VER: %d\r\n", g_config.softVer, g_config.hardVer);
    printf("Cloud: %s, PORT: %d\r\n", g_config.serverIp, g_config.serverPort);
    printf("SIMPLE: %d, REPORT: %d\r\n", g_config.simpleInterval, g_config.reportInterval);
    printf("2.5 K,B : %d,%d; 10K,B : %d,%d\r\n", g_config.pm25K, g_config.pm25B,
               g_config.pm10K, g_config.pm10B);
    printf("CO W,A : %d,%d; SO2 W,A : %d,%d\r\n", g_config.coVw, g_config.coVa,
               g_config.so2Vw, g_config.so2Va);
    printf("O3 W,A : %d,%d; NO2 W,A : %d,%d\r\n", g_config.o3Vw, g_config.o3Va,
               g_config.no2Vw, g_config.no2Va);
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

void ConfigSetpm25K(u16 val)
{
    g_config.pm25K = val;
}

u16 ConfigGetpm25K(void)
{
    return g_config.pm25K;
}

void ConfigSetpm25B(u16 val)
{
    g_config.pm25B = val;
}

u16 ConfigGetpm25B(void)
{
    return g_config.pm25B;
}

void ConfigSetpm10K(u16 val)
{
    g_config.pm10K = val;
}

u16 ConfigGetpm10K(void)
{
    return g_config.pm10K;
}

void ConfigSetpm10B(u16 val)
{
    g_config.pm10B = val;
}

u16 ConfigGetpm10B(void)
{
    return g_config.pm10B;
}

void ConfigSetcoVw(u16 val)
{
    g_config.coVw = val;
}

u16 ConfigGetcoVw(void)
{
    return g_config.coVw;
}

void ConfigSetcoVa(u16 val)
{
    g_config.coVa = val;
}

u16 ConfigGetcoVa(void)
{
    return g_config.coVa;
}

void ConfigSetso2Va(u16 val)
{
    g_config.so2Va = val;
}

u16 ConfigGetso2Va(void)
{
    return g_config.so2Va;
}

void ConfigSetso2Vw(u16 val)
{
    g_config.so2Vw = val;
}

u16 ConfigGetso2Vw(void)
{
    return g_config.so2Vw;
}

void ConfigSeto3Vw(u16 val)
{
    g_config.o3Vw = val;
}

u16 ConfigGeto3Vw(void)
{
    return g_config.o3Vw;
}

void ConfigSeto3Va(u16 val)
{
    g_config.o3Va = val;
}

u16 ConfigGeto3Va(void)
{
    return g_config.o3Va;
}

void ConfigSetno2Vw(u16 val)
{
    g_config.no2Vw = val;
}

u16 ConfigGetno2Vw(void)
{
    return g_config.no2Vw;
}

void ConfigSetno2Va(u16 val)
{
    g_config.no2Va = val;
}

u16 ConfigGetno2Va(void)
{
    return g_config.no2Va;
}






