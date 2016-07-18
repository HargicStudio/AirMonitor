
/***

History:
[2016-05-22 Ted]: Create

*/


#include <string.h>
#include "AaInclude.h"




#define AATAG_NAME_MAX_LENGTH   64

#define AATAG_CALLBACK_MAX_NUMBER   8

#define AATAG_MSGBOX_MAX_DEEP       8

#define AATAGDEAMON_STACK_SIZE      0x80


typedef struct SAaTag_t {

    struct SAaTag_t* next;
    struct SAaTag_t* prev;

    char    name[AATAG_NAME_MAX_LENGTH];

    u32     value;

    void    (*notify_func_callback[AATAG_CALLBACK_MAX_NUMBER])(u32 value);
        
} SAaTag;



SAaTag* _aatag_mng = NULL;


/** mutex for AaSysLogPrintF */ 
static osMutexDef(aatag_mutex);
osMutexId _aatag_mutex_id;


static osMessageQDef(aatag_msgbox, AATAG_MSGBOX_MAX_DEEP, u32);
osMessageQId _aatag_msgbox_id;



/** Daemon thread for AaTag */  
osThreadId _aatagdaemon_id;



static void AaTagDeamonThread(void const *arg);
static SAaTag* AaTagFindMngPtr(char* name);



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
static void AaTagDeamonThread(void const *arg)
{
    SAaTag* _this = (SAaTag*) arg;
    // avoid compiling warning
    _this = _this;
    
    SAaTag* get_ptr;
    osEvent evt;
    u8 i;

    AaSysLogPrintF(LOGLEVEL_DBG, FeatureTag, "AaTagDeamonThread started");


    for(;;) {
        
        evt = osMessageGet(_aatag_msgbox_id, osWaitForever);
        if(evt.status == osEventMessage) {
            
            get_ptr = evt.value.p;

//            AaSysLogPrintF(LOGLEVEL_DBG, FeatureTag, "get one msg %p", get_ptr);

            for(i=0; i<AATAG_CALLBACK_MAX_NUMBER; i++) {
                if(get_ptr->notify_func_callback[i] == NULL) {
                    continue;
                }
                get_ptr->notify_func_callback[i](get_ptr->value);
                
//                AaSysLogPrintF(LOGLEVEL_DBG, FeatureTag, "callback called %p", get_ptr->notify_func_callback[i]);
            }
        }
        
    }
}



u8 AaTagCEInit()
{
    _aatag_mutex_id = osMutexCreate(osMutex(aatag_mutex));
    if(_aatag_mutex_id == NULL) {
        AaSysLogPrintF(LOGLEVEL_ERR, FeatureTag, "%s %d: aatag_mutex initialize failed",
                __FUNCTION__, __LINE__);
        return 1;
    }
    AaSysLogPrintF(LOGLEVEL_DBG, FeatureTag, "create aatag_mutex success");


    _aatag_msgbox_id = osMessageCreate(osMessageQ(aatag_msgbox), NULL);
    if(_aatag_msgbox_id == NULL) {
        AaSysLogPrintF(LOGLEVEL_ERR, FeatureTag, "%s %d: aatag_msgbox initialize failed",
                __FUNCTION__, __LINE__);
        return 1;
    }
    AaSysLogPrintF(LOGLEVEL_DBG, FeatureTag, "create aatag_msgbox success");


    AaSysLogPrintF(LOGLEVEL_DBG, FeatureTag, "AaTag initialize success");

    return 0;
}



u8 AaTagCreateDeamon()
{
    osThreadDef(AaTagDaemon, AaTagDeamonThread, osPriorityHigh, 0, AATAGDEAMON_STACK_SIZE);
    
    _aatagdaemon_id = AaThreadCreateStartup(osThread(AaTagDaemon), _aatag_mng);
    if(_aatagdaemon_id == NULL) {
        AaSysLogPrintF(LOGLEVEL_ERR, FeatureTag, "%s %d: AaTagDeamon initialize failed",
                __FUNCTION__, __LINE__);
        return 1;
    }
    AaSysLogPrintF(LOGLEVEL_DBG, FeatureTag, "create AaTagDeamon success");

    return 0;
}



