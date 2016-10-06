
/***

History:
[2016-05-22 Ted]: Create

*/


#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>
#include "AaInclude.h"
#include "print_com.h"
#include "led.h"
#include "rtc_dev.h"
#include "gps.h"
#include "gsm.h"
#include "cp15.h"
#include "am2302.h"
#include "fan.h"
#include "osa_file.h"
#include "alpha_sense.h"
#include "dataRecord.h"
#include "dn7c3.h"


#define CCSDEAMON_STACK_SIZE        0x80


/** AaMem heap buffer for whole system */  
#define AAMEM_HEAP_BUFFER_SIZE  (1024*10)

char _mem_heap_buf[AAMEM_HEAP_BUFFER_SIZE];


/** Description of the macro */  
osThreadId _ccsdaemon_id;



static u8 CCSDaemonCreateThread();
static void CCSDaemonThread(void const *arg);



/** 
 * System Compute environment initialize
 * initialize the sub service which won't create thread
 * any sub service daemon which need create thread should be initialzed in CCSDaemon thread
 * @param[in]   inArgName input argument description. 
 * @param[out]  outArgName output argument description.  
 * @retval  
 * @retval  
 * @par 
 *      
 * @par 
 *      
 * @par History
 *      2016-5-21 Huang Shengda
 */  
u8 CCSDeamonCEInit()
{
    // WARNING: don't change the order

    // @first because all service should depand on log
    StdUsartInit();
    RtcInit();
    
    AaMemHeapCEInit(_mem_heap_buf, &_mem_heap_buf[AAMEM_HEAP_BUFFER_SIZE - 1]);

    // bip buffer has not construct because of memery heap do not setup,
    // so aasyslog should use stdio print interface
    AaSysLogProcessPrintRegister(AaSysLogPrintByPolling);

    // platform initialize
    AaThreadCEInit();
    AaSysLogCEInit();

    // after heap momery and bip buffer initialized, log can be input bip buffer
    AaSysLogProcessPrintRegister(AaSysLogStoreBipBufferByPolling);

    // alternative CCS service initialization
    AaSysComCEInit();
    //AaTagCEInit();
    //AaShellCEInit();

    //CCSDaemonCreateThread();     // for test
    AaSysLogCreateDeamon();
    //AaTagCreateDeamon();         // no need
    ////StartTFCardTask();           // This task for test
    // start application task
    StartDataRecordTask();
    StartRunLedTask();
    StartGpsTask();
    StartGsmTask();
    StartCP15Task();
    StartAlphaSenseTask();
    
    StartDn7c3Task();

    /* Init Fan */
    FanStart();
    
    //StartRunAm2302Task();

    // create global tag
    //AaTagCreate(AATAG_CCS_DAEMON_ONLINE, 0);
    //AaTagCreate(AATAG_CCS_STARTUP, 0);

    //AaSysLogPrintF(LOGLEVEL_INF, SystemStartup, "System started");

    // as scheduler started, print can be print into bipbuffer and send out by DMA
    AaSysLogGetBipRegister(GetBipAndSendByDMA);
    AaSysLogProcessPrintRegister(AaSysLogStoreBipBufferNormal);
    
    return 0;
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
 *      2016-5-22 Huang Shengda
 */  
static u8 CCSDaemonCreateThread()
{
    osThreadDef(CCSDaemon, CCSDaemonThread, osPriorityNormal, 0, CCSDEAMON_STACK_SIZE);
    
    _ccsdaemon_id = AaThreadCreateStartup(osThread(CCSDaemon), NULL);
    if(_ccsdaemon_id == NULL) {
        AaSysLogPrintF(LOGLEVEL_ERR, FeatureCCS, "%s %d: CCS Daemon initialize failed",
                __FUNCTION__, __LINE__);
        return 1;
    }
    AaSysLogPrintF(LOGLEVEL_DBG, FeatureCCS, "create CCS daemon success");
    
    return 0;
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
 *      2016-5-22 Huang Shengda
 */  
static void CCSDaemonThread(void const *arg)
{
    (void) arg;

    AaSysLogPrintF(LOGLEVEL_INF, FeatureCCS, "CCSDaemonThread started");
    AaTagSetValue(AATAG_CCS_DAEMON_ONLINE, 1);

    // initialize ccs service which need in CCSDeamon thread
    

    // start ccs service thread


    for(;;) {
        osDelay(5000);
        AaMemList();
        AaSysLogBipBufferList();
    }
}



// end of file
