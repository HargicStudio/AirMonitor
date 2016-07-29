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
    u8 simpleInterval;     /* ÿ�����������в�ͬ�ļ�� */
    u8 reportInterval;     /* �ϱ�ʱ���� */

}CONFIG_t;

void ConfigInit(void);

void ConfigSetAddr(u32 addr);
u32 ConfigGetAddr(void);

void ConfigSetStrAddr(u8 *buf);
u8 *ConfigGetStrAddr(void);

void ConfigSetServerIp(u8 *buf, u8 len);
u8 *ConfigGetServerIp(void);

void ConfigSetServerPort(u16 port);
u16 ConfigGetServerPort(void);

void ConfigSetSoftVer(u16 ver);
u16 ConfigGetSoftVer(void);

void ConfigSetSimpleInterval(s8 interval);
s8 ConfigGetSimpleInterval(void);

void ConfigSetReportInterval(s8 interval);
s8 ConfigGetReportInterval(void);
  
  
#ifdef __cplusplus
}
#endif
  

#endif 