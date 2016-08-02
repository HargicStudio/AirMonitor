
/* Includes ------------------------------------------------------------------*/
#include "gsm.h"
#include "gsm_dev.h"
#include "RingBufferUtils.h"
#include <string.h>
#include "gsm_power_dev.h"
#include "dataRecv.h"
#include "gsmCtrl.h"
#include "format.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/** wait for send complete by serial with timeout 1 sec */  
#define GSM_SENDCMPL_TIMEOUT        (1000 * 1)

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
#define GSM_SERIAL_RX_RINGBUFFER_SIZE       (1024)

ring_buffer_t _gsm_rx_ringbuf;
u8 _gsm_rx_ringbuf_data[GSM_SERIAL_RX_RINGBUFFER_SIZE];
u8 _gsm_rx_buf[GSM_SERIAL_RX_RINGBUFFER_SIZE];

/** Description of the macro */  
static char recv_char;

/* 发送 ringbuf */
/*
#define GSM_TX_RINGBUFFER_SIZE       (1024)

ring_buffer_t _gsm_tx_ringbuf;
u8 _gsm_tx_ringbuf_data[GSM_TX_RINGBUFFER_SIZE];
u8 _gsm_tx_buf[GSM_TX_RINGBUFFER_SIZE];
*/


extern UART_HandleTypeDef UartHandle_gsm;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static void GsmThread(void const *argument);
static void GsmDeviceInit();

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
    u16 len = 0;

    AaSysLogPrintF(LOGLEVEL_INF, FeatureGsm, "GsmThread started");
    
    testFillData();

    for (;;)
    {
        osSemaphoreWait(_gsm_recvcplt_sem_id, osWaitForever);
        
        ring_buffer_consume_enter(&_gsm_rx_ringbuf, _gsm_rx_buf, &len);
        _gsm_rx_buf[len] = 0;
        
        AaSysLogPrintF(LOGLEVEL_INF, FeatureGsm, "GSM Recived: LEN: %d: %s \r\n", len, _gsm_rx_buf);
        
        if (GetAtStatus() == AT_WAIT_RSP || GetAtStatus() == AT_WAIT_CONNECT_RSP
            || GetAtStatus() == AT_WAIT_CONNECT_STU
            || GetAtStatus() == AT_WAIT_REG || GetAtStatus() == AT_WAIT_TO_SEND)
        {
            if (true == ProcessAtResponse(_gsm_rx_buf, len))
            {
                continue;
            }
        }
        
        /* filter AT CMD */
        if (_gsm_rx_buf[0] == 'A' && _gsm_rx_buf[1] == 'T')
        {
            continue;
        }
        
        HandleGsmRecv(_gsm_rx_buf, len);
        
    }
}


