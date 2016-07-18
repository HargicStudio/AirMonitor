
/* Includes ------------------------------------------------------------------*/
#include "cp15.h"
#include "cp15_dev.h"
#include "RingBufferUtils.h"
#include <string.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/** wait for send complete by serial with timeout 1 sec */  
#define CP15_SENDCMPL_TIMEOUT        (1000 * 1)

#define CP15_SERIAL_RX_RINGBUFFER_SIZE       (256)

/* Private variables ---------------------------------------------------------*/

/** Description of the macro */  
osThreadId _cp15_id;
osThreadId _cp15_send_test_id;

/** signal for IT that data send complete */  
static osSemaphoreDef(cp15_sendcplt_sem);
osSemaphoreId _cp15_sendcplt_sem_id;

/** signal for IT that data receive complete */  
static osSemaphoreDef(cp15_recvcplt_sem);
osSemaphoreId _cp15_recvcplt_sem_id;


extern UART_HandleTypeDef UartHandle_cp15;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static void CP15Thread(void const *argument);
static void CP15DeviceInit();
static u8 CP15SendSerialData(u8* data, u32 len);

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
static void CP15Thread(void const *argument)
{
    (void) argument;
    u8 data_buf[32];

    AaSysLogPrintF(LOGLEVEL_INF, FeatureCP15, "CP15Thread started");

    AaSysLogPrintM(FeatureCP15, (const char*)data_buf, 32);

    for (;;)
    {
        if(HAL_OK != HAL_UART_Receive_IT(&UartHandle_cp15, data_buf, 32)) {
            AaSysLogPrintF(LOGLEVEL_ERR, FeatureCP15, "%s %d: HAL_UART_Receive_IT failed",
                        __FUNCTION__, __LINE__);
        }
        osSemaphoreWait(_cp15_recvcplt_sem_id, osWaitForever);
    }
}


static void CP15SendTestThread(void const *argument)
{
    (void) argument;

    AaSysLogPrintF(LOGLEVEL_INF, FeatureCP15, "%s started", __FUNCTION__);

    for(;;)
    {
        osDelay(1000);
        CP15SendSerialData("**Hello, CP15**", strlen("**Hello, CP15**"));
    }
}

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
u8 StartCP15Task()
{
    CP15DeviceInit();


    _cp15_sendcplt_sem_id = osSemaphoreCreate(osSemaphore(cp15_sendcplt_sem), 1);
    if(_cp15_sendcplt_sem_id == NULL) {
        AaSysLogPrintF(LOGLEVEL_ERR, FeatureCP15, "%s %d: cp15_sendcplt_sem initialize failed",
                    __FUNCTION__, __LINE__);
        return 1;
    }
    AaSysLogPrintF(LOGLEVEL_INF, FeatureCP15, "create cp15_sendcplt_sem success");


    _cp15_recvcplt_sem_id = osSemaphoreCreate(osSemaphore(cp15_recvcplt_sem), 1);
    if(_cp15_recvcplt_sem_id == NULL) {
        AaSysLogPrintF(LOGLEVEL_ERR, FeatureCP15, "%s %d: cp15_recvcplt_sem initialize failed",
                    __FUNCTION__, __LINE__);
        return 2;
    }
    AaSysLogPrintF(LOGLEVEL_INF, FeatureCP15, "create cp15_recvcplt_sem success");


    osThreadDef(CP15, CP15Thread, osPriorityNormal, 0, configMINIMAL_STACK_SIZE);
    _cp15_id = AaThreadCreateStartup(osThread(CP15), NULL);
    AaSysLogPrintF(LOGLEVEL_INF, FeatureCP15, "create CP15Thread success");


    osThreadDef(CP15Test, CP15SendTestThread, osPriorityNormal, 0, configMINIMAL_STACK_SIZE);
    _cp15_send_test_id = AaThreadCreateStartup(osThread(CP15Test), NULL);
    AaSysLogPrintF(LOGLEVEL_INF, FeatureCP15, "create CP15SendTestThread success");


    return 0;
}


static void CP15DeviceInit()
{
    CP15UsartInit();
    CP15GpioInit();

    AaSysLogPrintF(LOGLEVEL_INF, FeatureCP15, "cp15 device initialize success");
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
static u8 CP15SendSerialData(u8* data, u32 len)
{
    CP15DataSendByIT(data, len);

    return osSemaphoreWait(_cp15_sendcplt_sem_id, CP15_SENDCMPL_TIMEOUT);
}


void CP15WaitForSendCplt()
{
    osSemaphoreRelease(_cp15_sendcplt_sem_id);
}


void CP15RecvDataFromISR(UART_HandleTypeDef *huart)
{
    osSemaphoreRelease(_cp15_recvcplt_sem_id);
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
