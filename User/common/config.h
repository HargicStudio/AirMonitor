#ifndef _CONFIG_H_
#define _CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif
  
#include "AaInclude.h"
  
#define MAX_IP_LEN              20
#define MAX_ADDR_LEN            5
  
typedef struct CONFIG_t
{
    u32 myAddr;
    u8 strAddr[MAX_ADDR_LEN];
    u8 serverIp[MAX_IP_LEN];
    u16 serverPort;
    u16 softVer;
    u16 hardVer;
    u8 simpleInterval;     
    u8 reportInterval;
    u16 pm25K;    /* 以下是校准参数, 用于最终计算的类型可能变化 */
    u16 pm25B;
    u16 pm10K;
    u16 pm10B;
    u16 coVw;
    u16 coVa;
    u16 so2Vw;
    u16 so2Va;
    u16 o3Vw;
    u16 o3Va;
    u16 no2Vw;
    u16 no2Va;

}CONFIG_t;

void ConfigInit(void);

void ConfigSetAddr(u32 addr);
u32 ConfigGetAddr(void);

void ConfigSetStrAddr(u8 *buf);
u8 *ConfigGetStrAddr(void);

void ConfigSetServerIpInt(s32 ip);
void ConfigSetServerIp(u8 *buf, u8 len);
u8 *ConfigGetServerIp(void);

void ConfigSetServerPort(u16 port);
u16 ConfigGetServerPort(void);

void ConfigSetSoftVer(u16 ver);
u16 ConfigGetSoftVer(void);

void ConfigSetHardVer(u16 ver);
u16 ConfigGetHardVer(void);

void ConfigSetSimpleInterval(u8 interval);
s8 ConfigGetSimpleInterval(void);

void ConfigSetReportInterval(u8 interval);
s8 ConfigGetReportInterval(void);

u16 ConfigGetno2Va(void);
void ConfigSetno2Va(u16 val);

u16 ConfigGetno2Vw(void);
void ConfigSetno2Vw(u16 val);

u16 ConfigGeto3Va(void);
void ConfigSeto3Va(u16 val);

u16 ConfigGeto3Vw(void);
void ConfigSeto3Vw(u16 val);

u16 ConfigGetso2Va(void);
void ConfigSetso2Va(u16 val);

u16 ConfigGetso2Vw(void);
void ConfigSetso2Vw(u16 val);

u16 ConfigGetcoVa(void);
void ConfigSetcoVa(u16 val);

u16 ConfigGetcoVw(void);
void ConfigSetcoVw(u16 val);

u16 ConfigGetpm10B(void);
void ConfigSetpm10B(u16 val);

u16 ConfigGetpm10K(void);
void ConfigSetpm10K(u16 val);

u16 ConfigGetpm25B(void);
void ConfigSetpm25B(u16 val);

u16 ConfigGetpm25K(void);
void ConfigSetpm25K(u16 val);
  
  
#ifdef __cplusplus
}
#endif
  

#endif 