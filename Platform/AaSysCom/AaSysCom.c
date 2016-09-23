


/***

History:
2016-04-23 Ted: Create
2016-09-03 Ted: port to mbed
2016-09-22 Ted: support service log control

*/

#include "glo_def.h"
#include "AaSysCom.h"
#include "AaMem.h"
#include <stdbool.h>
#include <string.h>



#define AASYSCOM_MAGIC_WORD     0xDEADC0DE



#define AASYSCOM_NAME_LENGTH_MAX        configMAX_TASK_NAME_LEN

typedef struct SSysComMng_t
{
    struct SSysComMng_t* next;
    struct SSysComMng_t* prev;
    
    osMessageQId    q_id;
    u32             q_sz;
    ESysComID       syscom_id;
    char            name[AASYSCOM_NAME_LENGTH_MAX];
} SSysComMng;

SSysComMng *_syscom_mng = NULL;


osMutexId _aasyscom_mutex_id;


bool        _syscom_log_enable_flag = true;
ELogLevel   _syscom_log_level       = LOGLEVEL_WRN;


#define Log(level, content, ...)    do { \
                                        if (_syscom_log_enable_flag) \
                                            if (level >= _syscom_log_level) \
                                                AaSysLogPrintF(level, FeatureSysCom, content, ##__VA_ARGS__); \
                                    } while(0)



static ESysComID AaSysComGetSysComIDAuto();
static char* AaSysComGetName(ESysComID sc_id);
static osMessageQId AaSysComGetQueueID(ESysComID sc_id);
static SSysComMng* AaSysComGetControlBlock(ESysComID sc_id);
static bool AaSysComIsRegistered(ESysComID sc_id);
static void AaSysComAdd2ControlBlock(SSysComMng* new);
static void AaSysComRemove4ControlBlock(SSysComMng* rm);



u8 AaSysComCEInit()
{
    osMutexDef(SysComMutex);
    _aasyscom_mutex_id = osMutexCreate(osMutex(SysComMutex));
    if(_aasyscom_mutex_id == NULL)
    {
        Log(LOGLEVEL_ERR, "%s: AaSysCom CE initialize failed, Reason: SysComMutex initialize failed", __FUNCTION__);
        return 1;
    }
    Log(LOGLEVEL_INF, "%s: create SysComMutex success", __FUNCTION__);

    Log(LOGLEVEL_INF, "AaSysCom service CE initialize success");

    return 0;
}

static ESysComID AaSysComGetSysComIDAuto()
{
    static ESysComID _syscom_id = SysCom_Auto;
    
    if(_syscom_id >= SysCom_Auto && _syscom_id < SysCom_MAX) {
        return _syscom_id++;
    }
    else 
    {   
        return SysCom_Unknow;
    }
}

ESysComID AaSysComRegister(ESysComID syscom, char* name, u32 q_size)
{
    SSysComMng* new_ptr = AaMemCalloc(1, sizeof(SSysComMng));
    if(new_ptr == NULL)
    {
        Log(LOGLEVEL_ERR, "%s: SysCom %s register failed, Reason: control block calloc failed", 
            __FUNCTION__, name);
        return SysCom_Unknow;
    }

    osMutexWait(_aasyscom_mutex_id, osWaitForever);
        
    if(syscom == SysCom_Unknow)
    {
        new_ptr->syscom_id = AaSysComGetSysComIDAuto();
        if(new_ptr->syscom_id == SysCom_Unknow)
        {
            AaMemFree(new_ptr);
            Log(LOGLEVEL_ERR, "%s: SysCom %s initialize failed, Reason: get random syscomID failed", 
                __FUNCTION__, name);

            osMutexRelease(_aasyscom_mutex_id);
            
            return SysCom_Unknow;
        }
        Log(LOGLEVEL_DBG, "%s: get random syscomID %d", __FUNCTION__, new_ptr->syscom_id);
    }
    else
    {
        new_ptr->syscom_id = syscom;
    }
    
    osMessageQDef(msg_q, q_size, u32);
    new_ptr->q_id = osMessageCreate(osMessageQ(msg_q), NULL);
    if(new_ptr->q_id == NULL)
    {
        AaMemFree(new_ptr);
        Log(LOGLEVEL_ERR, "%s: SysCom %s initialize failed, Reason: osMessageCreate failed",
            __FUNCTION__, name);

        osMutexRelease(_aasyscom_mutex_id);
        
        return SysCom_Unknow;
    }

    if(strlen(name) > AASYSCOM_NAME_LENGTH_MAX) 
    {
        strncpy(new_ptr->name, name, AASYSCOM_NAME_LENGTH_MAX);
    }
    else 
    {
        strncpy(new_ptr->name, name, strlen(name));
    }
    new_ptr->name[AASYSCOM_NAME_LENGTH_MAX-1] = '\0';
    
    new_ptr->q_sz = q_size;

    AaSysComAdd2ControlBlock(new_ptr);

    osMutexRelease(_aasyscom_mutex_id);

    Log(LOGLEVEL_INF, "%s: SysCom %s register success", __FUNCTION__, new_ptr->name);

    return new_ptr->syscom_id;
}

