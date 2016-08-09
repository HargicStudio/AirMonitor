#include "dataRecord.h"
#include "osa_file.h"
#include "moduleStatus.h"

osThreadId _data_record_id;

/* Config file */
u8 _file_buf[60];

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
    OSA_FileHandle hFile;
    s32 ret = 0;
    
    
    if (-1 == Initcfg(NULL))
    {
        printf("Initcfg ERROR!\r\n");
        return false;
    }
    
    ConfigInit();
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
        /* ÿ10s�ȶ�һ��ʱ�䣬�ж��Ƿ��¼ */
        osDelay(10000);
        
    }
}