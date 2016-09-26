/***

History:
[2015-09-30 Ted]: Create file.
[2016-04-21 Ted]: merge code to FreeRTOS platform as stdio usart.
[2016-05-21 Ted]: support BipBuffer and DMA for uart tx debug.
[2016-09-02 Ted]: optimize code.

*/


#include "AaSysLogSystem.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>
#include "BipBuffer.h"
#include "print_com.h"


/** uart tx buffer size */  
#define AASYSLOG_BIPBUFFER_SIZE     (1024*4)

/** aasyslog daemon stack size */  
#define AASYSLOGDEAMON_STACK_SIZE   ( (u16) 0x80 )


/** Signal flag for AaSysLogDeamon */  
#define SIG_BIT_TX          (1<<0)
// #define SIG_BIT_TX_CPLT     (1<<1)


/** caculate syslog bip buffer max used size */
static u16 _aasyslog_max_bipbuf_used = 0;

/** bip buffer manage for uart tx */  
static CBipBuffer* _p_bip_buffer = NULL;


/** AaSysLog manage */ 
SAaSysLog _aasyslog_mng = { .processGetBip_callback = NULL, .processPrint_callback = NULL };


/** mutex for AaSysLog process */  
osMutexId _aasyslog_mutex_id;

/** signal for DMA/IT that data send complete */  
osSemaphoreId _aasyslog_sendcplt_sem_id;


/** Daemon thread id for AaSysLog */  
osThreadId _aasyslogdaemon_id;



static void AaSysLogDeamon(void const *arg);



/** 
 * This is a brief description. 
 * Will handle Bip Buffer, trigger send data when there is available data in Bip buffer.
 * @param[in]   inArgName input argument description. 
 * @param[out]  always 0 
 * @retval  
 * @retval  
 * @par 
 *      
 * @par 
 *      
 * @par History
 *      2016-5-21 Ted
 */  
static void AaSysLogDeamon(void const *arg)
{
    (void) arg;
    osEvent evt;
    u32 block_size;
    char* block_addr;

    AaSysLogPrintF(LOGLEVEL_INF, FeatureSysLog, "AaSysLogDeamon started");

    for(;;)
    {
        // !!! should not call AaSysLogPrintF during bip buffer data is sending

        evt = osSignalWait(SIG_BIT_TX, osWaitForever);
        
        if(evt.status == osEventSignal && evt.value.signals == SIG_BIT_TX)
        {
            do {
                if(_aasyslog_mng.processGetBip_callback == NULL) 
                {
                    break;
                }
                
                block_addr = CBipBuffer_Get(_p_bip_buffer, &block_size);
                if(block_addr == NULL || block_size == 0) 
                {
                    break;
                }
                
                _aasyslog_mng.processGetBip_callback(block_addr, block_size);

                // !!! we test failed in our local when using signal to detect sending complete
                // !!! so we use Semaphore instead
                osSemaphoreWait(_aasyslog_sendcplt_sem_id, osWaitForever);
                
                CBipBuffer_Decommit(_p_bip_buffer, block_size);
            } while(block_addr == NULL || block_size == 0);
        }
    }
}


/** 
 * This is a brief description. 
 * This AaSysLogInit should depand on AaMemInit
 * @param[in]   inArgName input argument description. 
 * @param[out]  always 0 
 * @retval  
 * @retval  
 * @par 
 *      
 * @par 
 *      
 * @par History
 *      2016-5-21 Ted
 */ 
u8 AaSysLogGetBipRegister(void(*function)(char*, u32))
{
    _aasyslog_mng.processGetBip_callback = function;
    return 0;
}


/** 
 * This is a brief description. 
 * This AaSysLogInit should depand on AaMemInit
 * @param[in]   inArgName input argument description. 
 * @param[out]  always 0 
 * @retval  
 * @retval  
 * @par 
 *      
 * @par 
 *      
 * @par History
 *      2016-5-21 Ted
 */ 
