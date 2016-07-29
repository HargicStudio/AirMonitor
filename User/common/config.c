#include "config.h"

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

void ConfigSetSimpleInterval(s8 interval)
{
    g_config.simpleInterval = interval;
}

s8 ConfigGetSimpleInterval(void)
{
    return g_config.simpleInterval;
}

void ConfigSetReportInterval(s8 interval)
{
    g_config.reportInterval = interval;
}

s8 ConfigGetReportInterval(void)
{
    return g_config.reportInterval;
}




