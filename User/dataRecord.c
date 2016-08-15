#include "dataRecord.h"
#include "osa_file.h"
#include "moduleStatus.h"
#include "common.h"
#include "gps.h"

osThreadId _data_record_id;

/*　38 的数据加 \r\n　*/ 
#define RECORD_DATA_LEN                 40

/* 保存到文件中的数据 */
u8 _record_buf[60];

/* 记录文件的信息 */
RECORD_INFO_t g_recordInfo;

/* 回调信息 */
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
*  配置项格式
*  80001VVHHFFFFFFFFPPAABBKKBBKKBBWWVVWWVVWWVVWWVV
*  FFFFFFFF: IP 
*  VV : 软件版本号
*  HH : 硬件版本号
*  PP : PORT
*  AA : 采集间隔
*  BB : 上报间隔
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
    
    /* 文件系统初始化 */
    OSA_fileInit();
    
    /* 获取SD卡容量信息 */
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
        /* 每1s比对一下时间，判断是否记录 */
        /* 同步或者收到校时以后才开始记录数据，数据记录以天为文件夹，以小时为文件 */
        /* 文件夹命名规则：一级目录 年月：1608 */
        /*                二级目录 日：10 */
        /* 文件命名规则： 0-23 .txt */
        /* 文件内容格式： 除头部上报数据完整记录 */
        /* 头部的地址信息可能更改 */
        osDelay(20000);
        
        RecallHandler();
        
        RecordData();
        
        UpdateConfig();
        
    }
}

/////////////////////////////// 记录到文件操作接口//////////////////////////////
void RecordData(void)
{
    u8 offset = 0;
    u8 path[] = "1608\\10\\00.txt";
    FIL fp;
    s32 ret;
    u32 writtenByte = 0;
    
    /* 时间不同步，无需记录 */
    /*  测试先屏蔽掉
    if (!IsClockSynced())
    {
        printf("*** Didn't synced! ***\r\n");
        return;
    }*/
    
    /* 判断是否到了记录的时间,RTC接口有了再做判断 */
    memset(_record_buf, 0, sizeof(_record_buf));
    offset = ConstructRecordData(_record_buf);
    
    /* 获得到时间，得到文件目录信息 */
    /* 获得RTC时间之前模拟 */
    memcpy(g_recordInfo.newNameMon, "1603", 4);
    memcpy(g_recordInfo.newNameDay, "1603\\01", 7);
    memcpy(g_recordInfo.newNameHour, "1603\\01\\23.txt", 14);
    
    
    /* 根据当前的时间写到相应的文件中 */
    // to do 获得时间
    /* 判断目录是否存在 */
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
            /* 设置TF卡状态 */
            SetModuleStu(MDU_CARD, STU_ERROR);
            printf("Create New file failed ! %s\r\n", g_recordInfo.newNameHour);
            return;
        }
    }
    
    f_lseek(&fp, f_size(&fp));
    
    ret = f_write(&fp, _record_buf, offset, &writtenByte);
    if (OSA_EFAIL == ret || writtenByte < offset)
    {
        /* 设置TF卡状态 */
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
    /* 没有回调 */
    if (0 == IsInRecall())
    {
        return;
    }
    
    /* 回调的数据还未发送，未构造的数据等待下一个轮询 */
    if (1 == g_sendRecallData.sendFlag)
    {
        return;
    }
    
    InitSendRecallData();
    
    GSM_LOG_P1("Recall data: type %d", g_recallInfo.type);
    
    /* 分钟回调和小时回调都用小时回调，因为数据量都不大 */
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
    /* 打开目标文件 */
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
            /* 缓存用完，可以发送了 */
            goto READY_TO_SEND;
        }
        
        seek += readByte;
        f_lseek(&fp, seek);
    }
    
READY_TO_SEND:
    g_sendRecallData.sendFlag = 1;
    g_sendRecallData.respFlag = 0;
    /* 小时数据，一次发送，所以不需要继续，发送数据时，判断这个是否继续标志决定是否清除回调标志 */
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
    /* 打开目标文件 */
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
            /* 缓存用完，可以发送了 */
            goto READY_TO_SEND_HOUR;
        }
        
        seek += readByte;
        f_lseek(&fp, seek);
    }
    
READY_TO_SEND_HOUR:
    g_sendRecallData.sendFlag = 1;
    g_sendRecallData.respFlag = 0;
    /* 小时数据，一次发送，所以不需要继续，发送数据时，判断这个是否继续标志决定是否清除回调标志 */
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
    /* 打开目标文件 */
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
            /* 缓存用完，可以发送了 */
            goto READY_TO_SENDDAY;
        }
        
        seek += readBytes;
        f_lseek(&fp, seek);
    }
    
READY_TO_SENDDAY:
    g_sendRecallData.sendFlag = 1;
    g_sendRecallData.respFlag = 0;
    /* 天数据，需要继续，发送数据时，判断这个是否继续标志决定是否清除回调标志 */
    
    
    /* 更新下一个要读的文件，g_recallInfo.file */
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
*  判断是否正在处理回调
*/
u8 IsInRecall()
{
    return g_recallInfo.flag;
}

/*
*  1. 接收到回调请求时置Flag
*  2. 发送完成时，清Flag
*  3. 构造数据出错时，清Flag
*/
void ClearRecallFlag()
{
    g_recallInfo.flag = 0;
}

void SetRecallFlag()
{
    g_recallInfo.flag = 1;
}

//////////// 更新配置 ////////////////////////////
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
//////////////////////////////记录到文件操作接口END/////////////////////////////
                       