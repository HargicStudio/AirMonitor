
/* Includes ------------------------------------------------------------------*/
#include "gps.h"
#include "gps_dev.h"
#include "RingBufferUtils.h"
#include <string.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/** wait for send complete by serial with timeout 1 sec */  
#define GPS_SENDCMPL_TIMEOUT        (1000 * 1)

#define GPS_SERIAL_RX_RINGBUFFER_SIZE       (256)

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
ring_buffer_t _gps_rx_ringbuf;
u8 _gps_rx_ringbuf_data[GPS_SERIAL_RX_RINGBUFFER_SIZE];

/** Description of the macro */  
static char recv_char;


extern UART_HandleTypeDef UartHandle_gps;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static void GpsThread(void const *argument);
static void GpsDeviceInit();
static u8 GpsSendSerialData(u8* data, u32 len);

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
    u32 used;
    u32 free;

    AaSysLogPrintF(LOGLEVEL_INF, FeatureGps, "GpsThread started");

    for (;;)
    {
        osSemaphoreWait(_gps_recvcplt_sem_id, osWaitForever);

        // receive one data
        used = ring_buffer_used_space(&_gps_rx_ringbuf);
        free = ring_buffer_free_space(&_gps_rx_ringbuf);
        AaSysLogPrintF(LOGLEVEL_DBG, FeatureGps, "%s %d: used %d, free %d", 
                    __FUNCTION__, __LINE__, used, free);

        // Grab data from the buffer
        // do {
        //     uint8_t* available_data;
        //     uint32_t bytes_available;

        //     ring_buffer_get_data( &_gps_rx_ringbuf, &available_data, (u32*)&bytes_available );
        //     bytes_available = MIN( bytes_available, used );
        //     memcpy( recv_data, available_data, bytes_available );
        //     used -= bytes_available;
        //     recv_data = (recv_data + bytes_available);
        //     ring_buffer_consume(&_gps_rx_ringbuf, bytes_available);
        // } while ( used != 0 );
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


    ring_buffer_init(&_gps_rx_ringbuf, _gps_rx_ringbuf_data, GPS_SERIAL_RX_RINGBUFFER_SIZE);
    AaSysLogPrintF(LOGLEVEL_INF, FeatureGps, "create gps serial rx ringbuffer success");


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
    GpsUsartInit();

    AaSysLogPrintF(LOGLEVEL_INF, FeatureGps, "gps device initialize success");
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


void GpsRecvDataFromISR(UART_HandleTypeDef *huart)
{
    ring_buffer_write(&_gps_rx_ringbuf, (u8*)&recv_char, 1);

    HAL_UART_Receive_IT(&UartHandle_gps, (u8*)&recv_char, 1);

    osSemaphoreRelease(_gps_recvcplt_sem_id);
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