u8 AaSysLogSendCplt()
{
    // should not call AaSysLogPrintF during bip buffer data is sending and interrupt trigger
    
//    osSignalSet(_aasyslogdaemon_id, SIG_BIT_TX_CPLT);
//    AaSysLogPrintF(LOGLEVEL_DBG, FeatureSysLog, "set tx_cplt signal");

    osSemaphoreRelease(_aasyslog_sendcplt_sem_id);

    return 0;
}

/** 
 * CCS Platform AaSysLog service computer environment initialization. 
 * AaSysLog service computer environment initialize should depand on AaMemInit.
 * @param[in]   inArgName input argument description. 
 * @param[out]  outArgName 
 * @retval      return error code. 0 is ok otherwise failed.
 * @retval  
 * @par 
 *      
 * @par 
 *      
 * @par History
 *      2016-05-21 Ted: create function.
 *      2016-09-02 Ted: optimize code.
 */  
u8 AaSysLogCEInit()
{
    _p_bip_buffer = AaMemCalloc(1, AASYSLOG_BIPBUFFER_SIZE);
    if(_p_bip_buffer == NULL)
    {
        AaSysLogPrintF( LOGLEVEL_ERR, FeatureSysLog, "%s %d: AaSysLog CE initialize failed, Reason: Bip buffer calloc failed",
                        __FUNCTION__, __LINE__);

        return 1;
    }
    
    AaSysLogPrintF(LOGLEVEL_INF, FeatureSysLog, "get _p_bip_buffer pointer %p", _p_bip_buffer);
    CBipBuffer_Construct(_p_bip_buffer, AASYSLOG_BIPBUFFER_SIZE);
    AaSysLogPrintF(LOGLEVEL_INF, FeatureSysLog, "create _p_bip_buffer success");


    osMutexDef(aasyslog_mutex);
    _aasyslog_mutex_id = osMutexCreate(osMutex(aasyslog_mutex));
    if(_aasyslog_mutex_id == NULL) 
    {
        AaSysLogPrintF( LOGLEVEL_ERR, FeatureSysLog, "%s %d: AaSysLog CE initialize failed, Reason: aasyslog_mutex initialize failed",
                        __FUNCTION__, __LINE__);
        return 2;
    }
    AaSysLogPrintF(LOGLEVEL_INF, FeatureSysLog, "create aasyslog_mutex success");


    osSemaphoreDef(aasyslog_sendcplt_sem);
    _aasyslog_sendcplt_sem_id = osSemaphoreCreate(osSemaphore(aasyslog_sendcplt_sem), 1);
    if(_aasyslog_sendcplt_sem_id == NULL)
    {
        AaSysLogPrintF( LOGLEVEL_ERR, FeatureSysLog, "%s %d: AaSysLog CE initialize failed, Reason: aasyslog_sendcplt_sem initialize failed",
                        __FUNCTION__, __LINE__);
        return 3;
    }
    AaSysLogPrintF(LOGLEVEL_INF, FeatureSysLog, "create aasyslog_sendcplt_sem success");


    AaSysLogPrintF(LOGLEVEL_INF, FeatureSysLog, "AaSysLog CE initialize success");

    return 0;
}


/** 
 * Create AaSysLog service daemon. 
 * It should be called after AaSysLog CE initialized.
 * @param[in]   inArgName input argument description. 
 * @param[out]  outArgName
 * @retval      return error code. 0 is ok otherwise failed.
 * @retval  
 * @par 
 *      
 * @par 
 *      
 * @par History
 *      2016-05-21 Ted: create function.
 *      2016-09-02 Ted: optimize code.
 */  
u8 AaSysLogCreateDeamon()
{
    osThreadDef(AaSysLogDeamon, AaSysLogDeamon, osPriorityHigh, 0, AASYSLOGDEAMON_STACK_SIZE);
    _aasyslogdaemon_id = AaThreadCreateStartup(osThread(AaSysLogDeamon), NULL);
    if(_aasyslogdaemon_id == NULL) 
    {
        AaSysLogPrintF( LOGLEVEL_ERR, FeatureSysLog, "%s %d: create AaSysLog service daemon failed",
                        __FUNCTION__, __LINE__);
        return 1;
    }
    AaSysLogPrintF(LOGLEVEL_INF, FeatureSysLog, "create AaSysLog service daemon success");

    return 0;
}

