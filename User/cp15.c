
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
char recv_char_cp15;

#define MAX_CP15_BUFF_SIZE              256
#define MAX_CP15_RECV_SZIE              50

/* recv buffer */
ring_buffer_t _cp15_ringbuf;
u8 _cp15_ringbuf_data[MAX_CP15_BUFF_SIZE];

/* 接收buffer */
u8 _cp15_buf[MAX_CP15_RECV_SZIE];

/* 中断接收buf */
u8 _cp15_int_buf[35];

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
    s16 k = 0;
    s16 b = 0;
    

    CP15_LOG_P0("CP15Thread started");

    for (;;)
    {
        osSemaphoreWait(_cp15_recvcplt_sem_id, osWaitForever);
        
        ring_buffer_consume_str(&_cp15_ringbuf, _cp15_buf, &len);
        
        //CP15_LOG_P2("Handle CP15: head: %d, tail: %d", _cp15_ringbuf.head, _cp15_ringbuf.tail);

        if (_cp15_buf[0] != 0x32 || _cp15_buf[1] != 0x3D ||
            _cp15_buf[2] != 0 || _cp15_buf[3] != 0x1c)
        {
            continue;
        }
        val25 = _cp15_buf[6]<<8 | _cp15_buf[7];
        
        k = ConfigGetpm25K();
        b = ConfigGetpm25B();
        
        val25 = k * val25 + b;
        
        StorePmInfo(val25, &g_pm25);
        
        /* 计算益杉德PM10*/
        val10 = _cp15_buf[8]<<8 | _cp15_buf[9];
        
        k = ConfigGetpm10K();
        b = ConfigGetpm10B();
        
        val10 = k * val10 + b;
        
        StorePmInfo(val10, &g_pm10);
        
        CP15_LOG_P2("received PM2.5 : %d PM10 : %d\r\n", val25, val10);
    }
}


static void CP15SendTestThread(void const *argument)
{
    (void) argument;

    CP15_LOG_P1("%s started", __FUNCTION__);

    for(;;)
    {
        SetAutoInterval(10);
        osDelay(5000);
        OpenAutoOutput();
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
    CP15_LOG_P0("create CP 15 ringbuffer success");

    _cp15_sendcplt_sem_id = osSemaphoreCreate(osSemaphore(cp15_sendcplt_sem), 1);
    if(_cp15_sendcplt_sem_id == NULL) {
        CP15_LOG_P2("%s %d: cp15_sendcplt_sem initialize failed",
                    __FUNCTION__, __LINE__);
        return 1;
    }
    CP15_LOG_P0("create cp15_sendcplt_sem success");


    _cp15_recvcplt_sem_id = osSemaphoreCreate(osSemaphore(cp15_recvcplt_sem), 1);
    if(_cp15_recvcplt_sem_id == NULL) {
        CP15_LOG_P2("ERROR: %s %d: cp15_recvcplt_sem initialize failed",
                    __FUNCTION__, __LINE__);
        return 2;
    }
    CP15_LOG_P0("create cp15_recvcplt_sem success");
    
    // will always receive one data from serial and save the data into ringbuffer.
    if(HAL_OK != HAL_UART_Receive_IT(&UartHandle_cp15, (u8*)&recv_char_cp15, 1)) {
        CP15_LOG_P2("ERROR: %s %d: HAL_UART_Receive_IT initialize failed",
                    __FUNCTION__, __LINE__);
        return 3;
    }

    osThreadDef(CP15, CP15Thread, osPriorityNormal, 0, configMINIMAL_STACK_SIZE);
    _cp15_id = AaThreadCreateStartup(osThread(CP15), NULL);
    CP15_LOG_P0("create CP15Thread success");


    osThreadDef(CP15Test, CP15SendTestThread, osPriorityNormal, 0, configMINIMAL_STACK_SIZE);
    _cp15_send_test_id = AaThreadCreateStartup(osThread(CP15Test), NULL);
    CP15_LOG_P0("create CP15SendTestThread success");


    return 0;
}


static void CP15DeviceInit()
{
    CP15UsartInit();
    //CP15GpioInit();

    CP15_LOG_P0("cp15 device initialize success");
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
    static u8 count = 2;
    static u8 freqCtr = 0;
    
    if (recv_char_cp15 == 0x32 && flag == 0)
    {
        flag = 1;
        goto GO_OUT;
    }
    
    if (flag == 1 && recv_char_cp15 == 0x3D)
    {
        flag = 2;
        goto GO_OUT;
    }
    
    if (flag != 2)
    {
        flag = 0;
        count = 2;
        goto GO_OUT;
    }
    
    _cp15_int_buf[count] = recv_char_cp15;
    count++;
    
    if (count == 32)
    {
        count = 0;
        flag = 0;
        
        _cp15_int_buf[0] = 0x32;
        _cp15_int_buf[1] = 0x3D;
        if (CheckCP15Crc(_cp15_int_buf, 32))
        {
            freqCtr++;
            if (freqCtr < 5)
            {
                goto GO_OUT;
            }
            
            _cp15_int_buf[32] = 0xff;
            _cp15_int_buf[33] = 0xff;
            _cp15_int_buf[34] = 0xff;
            ring_buffer_write(&_cp15_ringbuf, _cp15_int_buf, sizeof(_cp15_int_buf));
            osSemaphoreRelease(_cp15_recvcplt_sem_id);
            
            //CP15_LOG_P2("CP15 head: %d, tail:%d", _cp15_ringbuf.head, _cp15_ringbuf.tail);
        }
        else
        {
            //CP15_LOG_P0("_+_+_+_+_+_+ CP15 CRC error! _+_+_+_+_+_+");
        }
                
    }
    
GO_OUT:
    HAL_UART_Receive_IT(&UartHandle_cp15, (u8*)&recv_char_cp15, 1);
}


/*
*  CP 15数据处理
*
*/
void OpenAutoOutput()
{
  u8 cmd[16] = {0x33, 0x3e, 0x00, 0x0c, 0xa2, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x20};
  
  CalcuteCp15Crc(cmd, 16);
  
  CP15DataSendByIT(cmd, 16);
}

void SetAutoInterval(u16 interval)
{
  u8 cmd[16] = {0x33, 0x3e, 0x00, 0x0c, 0xa3, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x2A};
  
  cmd[6] = interval >> 8;
  cmd[7] = interval & 0xff;
  
  CalcuteCp15Crc(cmd, 16);
  
  CP15DataSendByIT(cmd, 16);
}


bool CheckCP15Crc(u8 *buf, u16 size)
{
     u16 i = 0;
     u16 sum = 0;
     for (i=0; i<size - 2; i++)
     {
         sum += buf[i];
     }
     
     return ((buf[size-2] << 8 | buf[size-1]) == sum);
}

void CalcuteCp15Crc(u8 *buf, u16 size)
{
    u16 i = 0;
    u16 sum = 0;
    
    for (i=0; i<size - 2; i++)
    {
       sum += buf[i];
    }
    
    buf[size-2] = (sum>>8) & 0xff;
    buf[size -1] = (sum) & 0xff;
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
