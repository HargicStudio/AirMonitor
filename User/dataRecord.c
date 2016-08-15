#include "dataRecord.h"
#include "osa_file.h"
#include "moduleStatus.h"
#include "common.h"
#include "gps.h"

osThreadId _data_record_id;

/*��38 �����ݼ� \r\n��*/ 
#define RECORD_DATA_LEN                 40

/* ���浽�ļ��е����� */
u8 _record_buf[60];

/* ��¼�ļ�����Ϣ */
RECORD_INFO_t g_recordInfo;

/* �ص���Ϣ */
RECALL_INFO_t g_recallInfo;

static void DataRecordThread(void const *argument);


s32 StartDataRecordTask(void)
{
    /*##-1- Start task #########################################################*/
    osThreadDef(dataRecord, DataRecordThread, osPriorityNormal, 0, 8 * configMINIMAL_STACK_SIZE);
    _data_record_id = AaThreadCreateStartup(osThread(dataRecord), NULL);

    return 0;
}

/*
*  �������ʽ
*  80001VVHHFFFFFFFFPPAABBKKBBKKBBWWVVWWVVWWVVWWVV
*  FFFFFFFF: IP 
*  VV : ����汾��
*  HH : Ӳ���汾��
*  PP : PORT
*  AA : �ɼ����
*  BB : �ϱ����
*/

#define CONFIG_FILE_LEN      47

bool ReadConfigFile(void)
{
    if (-1 == Initcfg(NULL))
    {
        printf("Initcfg ERROR!\r\n");
        return false;
    }
    
    ConfigInit();
    
    return true;
}


static void DataRecordThread(void const *argument)
{
    SD_sizeInfo sdSizeInfo;
    
    (void *)argument;
    
    /* �ļ�ϵͳ��ʼ�� */
    OSA_fileInit();
    
    /* ��ȡSD��������Ϣ */
    if (OSA_getSdSize(&sdSizeInfo) == OSA_OK)
    {
        AaSysLogPrintF(LOGLEVEL_INF, FeatureLog, "SD totalSize=%dM availableSize=%dM\n\r",
                       sdSizeInfo.totalSize, sdSizeInfo.availableSize);
    }
    else
    {
        printf("The SD card is abnormal!\r\n");
        SetModuleStu(MDU_CARD, STU_ERROR);
        return;
    }
    
    ReadConfigFile();
    
    for(;;)
    {
        /* ÿ1s�ȶ�һ��ʱ�䣬�ж��Ƿ��¼ */
        /* ͬ�������յ�Уʱ�Ժ�ſ�ʼ��¼���ݣ����ݼ�¼����Ϊ�ļ��У���СʱΪ�ļ� */
        /* �ļ�����������һ��Ŀ¼ ���£�1608 */
        /*                ����Ŀ¼ �գ�10 */
        /* �ļ��������� 0-23 .txt */
        /* �ļ����ݸ�ʽ�� ��ͷ���ϱ�����������¼ */
        /* ͷ���ĵ�ַ��Ϣ���ܸ��� */
        osDelay(20000);
        
        RecallHandler();
        
        RecordData();
        
        UpdateConfig();
        
    }
}

