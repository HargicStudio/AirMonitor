#include "config.h"
#include <string.h>

/*
typedef struct CONFIG_t
{
    u32 myAddr;
    u8 strAddr[5];
    u8 serverIp[MAX_IP_LEN];
    u16 serverPort;
    u16 version;
    u8 simpleInterval;  
    u8 reportInterval;  

}CONFIG_t;
*/


CONFIG_t g_config;

void ConfigInit(void)
{
    ConfigSetAddr(60000);
    ConfigSetStrAddr("60000");
    ConfigSetServerIp("39.191.114.217", 15);
    ConfigSetServerPort(8090);
    ConfigSetSoftVer(0);
    ConfigSetSimpleInterval(5);
    ConfigSetReportInterval(5);
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






