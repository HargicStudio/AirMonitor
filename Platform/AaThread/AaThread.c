
/***

History:
[2015-09-30 Ted]: Create
[2016-04-21 Ted]: merge code to FreeRTOS platform as stdio usart
[2016-08-26 Ted]: optimize the code
[2016-09-02 Ted]: optimize the code

*/


#include "AaThread.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>
#include "AaInclude.h"


#define AATHREAD_MAX_NAME_LENGTH    configMAX_TASK_NAME_LEN



typedef struct SAaThread_t
{
    /*!
     *  @brief  
     */
    struct SAaThread_t* next;
    struct SAaThread_t* prev;

    /*!
     *  @brief  
     */
    char                name[AATHREAD_MAX_NAME_LENGTH];        ///< Thread name 
    os_pthread          pthread;      ///< start address of thread function
    osPriority          tpriority;    ///< initial thread priority
    u32                 instances;    ///< maximum number of instances of that thread function
    u32                 stacksize;    ///< stack size requirements in bytes; 0 is default stack size

    /*!
     *  @brief  
     */
    osThreadId          t_id;
    
} SAaThread;



/** mutex for AaThread */  
SAaThread* _aathread_mng_header_ptr = NULL;


/** mutex for AaThread */  
osMutexId _aathread_mutex_id;



