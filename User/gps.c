
/* Includes ------------------------------------------------------------------*/
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
#define GPS_SERIAL_RX_RINGBUFFER_SIZE            512
#define MAX_GPS_INT_BUF              100

/* Private variables ---------------------------------------------------------*/

/** Description of the macro */  
osThreadId _gps_id;
osThreadId _gps_send_test_id;

/** signal for IT that data send complete */
/*
static osSemaphoreDef(gps_sendcplt_sem);
osSemaphoreId _gps_sendcplt_sem_id;
*/

/** signal for IT that data receive complete */  
static osSemaphoreDef(gps_recvcplt_sem);
osSemaphoreId _gps_recvcplt_sem_id;

/* ringbuf */
ring_buffer_t _gps_rx_ringbuf;
u8 _gps_rx_ringbuf_data[GPS_SERIAL_RX_RINGBUFFER_SIZE];

/* 接收处理buf */
u8 _gps_buf[MAX_GPS_INT_BUF];

/* 中断接收buf */
u8 _gps_int_buf[MAX_GPS_INT_BUF];

/* 控制GPS接收频率，会从配置读取。最好是能通过直接设置GPS模块控制 */
u8 _gps_freq = 10;

/** Description of the macro */  
char recv_char_gps;

/* 用于计算和保存信息 */
gps_process_data gps;



extern UART_HandleTypeDef UartHandle_gps;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static void GpsThread(void const *argument);
static void GpsDeviceInit();
//static u8 GpsSendSerialData(u8* data, u32 len);

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
    u16 len = 0;
    (void) argument;

    GPS_LOG_P0("GpsThread started");

    for (;;)
    {
        osSemaphoreWait(_gps_recvcplt_sem_id, osWaitForever);
        
        ring_buffer_consume_enter(&_gps_rx_ringbuf, _gps_buf, &len);
        
        if (len < 5)
        {
            continue;
        }

        // receive one data
        /*
        GPS_LOG_P4("Receved GPS: Head: %d, Tail: %d, len: %d, %s \n",
                    _gps_rx_ringbuf.head, _gps_rx_ringbuf.tail, len, _gps_buf);*/

        GPS_Analysis(&gps, _gps_buf);

        StoreCoordInfo(gps.latitude, gps.longitude, &g_coord);
    }
}

/*
static void GpsSendTestThread(void const *argument)
{
    (void) argument;

    GPS_LOG_P1("%s started", __FUNCTION__);

    for(;;)
    {
        osDelay(1000);
        GpsSendSerialData("**Hello, Gps**", strlen("**Hello, Gps**"));
    }
}*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
u8 StartGpsTask()
{
    GpsDeviceInit();

    ring_buffer_init(&_gps_rx_ringbuf, _gps_rx_ringbuf_data, GPS_SERIAL_RX_RINGBUFFER_SIZE);
    GPS_LOG_P0("create gps serial rx ringbuffer success");

/*
    _gps_sendcplt_sem_id = osSemaphoreCreate(osSemaphore(gps_sendcplt_sem), 1);
    if(_gps_sendcplt_sem_id == NULL) {
        AaSysLogPrintF(LOGLEVEL_ERR, FeatureGps, "%s %d: gps_sendcplt_sem initialize failed",
                    __FUNCTION__, __LINE__);
        return 1;
    }
    GPS_LOG_P0("create gps_sendcplt_sem success");
*/

    _gps_recvcplt_sem_id = osSemaphoreCreate(osSemaphore(gps_recvcplt_sem), 1);
    if(_gps_recvcplt_sem_id == NULL) {
      GPS_LOG_P2("ERROR: %s %d: gps_recvcplt_sem initialize failed",
                    __FUNCTION__, __LINE__);
        return 2;
    }
    GPS_LOG_P0("create gps_recvcplt_sem success");


    // will always receive one data from serial and save the data into ringbuffer.
    if(HAL_OK != HAL_UART_Receive_IT(&UartHandle_gps, (u8*)&recv_char_gps, 1)) {
      GPS_LOG_P2("ERROR: %s %d: HAL_UART_Receive_IT initialize failed",
                    __FUNCTION__, __LINE__);
        return 3;
    }


    osThreadDef(Gps, GpsThread, osPriorityNormal, 0, configMINIMAL_STACK_SIZE*2);
    _gps_id = AaThreadCreateStartup(osThread(Gps), NULL);
    GPS_LOG_P0("create GpsThread success");

/*
    osThreadDef(GpsTest, GpsSendTestThread, osPriorityAboveNormal, 0, configMINIMAL_STACK_SIZE);
    _gps_send_test_id = AaThreadCreateStartup(osThread(GpsTest), NULL);
    GPS_LOG_P0("create GpsSendTestThread success");
*/

    return 0;
}


static void GpsDeviceInit()
{
    GpsUsartInit();

    GPS_LOG_P0("gps device initialize success");
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
/*
static u8 GpsSendSerialData(u8* data, u32 len)
{
    GpsDataSendByIT(data, len);

    return osSemaphoreWait(_gps_sendcplt_sem_id, GPS_SENDCMPL_TIMEOUT);
}


void GpsWaitForSendCplt()
{
    osSemaphoreRelease(_gps_sendcplt_sem_id);
}
*/

/*  */
void GpsRecvDataFromISR(UART_HandleTypeDef *huart)
{
    static u8 recv_flag = 0;
    static u8 recv_times = 0;           /* 用于控制处理GPS数据的频率 */
    static u16 rcvCnt = 0;

    if (recv_char_gps == '$')
    {
        rcvCnt = 0;
        recv_flag = 1;
    }

    if (recv_flag)
    {
        _gps_int_buf[rcvCnt] = recv_char_gps;
        rcvCnt++;
        if (recv_char_gps == '\n')
        {
            recv_flag = 0;
            // GPRMC
            
            if (_gps_int_buf[1] == 'G' && _gps_int_buf[2] == 'P'
                  && _gps_int_buf[3] == 'R' && _gps_int_buf[4] == 'M'
                  && _gps_int_buf[5] == 'C')
            {
                recv_times++;
                
                if (recv_times == _gps_freq )
                {
                    recv_times = 0;
                    _gps_int_buf[rcvCnt] = 0;
                    //GPS_LOG_P2("recv GPS: Len: %d, %s", rcvCnt, _gps_int_buf);
                    ring_buffer_write(&_gps_rx_ringbuf, _gps_int_buf, rcvCnt);
                    osSemaphoreRelease(_gps_recvcplt_sem_id);
                }
            }
            
            rcvCnt = 0;
        }
    }
    
    HAL_UART_Receive_IT(&UartHandle_gps, (u8*)&recv_char_gps, 1);
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
