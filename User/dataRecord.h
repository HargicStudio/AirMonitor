#ifndef _DATA_RECORD_H_
#define _DATA_RECORD_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32f4xx_hal.h"
#include "AaInclude.h"
#include "format.h"
   

/* 回调发送的bufffer 是 1024. 最长填充这个长度发送 */
   
/* 回调信息 */
typedef struct RECALL_INFO_t
{
    u8 flag;                    /* 是否需要回调的标志 */
    u8 continueFlag;            /* 对于天回调，要构造多次数据 */
    u16 type;                   /* 回调类型 */
    u8 startTime[11];           /* 1608100828 */
    u8 Folder[9];               /* 1608/10/ */
    u8 file[7];                 /* 小时： 00- 23.txt*/
    
}RECALL_INFO_t;

/* 写文件信息 */
typedef struct RECORD_INFO_t
{
    u8 newNameMon[5];   /* 1608 */
    u8 newNameDay[8];   /* 1608//01 */
    u8 newNameHour[15];  /* 1608//01//10.txt */
  
}RECORD_INFO_t;
   
s32 StartDataRecordTask(void);
   
bool ReadConfigFile(void);

void RecordData(void);

void RecallHandler(void);

u8 IsInRecall();

void ClearRecallFlag();

void SetRecallFlag();

void RecallMinData(void);

void RecallHourData(void);

void RecallDayData(void);

void UpdateConfig(void);

/* 回调应答的数据缓冲 */
extern SEND_BIG_BUF_t g_sendRecallData;

/* 回调信息 */
extern RECALL_INFO_t g_recallInfo;
   
#ifdef __cplusplus
}
#endif
   
#endif