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
    u8 updateFlag;
    u8 strAddr[MAX_ADDR_LEN+1];
    u8 serverIp[MAX_IP_LEN+1];
    u32 myAddr;
    u16 serverPort;
    u16 softVer;
    u16 hardVer;
    u8 simpleInterval;     
    u8 reportInterval;
    s16 pm25K;    /* 以下是校准参数, 用于最终计算的类型可能变化 */
    s16 pm25B;
    s16 pm10K;
    s16 pm10B;
    s16 coK;    /* 以下是校准参数, 用于最终计算的类型可能变化 */
    s16 coB;
    s16 so2K;
    s16 so2B;
    s16 o3K;    /* 以下是校准参数, 用于最终计算的类型可能变化 */
    s16 o3B;
    s16 no2K;
    s16 no2B;
    s16 tmpK;   /* 外部温度校准 */
    s16 tmpB;   
    s16 pm10BaseV;
    s16 pm10BaseC;
    s16 pm10N;
    s16 coVw;
    s16 coVa;
    s16 so2Vw;
    s16 so2Va;
    s16 o3Vw;
    s16 o3Va;
    s16 no2Vw;
    s16 no2Va;
    /* 零点 及 灵敏度 */
    s16 coS;
    s16 so2S;
    s16 o3S;
    s16 no2S;

}CONFIG_t;

/* 读取配置是否改变 */
u8 IsConfigChanged();

u8 IsConfigUpdated();

void ConfigUpdate(void);

void ConfigSetUpdate(u8 flag);

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
u16 ConfigGetSimpleInterval(void);

void ConfigSetReportInterval(u8 interval);
u16 ConfigGetReportInterval(void);

u16 ConfigGetno2Va(void);
void ConfigSetno2Va(s16 val);

u16 ConfigGetno2Vw(void);
void ConfigSetno2Vw(s16 val);

u16 ConfigGeto3Va(void);
void ConfigSeto3Va(s16 val);

u16 ConfigGeto3Vw(void);
void ConfigSeto3Vw(s16 val);

u16 ConfigGetso2Va(void);
void ConfigSetso2Va(s16 val);

u16 ConfigGetso2Vw(void);
void ConfigSetso2Vw(s16 val);

u16 ConfigGetcoVa(void);
void ConfigSetcoVa(s16 val);

u16 ConfigGetcoVw(void);
void ConfigSetcoVw(s16 val);

s16 ConfigGetpm10B(void);
void ConfigSetpm10B(s16 val);

s16 ConfigGetpm10K(void);
void ConfigSetpm10K(s16 val);

s16 ConfigGetpm25B(void);
void ConfigSetpm25B(s16 val);

s16 ConfigGetpm25K(void);
void ConfigSetpm25K(s16 val);
    
void ConfigSetpm10BaseV(s16 val);
u16 ConfigGetpm10BaseV(void);
void ConfigSetpm10BaseC(s16 val);
u16 ConfigGetpm10BaseC(void);

void ConfigSetpm10N(s16 val);
u16 ConfigGetpm10N(void);

void ConfigSetno2S(s16 val);
void ConfigSeto3S(s16 val);
void ConfigSetso2S(s16 val);
void ConfigSetcoS(s16 val);

s16 ConfigGetno2S(void);
s16 ConfigGeto3S(void);
s16 ConfigGetso2S(void);
s16 ConfigGetcoS(void);

void Getno2Zero(s16 *Vw, s16 *Va, s16 *S);
void Geto3Zero(s16 *Vw, s16 *Va, s16 *S);
void GetSo2Zero(s16 *Vw, s16 *Va, s16 *S);
void GetCoZero(s16 *Vw, s16 *Va, s16 *S);

void ConfigSetTime();
u8 ConfigSetRTCTime(u16 y, u8 m, u8 d, u8 h, u8 min, u8 s);

void ConfigPrint();

s16 ConfigGetTmpK(void);
void ConfigSetTmpK(s16 val);

s16 ConfigGetTmpB(void);
void ConfigSetTmpB(s16 val);

s16 ConfigGetNo2K(void);
void ConfigSetNo2K(s16 val);

s16 ConfigGetNo2B(void);
void ConfigSetNo2B(s16 val);

s16 ConfigGetO3K(void);
void ConfigSetO3K(s16 val);

s16 ConfigGetO3B(void);
void ConfigSetO3B(s16 val);

s16 ConfigGetSo2K(void);
void ConfigSetSo2K(s16 val);

s16 ConfigGetSo2B(void);
void ConfigSetSo2B(s16 val);

s16 ConfigGetCoK(void);
void ConfigSetCoK(s16 val);

s16 ConfigGetCoB(void);
void ConfigSetCoB(s16 val);

  
#ifdef __cplusplus
}
#endif
  

#endif 