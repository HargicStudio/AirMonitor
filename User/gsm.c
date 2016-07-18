
/* Includes ------------------------------------------------------------------*/
#include "gsm.h"
#include "gsm_dev.h"
#include "RingBufferUtils.h"
#include <string.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/** wait for send complete by serial with timeout 1 sec */  
#define GSM_SENDCMPL_TIMEOUT        (1000 * 1)

#define GSM_SERIAL_RX_RINGBUFFER_SIZE       (256)

/* Private variables ---------------------------------------------------------*/

/** Description of the macro */  
osThreadId _gsm_id;
osThreadId _gsm_send_test_id;

/** signal for IT that data send complete */  
static osSemaphoreDef(gsm_sendcplt_sem);
osSemaphoreId _gsm_sendcplt_sem_id;

/** signal for IT that data receive complete */  
static osSemaphoreDef(gsm_recvcplt_sem);
osSemaphoreId _gsm_recvcplt_sem_id;

/** Description of the macro */  
ring_buffer_t _gsm_rx_ringbuf;
u8 _gsm_rx_ringbuf_data[GSM_SERIAL_RX_RINGBUFFER_SIZE];

/** Description of the macro */  
static char recv_char;


extern UART_HandleTypeDef UartHandle_gsm;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static void GsmThread(void const *argument);
static void GsmDeviceInit();
static u8 GsmSendSerialData(u8* data, u32 len);

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
static void GsmThread(void const *argument)
{
    (void) argument;
    u32 used;
    u32 free;

    AaSysLogPrintF(LOGLEVEL_INF, FeatureGsm, "GsmThread started");

    for (;;)
    {
        osSemaphoreWait(_gsm_recvcplt_sem_id, osWaitForever);

        // receive one data
        used = ring_buffer_used_space(&_gsm_rx_ringbuf);
        free = ring_buffer_free_space(&_gsm_rx_ringbuf);
        AaSysLogPrintF(LOGLEVEL_DBG, FeatureGsm, "%s %d: used %d, free %d", 
                    __FUNCTION__, __LINE__, used, free);

        // Grab data from the buffer
        // do {
        //     uint8_t* available_data;
        //     uint32_t bytes_available;

        //     ring_buffer_get_data( &_gsm_rx_ringbuf, &available_data, (u32*)&bytes_available );
        //     bytes_available = MIN( bytes_available, used );
        //     memcpy( recv_data, available_data, bytes_available );
        //     used -= bytes_available;
        //     recv_data = (recv_data + bytes_available);
        //     ring_buffer_consume(&_gsm_rx_ringbuf, bytes_available);
        // } while ( used != 0 );
    }
}


static void GsmSendTestThread(void const *argument)
{
    (void) argument;

    AaSysLogPrintF(LOGLEVEL_INF, FeatureGsm, "%s started", __FUNCTION__);

    for(;;)
    {
        osDelay(1000);
        GsmSendSerialData("**Hello, Gsm**", strlen("**Hello, Gsm**"));
    }
}

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
u8 StartGsmTask()
{
    GsmDeviceInit();


    ring_buffer_init(&_gsm_rx_ringbuf, _gsm_rx_ringbuf_data, GSM_SERIAL_RX_RINGBUFFER_SIZE);
    AaSysLogPrintF(LOGLEVEL_INF, FeatureGsm, "create gsm serial rx ringbuffer success");


    _gsm_sendcplt_sem_id = osSemaphoreCreate(osSemaphore(gsm_sendcplt_sem), 1);
    if(_gsm_sendcplt_sem_id == NULL) {
        AaSysLogPrintF(LOGLEVEL_ERR, FeatureGsm, "%s %d: gsm_sendcplt_sem initialize failed",
                    __FUNCTION__, __LINE__);
        return 1;
    }
    AaSysLogPrintF(LOGLEVEL_INF, FeatureGsm, "create gsm_sendcplt_sem success");


    _gsm_recvcplt_sem_id = osSemaphoreCreate(osSemaphore(gsm_recvcplt_sem), 1);
    if(_gsm_recvcplt_sem_id == NULL) {
        AaSysLogPrintF(LOGLEVEL_ERR, FeatureGsm, "%s %d: gsm_recvcplt_sem initialize failed",
                    __FUNCTION__, __LINE__);
        return 2;
    }
    AaSysLogPrintF(LOGLEVEL_INF, FeatureGsm, "create gsm_recvcplt_sem success");


    // will always receive one data from serial and save the data into ringbuffer.
    if(HAL_OK != HAL_UART_Receive_IT(&UartHandle_gsm, (u8*)&recv_char, 1)) {
        AaSysLogPrintF(LOGLEVEL_ERR, FeatureGsm, "%s %d: HAL_UART_Receive_IT initialize failed",
                    __FUNCTION__, __LINE__);
        return 3;
    }


    osThreadDef(Gsm, GsmThread, osPriorityNormal, 0, configMINIMAL_STACK_SIZE);
    _gsm_id = AaThreadCreateStartup(osThread(Gsm), NULL);
    AaSysLogPrintF(LOGLEVEL_INF, FeatureGsm, "create GsmThread success");


    osThreadDef(GsmTest, GsmSendTestThread, osPriorityNormal, 0, configMINIMAL_STACK_SIZE);
    _gsm_send_test_id = AaThreadCreateStartup(osThread(GsmTest), NULL);
    AaSysLogPrintF(LOGLEVEL_INF, FeatureGsm, "create GsmSendTestThread success");


    return 0;
}


static void GsmDeviceInit()
{
    GsmUsartInit();

    AaSysLogPrintF(LOGLEVEL_INF, FeatureGsm, "gsm device initialize success");
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
static u8 GsmSendSerialData(u8* data, u32 len)
{
    GsmDataSendByIT(data, len);

    return osSemaphoreWait(_gsm_sendcplt_sem_id, GSM_SENDCMPL_TIMEOUT);
}


void GsmWaitForSendCplt()
{
    osSemaphoreRelease(_gsm_sendcplt_sem_id);
}


void GsmRecvDataFromISR(UART_HandleTypeDef *huart)
{
    ring_buffer_write(&_gsm_rx_ringbuf, (u8*)&recv_char, 1);

    HAL_UART_Receive_IT(&UartHandle_gsm, (u8*)&recv_char, 1);

    osSemaphoreRelease(_gsm_recvcplt_sem_id);
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