/////////////////////////////// ��¼���ļ������ӿ�//////////////////////////////
void RecordData(void)
{
    u8 offset = 0;
    u8 path[] = "1608\\10\\00.txt";
    FIL fp;
    s32 ret;
    u32 writtenByte = 0;
    
    /* ʱ�䲻ͬ���������¼ */
    /*  ���������ε�
    if (!IsClockSynced())
    {
        printf("*** Didn't synced! ***\r\n");
        return;
    }*/
    
    /* �ж��Ƿ��˼�¼��ʱ��,RTC�ӿ����������ж� */
    memset(_record_buf, 0, sizeof(_record_buf));
    offset = ConstructRecordData(_record_buf);
    
    /* ��õ�ʱ�䣬�õ��ļ�Ŀ¼��Ϣ */
    /* ���RTCʱ��֮ǰģ�� */
    memcpy(g_recordInfo.newNameMon, "1603", 4);
    memcpy(g_recordInfo.newNameDay, "1603\\01", 7);
    memcpy(g_recordInfo.newNameHour, "1603\\01\\23.txt", 14);
    
    
    /* ���ݵ�ǰ��ʱ��д����Ӧ���ļ��� */
    // to do ���ʱ��
    /* �ж�Ŀ¼�Ƿ���� */
    if (!IsDirExit(g_recordInfo.newNameMon))
    {
        printf("Create New Month dir: %s\r\n", g_recordInfo.newNameMon);
        f_mkdir(g_recordInfo.newNameMon);
    }
    
    if (!IsDirExit(g_recordInfo.newNameDay))
    {
        printf("Create New Day dir: %s\r\n", g_recordInfo.newNameDay);
        f_mkdir(g_recordInfo.newNameDay);
    }
    
    if (FR_OK != f_open(&fp, g_recordInfo.newNameHour, FA_WRITE))
    {
        printf("Create New hour file: %s\r\n", g_recordInfo.newNameHour);
        if (OSA_OK != f_open(&fp, g_recordInfo.newNameHour, FA_CREATE_NEW | FA_WRITE))
        {
            /* ����TF��״̬ */
            SetModuleStu(MDU_CARD, STU_ERROR);
            printf("Create New file failed ! %s\r\n", g_recordInfo.newNameHour);
            return;
        }
    }
    
    f_lseek(&fp, f_size(&fp));
    
    ret = f_write(&fp, _record_buf, offset, &writtenByte);
    if (OSA_EFAIL == ret || writtenByte < offset)
    {
        /* ����TF��״̬ */
        SetModuleStu(MDU_CARD, STU_ERROR);
        printf("Write file failed ! %s\r\n", g_recordInfo.newNameHour);
        f_close(&fp);
        return;
    }
    
    printf("Write %d data to file: %s\r\n", writtenByte, g_recordInfo.newNameHour);
    f_sync(&fp);
    
    f_close(&fp);
}

void RecallHandler(void)
{
    /* û�лص� */
    if (0 == IsInRecall())
    {
        return;
    }
    
    /* �ص������ݻ�δ���ͣ�δ��������ݵȴ���һ����ѯ */
    if (1 == g_sendRecallData.sendFlag)
    {
        return;
    }
    
    InitSendRecallData();
    
    GSM_LOG_P1("Recall data: type %d", g_recallInfo.type);
    
    /* ���ӻص���Сʱ�ص�����Сʱ�ص�����Ϊ������������ */
    if (81 == g_recallInfo.type)
    {
        RecallMinData();
    }
    else if (83 == g_recallInfo.type)
    {
        RecallHourData();
    }
    else if (85 == g_recallInfo.type)
    {
        RecallDayData();
    }
}

void RecallMinData(void)
{
    FIL fp;
    u8 path[] = "1608\\11\\11.txt";
    u32 readByte = 0;
    u32 seek = 0;
    
    sprintf(path, "%s%s", g_recallInfo.Folder, g_recallInfo.file);
    /* ��Ŀ���ļ� */
    if (FR_OK != f_open(&fp, path, FA_READ))
    {
        GSM_LOG_P1("Open file fail: %s", g_recordInfo.newNameHour);
        ClearRecallFlag();
        return;
    }
    
    f_lseek(&fp, 0);
      
    while (FR_OK == f_read(&fp, _record_buf, RECORD_DATA_LEN, &readByte))
    {
        if (readByte < RECORD_DATA_LEN)
        {
            break;
        }
        
        if (0 == ConstructRecordDataToSend(_record_buf, "082"))
        {
            /* �������꣬���Է����� */
            goto READY_TO_SEND;
        }
        
        seek += readByte;
        f_lseek(&fp, seek);
    }
    
READY_TO_SEND:
    g_sendRecallData.sendFlag = 1;
    g_sendRecallData.respFlag = 0;
    /* Сʱ���ݣ�һ�η��ͣ����Բ���Ҫ��������������ʱ���ж�����Ƿ������־�����Ƿ�����ص���־ */
    g_recallInfo.continueFlag = 0;
    
    f_close(&fp);
    
}

