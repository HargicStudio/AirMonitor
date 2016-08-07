
/* Includes ------------------------------------------------------------------*/
#include "cp15.h"
#include "cp15_dev.h"
#include "RingBufferUtils.h"
#include <string.h>
#include "dataHandler.h"
#include "common.h"

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

/** Description of the macro */  
static char recv_char;

#define MAX_CP15_BUFF_SIZE              256
#define MAX_CP15_RECV_SZIE              50

/* recv buffer */
ring_buffer_t _cp15_ringbuf;
u8 _cp15_ringbuf_data[MAX_CP15_BUFF_SIZE];

/* 接收buffer */
u8 _cp15_buf[MAX_CP15_RECV_SZIE];


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
    u16 len = 0;
    u16 val25 = 0;
    u16 val10 = 0;

    AaSysLogPrintF(LOGLEVEL_INF, FeatureCP15, "CP15Thread started");

    for (;;)
    {
        osSemaphoreWait(_cp15_recvcplt_sem_id, osWaitForever);
        
        ring_buffer_consume_enter(&_cp15_ringbuf, _cp15_buf, &len);

        if (_cp15_buf[0] != 0 || _cp15_buf[1] != 0x1c)
        {
            continue;
        }
        val25 = _cp15_buf[4]<<8 | _cp15_buf[5];
        
        StorePmInfo(val25, &g_pm25);
        
        /* 计算益杉德PM10*/
        val10 = _cp15_buf[6]<<8 | _cp15_buf[7];
        
        StorePmInfo(val10, &g_pm10);
        
        /*
        AaSysLogPrintF(LOGLEVEL_INF, FeatureGsm, "PM2.5 %02x %02x %02x %02x %02x %02x %02x %02x \r\n", 
                       _cp15_buf[0], _cp15_buf[1], _cp15_buf[2], _cp15_buf[3], _cp15_buf[4], _cp15_buf[5],
                       _cp15_buf[6], _cp15_buf[7]);
        */
        AaSysLogPrintF(LOGLEVEL_INF, FeatureGsm, "received PM2.5 : %d PM10 : %d\r\n", val25, val10);
    }
}


static void CP15SendTestThread(void const *argument)
{
    (void) argument;

    AaSysLogPrintF(LOGLEVEL_INF, FeatureCP15, "%s started", __FUNCTION__);

    for(;;)
    {
        OpenAutoOutput();
        osDelay(5000);
        SetAutoInterval(10);
        osDelay(50000);
        //GSM_LOG_P0("**Hello, CP15**");
        
        /* 设置完成退出线程 */
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

    ring_buffer_init(&_cp15_ringbuf, _cp15_ringbuf_data, MAX_CP15_BUFF_SIZE);
    AaSysLogPrintF(LOGLEVEL_INF, FeatureGsm, "create CP 15 ringbuffer success");

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
    
    // will always receive one data from serial and save the data into ringbuffer.
    if(HAL_OK != HAL_UART_Receive_IT(&UartHandle_cp15, (u8*)&recv_char, 1)) {
        AaSysLogPrintF(LOGLEVEL_ERR, FeatureGsm, "%s %d: HAL_UART_Receive_IT initialize failed",
                    __FUNCTION__, __LINE__);
        return 3;
    }

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
    //CP15GpioInit();

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
    static u8 flag = 0;
    static u8 count = 0;
    
    HAL_UART_Receive_IT(&UartHandle_cp15, (u8*)&recv_char, 1);
    if (recv_char == 0x32 && flag == 0)
    {
        flag = 1;
        return;
    }
    
    if (flag == 1 && recv_char == 0x3D)
    {
        flag = 2;
        return;
    }
    
    if (flag != 2)
    {
        flag = 0;
        return;
    }
    
    count++;
    ring_buffer_write_c(&_cp15_ringbuf, recv_char);
    if (count == 18)
    {
        count = 0;
        flag = 0;
        ring_buffer_write_c(&_cp15_ringbuf, '\n');
        osSemaphoreRelease(_cp15_recvcplt_sem_id);
    }
}


/*
*  CP 15数据处理
*
*/
void OpenAutoOutput()
{
  u8 cmd[20] = {0x33, 0x3e, 0x00, 0x0c, 0xa2, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x20};
  
  CP15DataSendByIT(cmd, 16);
}

void SetAutoInterval(u16 interval)
{
  u8 cmd[20] = {0x33, 0x3e, 0x00, 0x0c, 0xa3, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x25};
  
  cmd[6] = interval >> 8;
  cmd[7] = interval & 0xff;
  
  CP15DataSendByIT(cmd, 16);
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