/** 
 * Recycling syscom control block. 
 * There is defective that even though control block has been recycling, 
 *  but RTOS MessageQueue resource can't be recycling because there is not 
 *  recycling API in mbed RTOS. 
 * @param[in]   inArgName input argument description. 
 * @param[out]  outArgName output argument description.  
 * @retval  
 * @retval  
 * @par 
 *      
 * @par 
 *      
 * @par History
 *      2016-09-04 Huang Shengda
 */  
ESysComStatus AaSysComUnregister(ESysComID syscom)
{
    if(!AaSysComIsRegistered(syscom))
    {
        Log(LOGLEVEL_ERR, "%s: failed, Reason: syscom %d not registered", __FUNCTION__, syscom);

        return SysCom_ErrParam;
    }

    SSysComMng* ptr = AaSysComGetControlBlock(syscom);
    if(ptr == NULL)
    {
        Log(LOGLEVEL_ERR, "%s: failed, Reason: get control block failed", __FUNCTION__);

        return SysCom_ErrParam;
    }

    osMutexWait(_aasyscom_mutex_id, osWaitForever);

    AaSysComRemove4ControlBlock(ptr);

    osMutexRelease(_aasyscom_mutex_id);

    Log(LOGLEVEL_INF, "%d: SysCom %s unregister success", __FUNCTION__, ptr->name);

    AaMemFree(ptr);

    return SysCom_Ok;
}


void* AaSysComCreate(SAaSysComMsgId msgid, ESysComID sender, ESysComID receiver, u16 pl_size)
{
    if(!AaSysComIsRegistered(sender) || !AaSysComIsRegistered(receiver))
    {
        Log(LOGLEVEL_WRN, "%s: msg 0x%04x create failed, Reason: no sender %d or receiver %d",
            __FUNCTION__, msgid, sender, receiver);
        return NULL;
    }

    if(msgid >= API_MESSAGE_ID_MAX)
    {
        Log(LOGLEVEL_WRN, "%s: msg 0x%04x error", __FUNCTION__, msgid);
        return NULL;
    }

    SMsgHeader* msg_ptr = AaMemCalloc(1, (sizeof(SMsgHeader) + pl_size));
    if(msg_ptr == NULL)
    {
        Log(LOGLEVEL_WRN, "%s: msg 0x%04x create failed, Reason: calloc failed", __FUNCTION__, msgid);
        return NULL;
    }

    msg_ptr->magic  = AASYSCOM_MAGIC_WORD;
    msg_ptr->msg_id = msgid;
    msg_ptr->target = receiver;
    msg_ptr->sender = sender;
    msg_ptr->pl_size = pl_size;

    Log(LOGLEVEL_DBG, "%s: create msg 0x%04x success", __FUNCTION__, msgid);

    return (void*)msg_ptr;
}


void AaSysComDestory(void* msg_ptr)
{
    AaMemFree(msg_ptr);
}

void* AaSysComGetPayload(void* msg_ptr)
{
    if(msg_ptr == NULL)
    {
        Log(LOGLEVEL_WRN, "%s: msg ptr NULL", __FUNCTION__);
        return NULL;
    }
    
    return ((char*)msg_ptr + sizeof(SMsgHeader));
}

SAaSysComMsgId AaSysComGetMsgID(void* msg_ptr)
{
    if(msg_ptr == NULL)
    {
        Log(LOGLEVEL_WRN, "%s: msg ptr NULL", __FUNCTION__);
        return NULL;
    }

    SMsgHeader* msg = (SMsgHeader*)msg_ptr;
    return msg->msg_id;
}

