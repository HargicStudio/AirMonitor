#ifndef _DATA_RECORD_H_
#define _DATA_RECORD_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32f4xx_hal.h"
#include "AaInclude.h"
#include "format.h"
   
/* ���ص�6�������� */
#define MAX_RECALL_MONTH        6
   
/* �ص����͵�bufffer �� 1024. ����������ȷ��� */
   
/* �ص���Ϣ */
typedef struct RECALL_INFO_t
{
    u8 flag;                    /* �Ƿ���Ҫ�ص��ı�־ */
    u8 continueFlag;            /* ������ص���Ҫ���������� */
    u8 stopflag;                /* �յ�������ֹͣ�ص����� */
    u8 sendStartFlag;           /* 0: δ����״̬ 1: �ѷ�����ʼ��Ϣ */
    u16 type;                   /* �ص����� */
    u8 startTime[15];           /* 201608100828 */
    u8 endTime[15];             /* 201608100829 */
    u16 startyear;
    u8 startmon;
    u8 startday;
    u8 starthour;
    u16 endyear;
    u8 endmon;
    u8 endday;
    u8 endhour;
    u16 nowyear;
    u8 nowmon;
    u8 nowday;
    u8 nowhour;
    
}RECALL_INFO_t;

/* д�ļ���Ϣ */
typedef struct RECORD_INFO_t
{
    u8 newNameYear[5];  /* 2016 */
    u8 newNameMon[8];   /* 2016//08 */
    u8 newNameDay[11];   /* 2016//08//01 */
    u8 newNameHour[18];  /* 2016//08//01//10.txt */
  
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

bool UpdateNowHourAndCheckFinish(void);

void RecallExcuter(void);

/* �ص�Ӧ������ݻ��� */
extern SEND_BIG_BUF_t g_sendRecallData;

/* �ص���Ϣ */
extern RECALL_INFO_t g_recallInfo;
   
#ifdef __cplusplus
}
#endif
   
#endif