static void GsmSendTestThread(void const *argument)
{
    (void) argument;
    u16 times = 0;
    static u16 Interval = 6000;
    static u8 RepInt = 0xff;

    AaSysLogPrintF(LOGLEVEL_INF, FeatureGsm, "%s started", __FUNCTION__);

    /*
    *  发送设计
    *  时间精度 50 ms 
    *  每 50ms 检查是否有数据发送
    *  每到发送时间间隔上报数据到服务器
    *  50 ms次数计数，为免GPS没信号，上报数据，以50ms和UTC时间两个做参考
    */
    for(;;)
    {
        osDelay(50);
        times++;

        // As the interval will change, cal every time.
        if (ConfigGetReportInterval() != RepInt)
        {
            RepInt = ConfigGetReportInterval();
            // Interval = 20 * 60 * RepInt;
            // for test is 2 s
            Interval = 20 * 5;
        }

        /* 间隔时间到 */
        if (times >= Interval)
        {
            times = 0;
            ContructDataUp();
            if (IsSendBufReady())
            {
                SEND_BUF_FLAG_CLEAR();
                SendDataToServer();
                
            }
        }

        if (IsSendResponseReady())
        {
            SEND_RESPONSE_FLAG_CLEAR();
            SendResponseToServer();
            
        }
        
        if (IsGsmWaitCloseFlag())
        {
            GsmWaitCloseFlagClear();
            GsmPowerUpDownOpt(GSM_POWER_DOWN);
            GsmStatusSet(GSM_CLOSED);
        }
        /*
        GsmWaitCloseCountAdd();
        if (IsGsmWaitCloseCountReach() && GsmStatusGet() != GSM_CLOSED)
        {
            GsmWaitCloseCountReset();
            // 执行关机
        }
        */
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
    ConfigInit();

    /* 初始化接收ringbuf */
    ring_buffer_init(&_gsm_rx_ringbuf, _gsm_rx_ringbuf_data, GSM_SERIAL_RX_RINGBUFFER_SIZE);
    
    /* 初始化发送ringbuf */
    //ring_buffer_init(&_gsm_tx_ringbuf, _gsm_tx_ringbuf_data, GSM_TX_RINGBUFFER_SIZE);

    AaSysLogPrintF(LOGLEVEL_INF, FeatureGsm, "create gsm serial rx/tx ringbuffer success");

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
    
    /* Also init the power ctrl */
    GsmPowerGpioInit();

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


void GsmWaitForSendCplt()
{
    osSemaphoreRelease(_gsm_sendcplt_sem_id);
}


void GsmRecvDataFromISR(UART_HandleTypeDef *huart)
{
    
    HAL_UART_Receive_IT(&UartHandle_gsm, (u8*)&recv_char, 1);

    ring_buffer_write_c(&_gsm_rx_ringbuf, recv_char);
    
    if (recv_char == '>')
    {
        ring_buffer_write_c(&_gsm_rx_ringbuf, '\n');
        osSemaphoreRelease(_gsm_recvcplt_sem_id);
        return;
    }
      
    if (recv_char == '\n')
    {
        osSemaphoreRelease(_gsm_recvcplt_sem_id);
    }
    return;
}


/***************** Data Handler *****************/
bool ProcessAtResponse(u8 *buf, u16 len)
{
    switch (GetAtStatus())
    {
    case AT_WAIT_TO_SEND:
      IsAtSuss(buf, ">");
      break;
    case AT_WAIT_CONNECT_RSP:
      IsAtSuss(buf, "CONNECT OK");
      IsAtSuss(buf, "ALREADY CONNECT");
      if (strstr((const char *)buf, "CONNECT FAIL"))
      {
          SetAtStatus(AT_ERROR);
      }
      else
      {
          IsAtSuss(buf, "CONNECT");    /* 透传模式 */
      }
      break;
    case AT_WAIT_REG:
      IsAtSuss(buf, "CHINA");
      break;
    case AT_WAIT_CONNECT_STU:
      IsAtSuss(buf, "CONNECT OK");
      break;
    default:
      IsAtSuss(buf, "OK");
      break;
    }
    
    if (GetAtStatus() == AT_WAIT_CONNECT_STU)
    {
        if (strstr((const char *)buf, "INITIAL") ||
            (strstr((const char *)buf, "CONNECTING")))
        {
            SetAtStatus(AT_ERROR);
            return false;
        }
    }
    else
    {
        if (strstr((const char *)buf, "ERROR"))
        {
            SetAtStatus(AT_ERROR);
            return false;
        }
    }
    
    return false;
}

/* For sending */
extern SEND_BUF_t g_sendBuf;

bool SendDataToServer(void)
{
   // osSemaphoreWait(_gsm_send_test_id, osWaitForever);
    if ( !SendData(g_sendBuf.buf, g_sendBuf.useLen, g_sendBuf.respFlag) )
    {
        GSM_LOG_P0("Send fail!");
        return false;
    }
    //osSemaphoreRelease(_gsm_send_test_id);
    return true;
}

extern SEND_BUF_t g_sendResponse;

bool SendResponseToServer(void)
{
   // osSemaphoreWait(_gsm_send_test_id, osWaitForever);
    if ( !SendData(g_sendResponse.buf, g_sendResponse.useLen, g_sendResponse.respFlag) )
    {
        GSM_LOG_P0("Send Response fail!");
        return false;
    }
    //osSemaphoreRelease(_gsm_send_test_id);
    return true;
}

/************** Data Handler End ****************/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
