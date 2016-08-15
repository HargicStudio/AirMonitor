#ifndef _DATA_RECORD_H_
#define _DATA_RECORD_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32f4xx_hal.h"
#include "AaInclude.h"
#include "format.h"
   

/* �ص����͵�bufffer �� 1024. ����������ȷ��� */
   
/* �ص���Ϣ */
typedef struct RECALL_INFO_t
{
    u8 flag;                    /* �Ƿ���Ҫ�ص��ı�־ */
    u8 continueFlag;            /* ������ص���Ҫ���������� */
    u16 type;                   /* �ص����� */
    u8 startTime[11];           /* 1608100828 */
    u8 Folder[9];               /* 1608/10/ */
    u8 file[7];                 /* Сʱ�� 00- 23.txt*/
    
}RECALL_INFO_t;

/* д�ļ���Ϣ */
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

/* �ص�Ӧ������ݻ��� */
extern SEND_BIG_BUF_t g_sendRecallData;

/* �ص���Ϣ */
extern RECALL_INFO_t g_recallInfo;
   
#ifdef __cplusplus
}
#endif
   
#endif