ESysComStatus AaSysComSend(void* msg_ptr, u32 timeout)
{
    if(msg_ptr == NULL)
    {
        Log(LOGLEVEL_WRN, "%s: msg ptr NULL", __FUNCTION__);
        return SysCom_ErrParam;
    }
    
    SMsgHeader* header = (SMsgHeader*)msg_ptr;

    if(header->msg_id >= API_MESSAGE_ID_MAX)
    {
        Log(LOGLEVEL_WRN, "%s: msg 0x%04x error", __FUNCTION__, header->msg_id);
        return SysCom_ErrParam;
    }

    if(!AaSysComIsRegistered(header->sender) || !AaSysComIsRegistered(header->target))
    {
        Log(LOGLEVEL_WRN, "%s: msg 0x%04x create failed, Reason: no sender %d or receiver %d",
            __FUNCTION__, header->msg_id, header->sender, header->target);
        return SysCom_ErrParam;
    }

    osMessageQId q_id = AaSysComGetQueueID(header->target);
    if(q_id == NULL)
    {
        Log(LOGLEVEL_WRN, "%s: do not get queue ID", __FUNCTION__, header->msg_id);
        return SysCom_ErrParam;
    }
    
    if(osOK != osMessagePut(q_id, (u32)msg_ptr, timeout))
    {
        Log(LOGLEVEL_ERR, "%s: msg 0x%04x send failed, Reason: osMessagePut failed",
            __FUNCTION__, header->msg_id);
        AaSysComDestory(msg_ptr);
        return SysCom_ErrRTOS;
    }

    Log(LOGLEVEL_DBG, "%s: msg 0x%04x send success from %s to %s", 
        __FUNCTION__, header->msg_id, AaSysComGetName(header->sender), AaSysComGetName(header->target));

    return SysCom_Ok;
}

ESysComID AaSysComGetSender(void* msg_ptr)
{
    if(msg_ptr == NULL)
    {
        Log(LOGLEVEL_WRN, "%s: msg ptr NULL", __FUNCTION__);
        return SysCom_Unknow;
    }
    
    SMsgHeader* msg = (SMsgHeader*)msg_ptr;
    return msg->sender;
}

ESysComStatus AaSysComSetSender(void* msg_ptr, ESysComID sender)
{
    if(msg_ptr == NULL)
    {
        Log(LOGLEVEL_WRN, "%s: msg ptr NULL", __FUNCTION__);
        return SysCom_ErrParam;
    }

    if(!AaSysComIsRegistered(sender))
    {
        Log(LOGLEVEL_WRN, "%s: failed, Reason: no sender %d", __FUNCTION__, sender);
        return SysCom_ErrParam;
    }

    SMsgHeader* msg = (SMsgHeader*)msg_ptr;
    msg->sender = sender;

    return SysCom_Ok;
}

ESysComID AaSysComGetReceiver(void* msg_ptr)
{
    if(msg_ptr == NULL)
    {
        Log(LOGLEVEL_WRN, "%s: msg ptr NULL", __FUNCTION__);
        return SysCom_Unknow;
    }
    
    SMsgHeader* msg = (SMsgHeader*)msg_ptr;
    return msg->target;
}

ESysComStatus AaSysComSetReceiver(void* msg_ptr, ESysComID receiver)
{
    if(msg_ptr == NULL)
    {
        Log(LOGLEVEL_WRN, "%s: msg ptr NULL", __FUNCTION__);
        return SysCom_ErrParam;
    }

    if(!AaSysComIsRegistered(receiver))
    {
        Log(LOGLEVEL_WRN, "%s: failed, Reason: no target %d", __FUNCTION__, receiver);
        return SysCom_ErrParam;
    }

    SMsgHeader* msg = (SMsgHeader*)msg_ptr;
    msg->target = receiver;

    return SysCom_Ok;
}


ESysComStatus AaSysComForward(void* msg_ptr, ESysComID sender, ESysComID receiver, u32 timeout)
{ 
    if(msg_ptr == NULL)
    {
        Log(LOGLEVEL_WRN, "%s: msg ptr NULL", __FUNCTION__);
        return SysCom_ErrParam;
    }
    
    if(!AaSysComIsRegistered(sender) || !AaSysComIsRegistered(receiver))
    {
        Log(LOGLEVEL_WRN, "%s: forward failed, Reason: no sender %d or receiver %d",
            __FUNCTION__, sender, receiver);
        return SysCom_ErrParam;
    }

    SMsgHeader* msg = (SMsgHeader*)msg_ptr;

    void* msg_fw = AaMemCalloc(1, (sizeof(SMsgHeader) + msg->pl_size));
    if(msg_fw == NULL)
    {
        Log(LOGLEVEL_WRN, "%s: msg 0x%04x forward failed, Reason: calloc failed", 
            __FUNCTION__, msg->msg_id);
        return SysCom_ErrMem;
    }

    memcpy(msg_fw, msg_ptr, (sizeof(SMsgHeader) + msg->pl_size));
    if(SysCom_Ok != AaSysComSetSender(msg_fw, sender)) 
    {
        Log(LOGLEVEL_WRN, "%s: msg 0x%04x forward failed, Reason: set sender %s failed",
            __FUNCTION__, msg->msg_id, AaSysComGetName(sender));
        return SysCom_ErrParam;
    }
    if(SysCom_Ok != AaSysComSetReceiver(msg_fw, receiver)) 
    {
        Log(LOGLEVEL_WRN, "%s: msg 0x%04x forward failed, Reason: set receiver %s failed",
            __FUNCTION__, msg->msg_id, AaSysComGetName(receiver));
        return SysCom_ErrParam;
    }

    if(SysCom_Ok != AaSysComSend(msg_fw, timeout))
    {
        Log(LOGLEVEL_WRN, "%s: msg 0x%04x forward failed, Reason: AaSysComSend failed",
            __FUNCTION__, msg->msg_id);
        return SysCom_ErrProc;
    }

    SMsgHeader* header = (SMsgHeader*)msg_fw;
    Log(LOGLEVEL_DBG, "%s: msg 0x%04x forward success from %s to %s", 
        __FUNCTION__, header->msg_id, AaSysComGetName(header->sender), AaSysComGetName(header->target));

    return SysCom_Ok;
}