u8 AaTagCreate(char* name, u32 value)
{
    osMutexWait(_aatag_mutex_id, osWaitForever);
    
    SAaTag* new_ptr = AaMemMalloc(sizeof(SAaTag));
    if(new_ptr == NULL) {
        AaSysLogPrintF(LOGLEVEL_WRN, FeatureTag, "create tag failed");
        return 1;
    }

    // initialize
    strcpy(new_ptr->name, name);
    new_ptr->value = value;
    new_ptr->next = NULL;
    new_ptr->prev = NULL;
    for(u8 i=0; i<AATAG_CALLBACK_MAX_NUMBER; i++) {
        new_ptr->notify_func_callback[i] = NULL;
    }

    SAaTag* cur_ptr = _aatag_mng;
    if(cur_ptr == NULL) {
        _aatag_mng = new_ptr;
    }
    else {
        while(cur_ptr->next != NULL) {
            cur_ptr = cur_ptr->next;
        }

        cur_ptr->next = new_ptr;
        new_ptr->prev = cur_ptr;
    }

    osMutexRelease(_aatag_mutex_id);

    AaSysLogPrintF(LOGLEVEL_DBG, FeatureTag, "create tag %s success", cur_ptr->name);

    return 0;
}



u8 AaTagDelete(char* name)
{
    osMutexWait(_aatag_mutex_id, osWaitForever);

    SAaTag* cur_ptr = AaTagFindMngPtr(name);
    if(cur_ptr == NULL) {
        AaSysLogPrintF(LOGLEVEL_WRN, FeatureTag, "delete tag failed, don't find");
        return 1;
    }
    
    SAaTag* prv_ptr = cur_ptr->prev;
    SAaTag* nxt_ptr = cur_ptr->next;

    prv_ptr->next = nxt_ptr;
    nxt_ptr->prev = prv_ptr;

    AaSysLogPrintF(LOGLEVEL_DBG, FeatureTag, "delete tag %s", cur_ptr->name);

    AaMemFree(cur_ptr);

    osMutexRelease(_aatag_mutex_id);

    return 0;
}


u8 AaTagRegister(char* name, void(*function)(u32))
{
    SAaTag* cur_ptr = AaTagFindMngPtr(name);
    if(cur_ptr == NULL) {
        AaSysLogPrintF(LOGLEVEL_WRN, FeatureTag, "register tag failed, don't find");
        return 1;
    }

    for(u8 i=0; i<AATAG_CALLBACK_MAX_NUMBER; i++) {
        if(cur_ptr->notify_func_callback[i] == NULL) {
            
            cur_ptr->notify_func_callback[i] = function;
            AaSysLogPrintF(LOGLEVEL_DBG, FeatureTag, "register tag success");
            
            return 0;;
        }
    }

    AaSysLogPrintF(LOGLEVEL_WRN, FeatureTag, "register tag failed, callback full");

    return 2;
}


u8 AaTagSetValue(char* name, u32 value)
{
    osMutexWait(_aatag_mutex_id, osWaitForever);

    SAaTag* cur_ptr = AaTagFindMngPtr(name);
    if(cur_ptr == NULL) {
        return 1;
    }

    cur_ptr->value = value;

    osMutexRelease(_aatag_mutex_id);

    // trigger notification
    osMessagePut(_aatag_msgbox_id, (u32)cur_ptr, osWaitForever);

    AaSysLogPrintF(LOGLEVEL_DBG, FeatureTag, "set tag %s with %d", cur_ptr->name, cur_ptr->value);

    return 0;
}

static SAaTag* AaTagFindMngPtr(char* name)
{
    SAaTag* cur_ptr = _aatag_mng;

    if(cur_ptr == NULL) {
        return NULL;
    }

    do {
        if(strcmp(cur_ptr->name, name) == 0) {
            break;
        }
        else {
            cur_ptr = cur_ptr->next;
        }
    } while(cur_ptr != NULL);

    if(cur_ptr == NULL) {
        return NULL;
    }

    return cur_ptr;
}



// end of file
