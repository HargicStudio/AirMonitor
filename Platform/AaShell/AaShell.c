
/***

History:
[2016-07-13 Ted]: Create

*/


#include <string.h>
#include "AaInclude.h"
#include "RingBufferUtils.h"


/** Description of the macro */  
#define AASHELL_SIGNAL_HANDLE_THREAD_RX     (1<<0)

/** Description of the macro */  
#define AASHELL_SERIAL_RX_RINGBUFFER_SIZE   64

/** Description of the macro */  
#define AASHELL_STACK_SIZE       0x80

/** Description of the macro */  
#define AASHELL_CMDBUFFER_MAX_SIZE      32

/** Description of the macro */  
static osSemaphoreDef(aashell_recvcplt_sem);
osSemaphoreId _aashell_recvcplt_sem_id;

/** Description of the macro */  
osThreadId _aashellrecvdaemon_id;

/** Description of the macro */  
osThreadId _aashellhandledaemon_id;

/** Description of the macro */  
ring_buffer_t _serial_rx_ringbuf;
u8 _serial_rx_ringbuf_data[AASHELL_SERIAL_RX_RINGBUFFER_SIZE];

/** Description of the macro */  
static char recv_char;


extern UART_HandleTypeDef UartHandle_dbg;



static void AaShellSerialHandleThread(void const *arg);
static void AaShellRecvThread(void const *arg);



/** 
 * This is a brief description. 
 * This thread will only handle the cmd data in the ringbuffer from serial com.
 * @param[in]   inArgName input argument description. 
 * @param[out]  outArgName output argument description.  
 * @retval  
 * @retval  
 * @par 
 *      
 * @par 
 *      
 * @par History
 *      2016-07-13 Huang Shengda
 */  
static void AaShellSerialHandleThread(void const *arg)
{
    (void) arg;
    
    u32 used;
    u32 free;
    char cmdbuf[AASHELL_CMDBUFFER_MAX_SIZE];

    AaSysLogPrintF(LOGLEVEL_INF, FeatureAaShell, "%s started", __FUNCTION__);

    for(;;) {
        osSemaphoreWait(_aashell_recvcplt_sem_id, osWaitForever);

        // TODO: get the data and print
        used = ring_buffer_used_space(&_serial_rx_ringbuf);

        if (used <= 1) {
            ring_buffer_consume(&_serial_rx_ringbuf, used);
            continue ;
        }

        free = ring_buffer_free_space(&_serial_rx_ringbuf);

        AaSysLogPrintF(LOGLEVEL_DBG, FeatureAaShell, "%s %d: used %d, free %d", 
                    __FUNCTION__, __LINE__, used, free);

        char* cmdline = cmdbuf;

        // Grab data from the buffer
        do {
            uint8_t* available_data;
            uint32_t bytes_available;

            ring_buffer_get_data( &_serial_rx_ringbuf, &available_data, (u32*)&bytes_available );
            bytes_available = MIN( bytes_available, used );
            memcpy( cmdline, available_data, bytes_available );
            used -= bytes_available;
            cmdline = (cmdline + bytes_available);
            ring_buffer_consume(&_serial_rx_ringbuf, bytes_available);
        } while ( used != 0 );

        if (0 == strncmp(cmdbuf, "hello", strlen("hello"))) {
            AaSysLogPrintF(LOGLEVEL_DBG, FeatureAaShell, "%s %d: get cmd %s", 
                        __FUNCTION__, __LINE__, "hello");
        }

        used = ring_buffer_used_space(&_serial_rx_ringbuf);
        free = ring_buffer_free_space(&_serial_rx_ringbuf);
        AaSysLogPrintF(LOGLEVEL_DBG, FeatureAaShell, "%s %d: used %d, free %d", 
                    __FUNCTION__, __LINE__, used, free);
    }
}

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
 *      2016-07-13 Huang Shengda
 */  
u8 AaShellCEInit()
{
    AaSysLogPrintF(LOGLEVEL_INF, FeatureAaShell, "%s IN", __FUNCTION__);


    ring_buffer_init(&_serial_rx_ringbuf, _serial_rx_ringbuf_data, AASHELL_SERIAL_RX_RINGBUFFER_SIZE);
    AaSysLogPrintF(LOGLEVEL_INF, FeatureAaShell, "create serial rx ringbuffer success");


    _aashell_recvcplt_sem_id = osSemaphoreCreate(osSemaphore(aashell_recvcplt_sem), 1);
    if(_aashell_recvcplt_sem_id == NULL) {
        AaSysLogPrintF(LOGLEVEL_ERR, FeatureAaShell, "%s %d: aashell_recvcplt_sem initialize failed",
                    __FUNCTION__, __LINE__);
        return 1;
    }
    AaSysLogPrintF(LOGLEVEL_INF, FeatureAaShell, "create aashell_recvcplt_sem success");


    // will always receive one data from serial and save the data into ringbuffer.
    if(HAL_OK != HAL_UART_Receive_IT(&UartHandle_dbg, (u8*)&recv_char, 1)) {
        AaSysLogPrintF(LOGLEVEL_ERR, FeatureAaShell, "%s %d: HAL_UART_Receive_IT initialize failed",
                    __FUNCTION__, __LINE__);
        return 2;
    }


    osThreadDef(AaShellSerialHandleDaemon, AaShellSerialHandleThread, osPriorityHigh, 0, AASHELL_STACK_SIZE);
    _aashellhandledaemon_id = AaThreadCreateStartup(osThread(AaShellSerialHandleDaemon), NULL);
    if(_aashellhandledaemon_id == NULL) {
        AaSysLogPrintF(LOGLEVEL_ERR, FeatureAaShell, "%s %d: AaShell serial handle Daemon initialize failed",
                    __FUNCTION__, __LINE__);
        return 3;
    }
    AaSysLogPrintF(LOGLEVEL_INF, FeatureAaShell, "create AsShell serial handle daemon success");


    AaSysLogPrintF(LOGLEVEL_INF, FeatureAaShell, "%s EXIT success", __FUNCTION__);

    return 0;
}

/** 
 * This is a brief description. 
 * This function will be called when rx complete interrupt is triggered,
 *   and will unlock the receive handle thread to save the data.
 * @param[in]   inArgName input argument description. 
 * @param[out]  outArgName output argument description.  
 * @retval  
 * @retval  
 * @par 
 *      
 * @par 
 *      
 * @par History
 *      2016-07-13 Huang Shengda
 */  
void AaShellRecvDataFromISR(UART_HandleTypeDef *huart)
{
    ring_buffer_write(&_serial_rx_ringbuf, (u8*)&recv_char, 1);

    HAL_UART_Receive_IT(&UartHandle_dbg, (u8*)&recv_char, 1);

    if (recv_char == 0x0d || recv_char == 0x0a) {
        osSemaphoreRelease(_aashell_recvcplt_sem_id);
    }
}




// end of file