void* AaSysComReceiveHandler(ESysComID receiver, u32 timeout)
{
    osEvent  evt;

    if(!AaSysComIsRegistered(receiver))
    {
        Log(LOGLEVEL_WRN, "%s: failed, Reason: receiver %d is not registered",
            __FUNCTION__, receiver);
        return NULL;
    }

    osMessageQId q_id = AaSysComGetQueueID(receiver);
    if(q_id == NULL)
    {
        Log(LOGLEVEL_WRN, "%s: do not get receiver %d queue ID", __FUNCTION__, receiver);
        return NULL;
    }
    
    evt = osMessageGet(q_id, timeout);
    if (evt.status == osEventMessage) 
    {
        void* msg = evt.value.p;

        SMsgHeader* header = (SMsgHeader*)msg;

        if(header->magic != AASYSCOM_MAGIC_WORD)
        {
            Log(LOGLEVEL_WRN, "%s: failed, Reason: get error MAGIC 0x%x", __FUNCTION__, header->magic);
            AaSysComDestory(msg);
            
            return NULL;
        }

        Log(LOGLEVEL_DBG, "%s: receive msg 0x%04x success from %s to %s", 
            __FUNCTION__, header->msg_id, AaSysComGetName(header->sender), AaSysComGetName(header->target));

        return msg;
    }

    return NULL;
}

static char* AaSysComGetName(ESysComID sc_id)
{
    SSysComMng* ptr = _syscom_mng;

    if(ptr == NULL)
    {
        return NULL;
    }

    while(ptr->syscom_id != sc_id)
    {
        if(ptr->next != NULL)
        {
            ptr = ptr->next;
        }
        else 
        {
            return NULL;
        }
    }

    return ptr->name;
}

static osMessageQId AaSysComGetQueueID(ESysComID sc_id)
{
    SSysComMng* ptr = _syscom_mng;

    if(ptr == NULL)
    {
        return NULL;
    }

    while(ptr->syscom_id != sc_id)
    {
        if(ptr->next != NULL)
        {
            ptr = ptr->next;
        }
        else 
        {
            return NULL;
        }
    }

    return ptr->q_id;
}

static SSysComMng* AaSysComGetControlBlock(ESysComID sc_id)
{
    SSysComMng* ptr = _syscom_mng;

    if(ptr == NULL)
    {
        return NULL;
    }

    while(ptr->syscom_id != sc_id)
    {
        if(ptr->next != NULL)
        {
            ptr = ptr->next;
        }
        else 
        {
            return NULL;
        }
    }

    return ptr;
}

static bool AaSysComIsRegistered(ESysComID sc_id)
{
    SSysComMng* ptr = _syscom_mng;

    if(ptr == NULL)
    {
        return false;
    }

    while(ptr->syscom_id != sc_id)
    {
        if(ptr->next != NULL)
        {
            ptr = ptr->next;
        }
        else 
        {
            return false;
        }
    }

    return true;
}

static void AaSysComAdd2ControlBlock(SSysComMng* new)
{
    struct SSysComMng_t* cur;
    
    if(_syscom_mng == NULL)
    {
        _syscom_mng = new;
        new->next = NULL;
        new->prev = NULL;
    }
    else
    {
        cur = _syscom_mng;
        while(cur->next != NULL)
        {
            cur = cur->next;
        } 

        cur->next = new;
        new->prev = cur;
        new->next = NULL;
    }
}

static void AaSysComRemove4ControlBlock(SSysComMng* rm)
{
    struct SSysComMng_t* prv;
    struct SSysComMng_t* nxt;

    if(rm == NULL)
    {
        return ;
    }

    if(rm->prev == NULL)
    {
        if(rm->next == NULL)
        {
            _syscom_mng = NULL;
        }
        else
        {
            nxt = rm->next;
            nxt->prev = NULL;
            
            _syscom_mng = nxt;
        }
    }
    else
    {
        if(rm->next == NULL)
        {
            prv = rm->prev;
            prv->next = NULL;
        }
        else 
        {
            prv = rm->prev;
            nxt = rm->next;

            prv->next = nxt;
            nxt->prev = prv;
        }
    }
}



// end of file


