
/***

History:
[2015-09-30 Ted]: Create.
[2016-04-21 Ted]: port code to FreeRTOS platform as stdio thread manage.
[2016-09-02 Ted]: optimize code.

*/

#ifndef _AATHREAD_H
#define _AATHREAD_H

#ifdef __cplusplus
 extern "C" {
#endif 


#include "cmsis_os.h"
#include "glo_def.h"



/** 
 * CCS Platform AaThread service Computer Environment initialization. 
 * AaThread service initialize after AaMemInit service.
 * @param[in]   inArgName input argument description. 
 * @param[out]  outArgName output argument description.  
 * @retval      return error code.
 *              0 is ok otherwise failed.
 * @retval  
 * @par 
 *      
 * @par 
 *      
 * @par History
 *      2016-05-21 Ted: create function
 */  
u8 AaThreadCEInit();


/** 
 * Create thread. 
 * This function should first been called after AaMemInit, and also after start scheduler.
 * @param[in]   t_def: thread definition referenced with \ref osThread. 
 * @param[in]   arg: pointer that is passed to the thread function as start argument.
 * @param[in]   timeout: timeout for waiting creation if it is blocked by other creation.
 *                       osWaitForever or specific time
 * @param[out]  
 * @retval      return thread ID for reference by other functions or NULL in case of error.
 * @par 
 *      
 * @par 
 *      
 * @par History
 *      2016-05-21 Ted: create function
 *      2016-09-02 Ted: fix recycling when create thread failed
 */  
osThreadId AaThreadCreateScheduler(const osThreadDef_t *t_def, void *arg, u32 timeout);


/** 
 * Create thread. 
 * This function should first been called after AaMemInit, and also before start scheduler.
 * @param[in]   t_def: thread definition referenced with \ref osThread. 
 * @param[in]   arg: pointer that is passed to the thread function as start argument.
 * @param[out]  
 * @retval      return thread ID for reference by other functions or NULL in case of error.
 * @par 
 *      
 * @par 
 *      
 * @par History
 *      2016-05-21 Ted: create function
 *      2016-09-02 Ted: fix recycling when create thread failed
 */  
osThreadId AaThreadCreateStartup(const osThreadDef_t *t_def, void *arg);


/** 
 * Kill thread. 
 * Should be called after scheduler. 
 * @param[in]   t_id: thread ID.
 * @param[in]   timeout: timeout for waiting killing if it is blocked by other killing.
 *                       osWaitForever or specific time
 * @param[out]  outArgName output argument description.  
 * @retval      return osOK if terminate success or other error code if failed.
 * @par 
 *      
 * @par 
 *      
 * @par History
 *      2016-05-22 Ted: create function
 *      2016-09-02 Ted: little modification
 */  
osStatus AaThreadKill(osThreadId t_id, u32 timeout);


/** 
 * Get the designated thread name. 
 * @param[in]   t_id: thread ID. 
 * @param[out]  outArgName output argument description.  
 * @retval      return the thread name pointer.
 * @retval  
 * @par 
 *      
 * @par 
 *      
 * @par History
 *      2016-05-22 Ted: create function
 */  
char* AaThreadGetName(osThreadId t_id);



#ifdef __cplusplus
}
#endif

#endif // _AATHREAD_H

// end of file