/** 
 * Print AaSysLog service bip buffer memory information.
 * @param[in]   inArgName input argument description. 
 * @param[out]  outArgName
 * @retval      
 * @retval  
 * @par 
 *      
 * @par 
 *      
 * @par History
 *      2016-07-18 Ted: create function.
 */  
void AaSysLogBipBufferList(void)
{
    AaSysLogPrintF(LOGLEVEL_DBG, FeatureSysLog, "******************************");
    AaSysLogPrintF(LOGLEVEL_DBG, FeatureSysLog, "total syslog buffer size: %d", AASYSLOG_BIPBUFFER_SIZE);
    AaSysLogPrintF(LOGLEVEL_DBG, FeatureSysLog, "used syslog buffer size : %d", CBipBuffer_HowMuchData(_p_bip_buffer));
    AaSysLogPrintF(LOGLEVEL_DBG, FeatureSysLog, "maximum allocated syslog buffer size: %d", _aasyslog_max_bipbuf_used);
    AaSysLogPrintF(LOGLEVEL_DBG, FeatureSysLog, "******************************");
}

/** 
 * Register syslog print process function. 
 * @param[in]   void (*function)(char*, u32): the process function should processing
 *                                            these log data. 
 * @param[out]  outArgName
 * @retval      
 * @retval  
 * @par 
 *      
 * @par 
 *      
 * @par History
 *      2016-05-21 Ted: create function.
        2016-07-17 Ted: optimize code.
 */  
void AaSysLogProcessPrintRegister(void (*function)(char*, u32))
{
    _aasyslog_mng.processPrint_callback = function;
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
 *      2016-05-21 Ted: create function.
 */  
void AaSysLogPrintByPolling(char* str, u32 len)
{
    if (str == NULL || len == 0) {
        return;
    }

    GetBipAndSendByPolling(str, len);
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
 *      2016-07-17 Ted
 */  
void AaSysLogStoreBipBufferByPolling(char* str, u32 len)
{
    if (str == NULL || len == 0) {
        return;
    }

    if (len > (AASYSLOG_BIPBUFFER_SIZE - CBipBuffer_HowMuchData(_p_bip_buffer))) {
        return ;
    }

    char* bip_buf_addr = CBipBuffer_Reserve(_p_bip_buffer, len);
    if (bip_buf_addr == NULL)
    {
        return;
    }
    memcpy(bip_buf_addr, str, len);
    CBipBuffer_Commit(_p_bip_buffer, len);

    u32 size = CBipBuffer_HowMuchData(_p_bip_buffer);

    if (size > _aasyslog_max_bipbuf_used) {
        _aasyslog_max_bipbuf_used = size;
    }

    // u32 block_size;

    // char* block_addr = CBipBuffer_Get(_p_bip_buffer, &block_size);
    // GetBipAndSendByPolling(block_addr, block_size);
    // CBipBuffer_Decommit(_p_bip_buffer, block_size);
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
 *      2016-07-17 Ted
 */  
void AaSysLogStoreBipBufferNormal(char* str, u32 len)
{
    if (str == NULL || len == 0) {
        return;
    }

    if (len > (AASYSLOG_BIPBUFFER_SIZE - CBipBuffer_HowMuchData(_p_bip_buffer))) {
        return ;
    }

    osMutexWait(_aasyslog_mutex_id, osWaitForever);

    char* bip_buf_addr = CBipBuffer_Reserve(_p_bip_buffer, len);
    if (bip_buf_addr == NULL) {
        osMutexRelease(_aasyslog_mutex_id);
        return;
    }
    memcpy(bip_buf_addr, str, len);
    CBipBuffer_Commit(_p_bip_buffer, len);

    u32 size = CBipBuffer_HowMuchData(_p_bip_buffer);

    if (size > _aasyslog_max_bipbuf_used) {
        _aasyslog_max_bipbuf_used = size;
    }

    osMutexRelease(_aasyslog_mutex_id);
    osSignalSet(_aasyslogdaemon_id, SIG_BIT_TX);
}



// end of file