void RecallHourData(void)
{
    FIL fp;
    u8 path[] = "1608\\11\\11.txt";
    u32 readByte = 0;
    u32 seek = 0;
    
    sprintf(path, "%s%s", path, g_recallInfo.file);
    /* ��Ŀ���ļ� */
    if (FR_OK != f_open(&fp, g_recordInfo.newNameHour, FA_READ))
    { 
        GSM_LOG_P1("Open file fail: %s", g_recordInfo.newNameHour);
        ClearRecallFlag();
        return;
    }
    
    f_lseek(&fp, 0);
      
    while (FR_OK == f_read(&fp, _record_buf, RECORD_DATA_LEN, &readByte))
    {
        if (readByte < RECORD_DATA_LEN)
        {
            break;
        }
        
        if (0 == ConstructRecordDataToSend(_record_buf, "082"))
        {
            /* �������꣬���Է����� */
            goto READY_TO_SEND_HOUR;
        }
        
        seek += readByte;
        f_lseek(&fp, seek);
    }
    
READY_TO_SEND_HOUR:
    g_sendRecallData.sendFlag = 1;
    g_sendRecallData.respFlag = 0;
    /* Сʱ���ݣ�һ�η��ͣ����Բ���Ҫ��������������ʱ���ж�����Ƿ������־�����Ƿ�����ص���־ */
    g_recallInfo.continueFlag = 0;
    
    f_close(&fp);
    
}

void RecallDayData(void)
{
    FIL fp;
    u8 path[] = "1608\\11\\11.txt";
    u32 readBytes = 0;
    u32 seek = 0;
    
    sprintf(path, "%s%s", g_recallInfo.Folder, g_recallInfo.file);
    /* ��Ŀ���ļ� */
    if (FR_OK != f_open(&fp, path, FA_READ))
    {
        GSM_LOG_P1("Open file fail: %s", g_recordInfo.newNameHour);
        ClearRecallFlag();
        return;
    }
    
    f_lseek(&fp, 0);
      
    while (FR_OK == f_read(&fp, _record_buf, RECORD_DATA_LEN, &readBytes))
    {
        if (readBytes < RECORD_DATA_LEN)
        {
            break;
        }
        
        if (0 == ConstructRecordDataToSend(_record_buf, "082"))
        {
            /* �������꣬���Է����� */
            goto READY_TO_SENDDAY;
        }
        
        seek += readBytes;
        f_lseek(&fp, seek);
    }
    
READY_TO_SENDDAY:
    g_sendRecallData.sendFlag = 1;
    g_sendRecallData.respFlag = 0;
    /* �����ݣ���Ҫ��������������ʱ���ж�����Ƿ������־�����Ƿ�����ص���־ */
    
    
    /* ������һ��Ҫ�����ļ���g_recallInfo.file */
    readBytes = (g_recallInfo.file[0] - '0') * 10 + g_recallInfo.file[1] - '0';
    if (readBytes >= 23)
    {
        readBytes = 23;
        g_recallInfo.continueFlag = 0;
    }
    else
    {
        readBytes++;
        g_recallInfo.continueFlag = 1;
        g_recallInfo.file[0] = (readBytes%100)/10 + '0';
        g_recallInfo.file[1] = readBytes%10 + '0';
    }
    
    f_close(&fp);
    
    return;
    
}

/*
*  �ж��Ƿ����ڴ���ص�
*/
u8 IsInRecall()
{
    return g_recallInfo.flag;
}

/*
*  1. ���յ��ص�����ʱ��Flag
*  2. �������ʱ����Flag
*  3. �������ݳ���ʱ����Flag
*/
void ClearRecallFlag()
{
    g_recallInfo.flag = 0;
}

void SetRecallFlag()
{
    g_recallInfo.flag = 1;
}

//////////// �������� ////////////////////////////
void UpdateConfig(void)
{
    if (!IsConfigUpdated())
    {
        return;
    }
    
    ConfigUpdate();
    
    ConfigSetUpdate(0);
}
//////////////////////END/////////////////////////
//////////////////////////////��¼���ļ������ӿ�END/////////////////////////////
                       