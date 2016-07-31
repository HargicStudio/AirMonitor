
/* Includes ------------------------------------------------------------------*/
#include "gpsAnalyser.h"
#include "gps.h"
#include "dataHandler.h"
#include "gps_dev.h"
#include "RingBufferUtils.h"
#include <string.h>


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/** wait for send complete by serial with timeout 1 sec */  
#define GPS_SENDCMPL_TIMEOUT        (1000 * 1)

/* Private variables ---------------------------------------------------------*/

/** Description of the macro */  
osThreadId _gps_id;
osThreadId _gps_send_test_id;

/** signal for IT that data send complete */  
static osSemaphoreDef(gps_sendcplt_sem);
osSemaphoreId _gps_sendcplt_sem_id;

/** signal for IT that data receive complete */  
static osSemaphoreDef(gps_recvcplt_sem);
osSemaphoreId _gps_recvcplt_sem_id;

/** Description of the macro */  
GPS_BUF_t _gps_buf;

/* 控制GPS接收频率，会从配置读取。最好是能通过直接设置GPS模块控制 */
u8 _gps_freq = 10;

/** Description of the macro */  
static char recv_char;

/* 用于计算和保存信息 */
gps_process_data gps;



extern UART_HandleTypeDef UartHandle_gps;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static void GpsThread(void const *argument);
static void GpsDeviceInit();
static u8 GpsSendSerialData(u8* data, u32 len);
static void GpsBufInit(void);

/** 
 * This is a brief description. 
 * This is a detail description. 
 * @param[in]   inArgName input argument description. 
 * @param[out]  outArgName output argument description.  
 * @retval  
 * @retval  
 * @par 
 *      
 * @par 
 *      
 * @par History
 *      2016-6-26 Huang Shengda
 */  
static void GpsThread(void const *argument)
{
    (void) argument;

    AaSysLogPrintF(LOGLEVEL_INF, FeatureGps, "GpsThread started");

    for (;;)
    {
        osSemaphoreWait(_gps_recvcplt_sem_id, osWaitForever);

        // receive one data
        AaSysLogPrintF(LOGLEVEL_DBG, FeatureGps, "Receved GPS: %s %d : %s \n",
                    __FUNCTION__, __LINE__, _gps_buf.curReadBuf);

        GPS_Analysis(&gps, (u8*)_gps_buf.curReadBuf);

        StoreCoordInfo(gps.latitude, gps.longitude, &g_coord);
    }
}


static void GpsSendTestThread(void const *argument)
{
    (void) argument;

    AaSysLogPrintF(LOGLEVEL_INF, FeatureGps, "%s started", __FUNCTION__);

    for(;;)
    {
        osDelay(1000);
        GpsSendSerialData("**Hello, Gps**", strlen("**Hello, Gps**"));
    }
}

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
u8 StartGpsTask()
{
    GpsDeviceInit();

    // ring_buffer_init(&_gps_rx_ringbuf, _gps_rx_ringbuf_data, GPS_SERIAL_RX_RINGBUFFER_SIZE);
    // AaSysLogPrintF(LOGLEVEL_INF, FeatureGps, "create gps serial rx ringbuffer success");


    _gps_sendcplt_sem_id = osSemaphoreCreate(osSemaphore(gps_sendcplt_sem), 1);
    if(_gps_sendcplt_sem_id == NULL) {
        AaSysLogPrintF(LOGLEVEL_ERR, FeatureGps, "%s %d: gps_sendcplt_sem initialize failed",
                    __FUNCTION__, __LINE__);
        return 1;
    }
    AaSysLogPrintF(LOGLEVEL_INF, FeatureGps, "create gps_sendcplt_sem success");


    _gps_recvcplt_sem_id = osSemaphoreCreate(osSemaphore(gps_recvcplt_sem), 1);
    if(_gps_recvcplt_sem_id == NULL) {
        AaSysLogPrintF(LOGLEVEL_ERR, FeatureGps, "%s %d: gps_recvcplt_sem initialize failed",
                    __FUNCTION__, __LINE__);
        return 2;
    }
    AaSysLogPrintF(LOGLEVEL_INF, FeatureGps, "create gps_recvcplt_sem success");


    // will always receive one data from serial and save the data into ringbuffer.
    if(HAL_OK != HAL_UART_Receive_IT(&UartHandle_gps, (u8*)&recv_char, 1)) {
        AaSysLogPrintF(LOGLEVEL_ERR, FeatureGps, "%s %d: HAL_UART_Receive_IT initialize failed",
                    __FUNCTION__, __LINE__);
        return 3;
    }


    osThreadDef(Gps, GpsThread, osPriorityNormal, 0, configMINIMAL_STACK_SIZE);
    _gps_id = AaThreadCreateStartup(osThread(Gps), NULL);
    AaSysLogPrintF(LOGLEVEL_INF, FeatureGps, "create GpsThread success");


    osThreadDef(GpsTest, GpsSendTestThread, osPriorityNormal, 0, configMINIMAL_STACK_SIZE);
    _gps_send_test_id = AaThreadCreateStartup(osThread(GpsTest), NULL);
    AaSysLogPrintF(LOGLEVEL_INF, FeatureGps, "create GpsSendTestThread success");


    return 0;
}