static SAaThread* AaThreadFindMngPtr(osThreadId t_id);



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
u8 AaThreadCEInit()
{
    osMutexDef(aathread_mutex);
    _aathread_mutex_id = osMutexCreate(osMutex(aathread_mutex));
    if(_aathread_mutex_id == NULL) 
    {
        AaSysLogPrintF(LOGLEVEL_ERR, FeatureAaThread, "%s %d: AaThread mutex initialize failed",
                __FUNCTION__, __LINE__);
        return 1;
    }
    AaSysLogPrintF(LOGLEVEL_INF, FeatureAaThread, "create aathread_mutex success");

    return 0;
}

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
osThreadId AaThreadCreateScheduler(const osThreadDef_t *t_def, void *arg, u32 timeout)
{
    osMutexWait(_aathread_mutex_id, timeout);

    SAaThread* new_ptr;
    SAaThread* cur_ptr;

    osThreadId t_id = osThreadCreate(t_def, arg);
    if(t_id == NULL) 
    {
        osMutexRelease(_aathread_mutex_id);

        AaSysLogPrintF( LOGLEVEL_ERR, FeatureAaThread, "%s %d: create thread failed, Reason: system creation call failed",
                        __FUNCTION__, __LINE__);
        return NULL;
    }
    
    cur_ptr = _aathread_mng_header_ptr;
    
    if(cur_ptr == NULL)     // this is the first thread
    {    
        new_ptr = AaMemCalloc(1, sizeof(SAaThread));
        if(new_ptr == NULL)
        {
            // recycling thread resources
            osStatus err = osThreadTerminate(t_id);
            if(osOK != err)
            {
                AaSysLogPrintF( LOGLEVEL_ERR, FeatureAaThread, "%s %d: terminate thread failed with errcode %d, Reason: system terminate call failed",
                                __FUNCTION__, __LINE__, err);
            }
            
            osMutexRelease(_aathread_mutex_id);

            AaSysLogPrintF( LOGLEVEL_ERR, FeatureAaThread, "%s %d: create thread failed, Reason: calloc memory failed",
                            __FUNCTION__, __LINE__);
            return NULL;
        }

        // get the thread name
        if (strlen(t_def->name) < AATHREAD_MAX_NAME_LENGTH)
        {
            strncpy(new_ptr->name, t_def->name, strlen(t_def->name));
        }
        else
        {
            strncpy(new_ptr->name, t_def->name, AATHREAD_MAX_NAME_LENGTH);
        }
        
        new_ptr->pthread = t_def->pthread;
        new_ptr->tpriority = t_def->tpriority;
        new_ptr->instances = t_def->instances;
        new_ptr->stacksize = t_def->stacksize;
        new_ptr->t_id = t_id;
        new_ptr->next = NULL;
        new_ptr->prev = NULL;

        _aathread_mng_header_ptr = new_ptr;
    }
    else 
    {    
        while(cur_ptr->next != NULL)
        {
            cur_ptr = cur_ptr->next;
        }

        // get the last mng_ptr
        new_ptr = AaMemCalloc(1, sizeof(SAaThread));
        if(new_ptr == NULL)
        {
            // recycling thread resources
            osStatus err = osThreadTerminate(t_id);
            if(osOK != err)
            {
                AaSysLogPrintF( LOGLEVEL_ERR, FeatureAaThread, "%s %d: terminate thread failed with errcode %d, Reason: system terminate call failed",
                                __FUNCTION__, __LINE__, err);
            }
            
            osMutexRelease(_aathread_mutex_id);

            AaSysLogPrintF( LOGLEVEL_ERR, FeatureAaThread, "%s %d: create thread failed, Reason: calloc memory failed",
                            __FUNCTION__, __LINE__);
            
            return NULL;
        }

        // get the thread name
        if (strlen(t_def->name) < AATHREAD_MAX_NAME_LENGTH)
        {
            strncpy(new_ptr->name, t_def->name, strlen(t_def->name));
        }
        else
        {
            strncpy(new_ptr->name, t_def->name, AATHREAD_MAX_NAME_LENGTH);
        }
        
        new_ptr->pthread = t_def->pthread;
        new_ptr->tpriority = t_def->tpriority;
        new_ptr->instances = t_def->instances;
        new_ptr->stacksize = t_def->stacksize;
        new_ptr->t_id = t_id;
        new_ptr->next = NULL;
        new_ptr->prev = cur_ptr;

        cur_ptr->next = new_ptr;
    }

    osMutexRelease(_aathread_mutex_id);

    AaSysLogPrintF( LOGLEVEL_DBG, FeatureAaThread, "%s %d: create thread %s success with tpriority %d instances %d stacksize %d",
                    __FUNCTION__, __LINE__, new_ptr->name, new_ptr->tpriority, new_ptr->instances, new_ptr->stacksize);

    return t_id;
}

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
osThreadId AaThreadCreateStartup(const osThreadDef_t *t_def, void *arg)
{
    SAaThread* new_ptr;
    SAaThread* cur_ptr;

    osThreadId t_id = osThreadCreate(t_def, arg);
    if(t_id == NULL)
    {
        return NULL;
    }
    
    cur_ptr = _aathread_mng_header_ptr;
    
    if(cur_ptr == NULL)     // this is the first thread
    {
        new_ptr = AaMemCalloc(1, sizeof(SAaThread));
        if(new_ptr == NULL)
        {
            // recycling thread resources
            osThreadTerminate(t_id);

            return NULL;
        }
        
        // get the thread name
        if (strlen(t_def->name) < AATHREAD_MAX_NAME_LENGTH)
        {
            strncpy(new_ptr->name, t_def->name, strlen(t_def->name));
        }
        else
        {
            strncpy(new_ptr->name, t_def->name, AATHREAD_MAX_NAME_LENGTH);
        }
        
        new_ptr->pthread = t_def->pthread;
        new_ptr->tpriority = t_def->tpriority;
        new_ptr->instances = t_def->instances;
        new_ptr->stacksize = t_def->stacksize;
        new_ptr->t_id = t_id;
        new_ptr->next = NULL;
        new_ptr->prev = NULL;

        _aathread_mng_header_ptr = new_ptr;
    }
    else
    {
        while(cur_ptr->next != NULL) 
        {
            cur_ptr = cur_ptr->next;
        }

        // get the last mng_ptr
        new_ptr = AaMemCalloc(1, sizeof(SAaThread));
        if(new_ptr == NULL)
        {
            // recycling thread resources
            osThreadTerminate(t_id);
            
            return NULL;
        }

        // get the thread name
        if (strlen(t_def->name) < AATHREAD_MAX_NAME_LENGTH)
        {
            strncpy(new_ptr->name, t_def->name, strlen(t_def->name));
        }
        else
        {
            strncpy(new_ptr->name, t_def->name, AATHREAD_MAX_NAME_LENGTH);
        }
        
        new_ptr->pthread = t_def->pthread;
        new_ptr->tpriority = t_def->tpriority;
        new_ptr->instances = t_def->instances;
        new_ptr->stacksize = t_def->stacksize;
        new_ptr->t_id = t_id;
        new_ptr->next = NULL;
        new_ptr->prev = cur_ptr;

        cur_ptr->next = new_ptr;
    }

    return t_id;
}

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
osStatus AaThreadKill(osThreadId t_id, u32 timeout)
{
    osMutexWait(_aathread_mutex_id, timeout);
    
    osStatus err = osThreadTerminate(t_id);
    if(err != osOK )
    {
        AaSysLogPrintF( LOGLEVEL_ERR, FeatureAaThread, "%s %d: terminate thread failed with errcode %d, Reason: system terminate call failed",
                        __FUNCTION__, __LINE__, err);
    }

    SAaThread* cur_ptr = AaThreadFindMngPtr(t_id);
    SAaThread* pre_ptr = cur_ptr->prev;
    SAaThread* nxt_ptr = cur_ptr->next;

    pre_ptr->next = nxt_ptr;
    nxt_ptr->prev = pre_ptr;

    AaSysLogPrintF( LOGLEVEL_DBG, FeatureAaThread, "%s %d: thread %s will be killed",
                    __FUNCTION__, __LINE__, cur_ptr->name);
    
    AaMemFree(cur_ptr);

    osMutexRelease(_aathread_mutex_id);

    return err;
}


static SAaThread* AaThreadFindMngPtr(osThreadId t_id)
{
    SAaThread* cur_ptr = _aathread_mng_header_ptr;
    
    if(cur_ptr == NULL) {    // thread manage is empty
        return NULL;
    }
    else {
        do {
            if(cur_ptr->t_id == t_id) {
                break;
            }
            else {
                cur_ptr = cur_ptr->next;
            }
        } while(cur_ptr != NULL);

        if(cur_ptr == NULL) {   // no this t_id
            return NULL;
        }
        
        return cur_ptr;
    }
}

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
char* AaThreadGetName(osThreadId t_id)
{
    SAaThread* cur_ptr = AaThreadFindMngPtr(t_id);
    if(cur_ptr == NULL)
    {
        return NULL;
    }
    
    return cur_ptr->name;
}



// end of file
