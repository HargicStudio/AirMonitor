#ifndef _GSM_CTRL_H_
#define _GSM_CTRL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"


/* GSM Control */
#define MAX_WAIT_TIMES          20
  
/* 关闭GSM计数，以50ms为单位, 现在是10s */
#define MAX_WAIT_CLOSE_GSM      200
  
typedef enum GSM_STATUS
{
    GSM_CLOSED,
    GSM_FREE,
    GSM_TCP_CONNECTED,

}GSM_STATUS;

enum GSM_POWER
{
    GSM_POWER_UP,
    GSM_POWER_DOWN,
};

typedef enum AT_STATUS_e
{
    AT_INVALID,
    AT_WAIT_RSP,                        /* 等待OK */
    AT_WAIT_CONNECT_RSP,
    AT_WAIT_CONNECT_STU,
    AT_WAIT_SEND_OK,
    AT_WAIT_REG,
    AT_WAIT_TO_SEND,
    AT_ERROR,
    AT_SUSS,
    
}AT_STATUS_e;

/* GSM Control End */

bool IsGsmRunning(void);
void GsmPowerUpDownOpt(u8 type);
bool GsmStartup(void);
bool GsmStartAndconect(void);
bool SendData(u8 *str, int len);

bool GsmWaitForReg(void);

void AtValueInit();
void SetAtStatus(s32 stu);
s32 GetAtStatus(void);

u32 GsmStatusGet(void);
void GsmStatusSet(u32 stu);

bool AtCmdRun(u8 *cmd, u16 len, u16 waitTimes, u32 rspType);
void IsAtSuss(u8 *buf, u8 *key);

/* 控制关闭GSM模块 */
void GsmWaitCloseCountReset(void);
void GsmWaitCloseCountAdd(void);
bool IsGsmWaitCloseCountReach(void);

#ifdef __cplusplus
}
#endif

#endif