static void GpsDeviceInit()
{
    GpsBufInit();
    GpsUsartInit();

    AaSysLogPrintF(LOGLEVEL_INF, FeatureGps, "gps device initialize success");
}

static void GpsBufInit(void)
{
    memset(_gps_buf.bufData, 0, GPS_SERIAL_RX_BUFFER_NUM * GPS_SERIAL_RX_BUFFER_SIZE);
    _gps_buf.curWriteBuf = _gps_buf.bufData[0];
    _gps_buf.curReadBuf = _gps_buf.bufData[0];
    _gps_buf.curWriteNum = 0;
    _gps_buf.curReadNum = 0;
    _gps_buf.switchFlag  = 0;
}

/** 
 * This is a brief description. 
 * This is a detail description. 
 * @param[in]   inArgName input argument description. 
 * @param[out]  outArgName output argument description.  
 * @retval  Number of available tokens, or -1 in case of incorrect parameters.
 * @retval  
 * @par 
 *      
 * @par 
 *      
 * @par History
 *      2016-07-17 Huang Shengda
 */  
static u8 GpsSendSerialData(u8* data, u32 len)
{
    GpsDataSendByIT(data, len);

    return osSemaphoreWait(_gps_sendcplt_sem_id, GPS_SENDCMPL_TIMEOUT);
}


void GpsWaitForSendCplt()
{
    osSemaphoreRelease(_gps_sendcplt_sem_id);
}


/*  */
void GpsRecvDataFromISR(UART_HandleTypeDef *huart)
{
    static u8 recv_flag = 0;
    static u8 recv_times = 0;           /* 用于控制处理GPS数据的频率 */

    if (recv_char == '$')
    {
        _gps_buf.curWriteNum = 0;
        recv_flag = 1;
    }

    if (recv_flag)
    {
        _gps_buf.curWriteBuf[_gps_buf.curWriteNum] = recv_char;
        _gps_buf.curWriteNum++;
        if (recv_char == '\n')
        {
            recv_flag = 0;
            recv_times++;
        }
    }
    
    HAL_UART_Receive_IT(&UartHandle_gps, (u8*)&recv_char, 1);

    if (recv_times == _gps_freq)
    {
        recv_times = 0;
        /* 交换缓存 */
        _gps_buf.curReadBuf = _gps_buf.curWriteBuf;
        _gps_buf.curReadNum = _gps_buf.curWriteNum;
        _gps_buf.switchFlag = _gps_buf.switchFlag == 0 ? 1 : 0;
        _gps_buf.curWriteBuf = _gps_buf.bufData[_gps_buf.switchFlag];
        _gps_buf.curWriteNum = 0;
        osSemaphoreRelease(_gps_recvcplt_sem_id);
    }
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
