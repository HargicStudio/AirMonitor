


/***

History:
2016-04-23: Ted: Create

*/

#include "glo_def.h"
#include "AaSysCom.h"
#include "AaSysLog.h"
#include <stdbool.h>
#include <string.h>
#include "ApiInternalMsg.h"


typedef struct SMsgInternalHeader_t {
    SAaSysComMsgId  msg_id;
    SAaSysComSicad  target;
    SAaSysComSicad  sender;
    u16             body_size;
    void*           body;
} SMsgInternalHeader;


#define AASYSCOM_QUEUE_NAME_LENGTH_MAX    16

#define AASYSCOM_QUEUE_DEEP_MAX           8


// Define mail queue
osMailQDef(aasyscom_queue, AASYSCOM_QUEUE_DEEP_MAX, SMsgInternalHeader);
osMailQId  _aasyscom_queue_id;


EStatus AaSysComCEInit(void)
{
    EStatus err = ErrOk;
    char queue_name[AASYSCOM_QUEUE_NAME_LENGTH_MAX];

    for(u8 i=0; i<MsgQueue_MAX; i++) {
        sprintf(queue_name, "MsgQueue%d", i);
        _aasyscom_queue_id = osMailCreate(osMailQ(aasyscom_queue), NULL);
        if(_aasyscom_queue_id == NULL) {
            AaSysLogPrint(LOGLEVEL_ERR, FeatureSysCom, "MsgQueue%d initialize failed", i);
            err = ErrInitialize;
        } else {
            AaSysLogPrint(LOGLEVEL_INF, FeatureSysCom, "MsgQueue%d initialize success", i);
        }
    }

    return err;
}

EStatus AaSysComEEInit(osMailQId q_id)
{
    
}


void* AaSysComCreate(SAaSysComMsgId msgid, SAaSysComSicad sender, SAaSysComSicad receiver, u16 pl_size)
{
    if(sender >= MsgQueue_MAX || receiver >= MsgQueue_MAX) {
        AaSysLogPrint(LOGLEVEL_ERR, FeatureSysCom, "sender 0x%02x or receiver 0x%02x failed", sender, receiver);
        return NULL;
    }

    if(msgid >= API_MESSAGE_ID_MAX) {
        AaSysLogPrint(LOGLEVEL_ERR, FeatureSysCom, "message id 0x%04x failed", msgid);
        return NULL;
    }

    SMsgHeader* msg_ptr = AaMemMalloc(sizeof(SMsgHeader) + pl_size);
    if(msg_ptr == NULL) {
        AaSysLogPrint(LOGLEVEL_ERR, FeatureSysCom, "create message 0x%04x failed", msgid);
        return NULL;
    }

    msg_ptr->msg_id = msgid;
    msg_ptr->target = receiver;
    msg_ptr->sender = sender;
    msg_ptr->pl_size = pl_size;

    AaSysLogPrint(LOGLEVEL_DBG, FeatureSysCom, "create message 0x%04x success", msgid);

    return (void*)msg_ptr;
}


void* AaSysComGetPayload(void* msg_ptr)
{
    if(msg_ptr == NULL) {
        AaSysLogPrint(LOGLEVEL_ERR, FeatureSysCom, "msg_ptr is NULL");
        return NULL;
    }
    
    u8* msg = msg_ptr;
    
    return (msg + sizeof(SMsgHeader));
}


EStatus AaSysComSend(void* msg_ptr)
{
    if(msg_ptr == NULL) {
        AaSysLogPrint(LOGLEVEL_ERR, FeatureSysCom, "msg_ptr is NULL");
        return ErrParam;
    }

    SMsgInternalHeader* header = osMailAlloc(_aasyscom_queue_id, osWaitForever);  
    if(header == NULL) {
        AaSysLogPrint(LOGLEVEL_ERR, FeatureSysCom, "create internal msg failed");
        return ErrMem;
    }
    
    SMsgHeader* msg = (SMsgHeader*)msg_ptr;

    if(msg->sender >= MsgQueue_MAX || msg->target >= MsgQueue_MAX) {
        AaSysLogPrint(LOGLEVEL_ERR, FeatureSysCom, "sender 0x%02x or receiver 0x%02x failed", 
                msg->sender, msg->target);
        return ErrParam;
    }

    if(msg->msg_id >= API_MESSAGE_ID_MAX) {
        AaSysLogPrint(LOGLEVEL_ERR, FeatureSysCom, "message id 0x%04x failed", msg->msg_id);
        return ErrParam;
    }

    header->msg_id = msg->msg_id;
    header->target = msg->target;
    header->sender = msg->sender;
    header->body_size = sizeof(SMsgHeader) + msg->pl_size;
    header->body = msg_ptr;
    
    if(osOK != osMailPut(_aasyscom_queue_id, header)) {
        AaSysLogPrint(LOGLEVEL_ERR, FeatureSysCom, "create internal msg failed");
        osMailFree(_aasyscom_queue_id, header);
        AaMemFree(msg_ptr);
        return ErrRTOS;
    }

    AaSysLogPrint(LOGLEVEL_DBG, "message(0x%04x) have been sent from %s to %s", 
            header.msg_id, 
            AaSysComPrintThreadName(header.sender), 
            AaSysComPrintThreadName(header.target));

    return kNoErr;
}

SAaSysComSicad AaSysComGetSender(void* msg_ptr)
{
    if(msg_ptr == NULL) {
        AaSysLogPrint(LOGLEVEL_ERR, "msg_ptr is NULL");
        return MsgQueue_Unknow;
    }
    
    SMsgHeader* msg = (SMsgHeader*)msg_ptr;
    return msg->sender;
}

OSStatus AaSysComSetSender(void* msg_ptr, SAaSysComSicad sender)
{
    if(msg_ptr == NULL) {
        AaSysLogPrint(LOGLEVEL_ERR, "msg_ptr is NULL");
        return kParamErr;
    }

    SMsgHeader* msg = (SMsgHeader*)msg_ptr;
    
    if(sender >= MsgQueue_MAX) {
        AaSysLogPrint(LOGLEVEL_ERR, "sender 0x%02x incorrect", sender);
        return kParamErr;
    }

    msg->sender = sender;

    return kNoErr;
}

SAaSysComSicad AaSysComGetReceiver(void* msg_ptr)
{
    if(msg_ptr == NULL) {
        AaSysLogPrint(LOGLEVEL_ERR, "msg_ptr is NULL");
        return MsgQueue_Unknow;
    }
    
    SMsgHeader* msg = (SMsgHeader*)msg_ptr;
    return msg->target;
}

OSStatus AaSysComSetReceiver(void* msg_ptr, SAaSysComSicad receiver)
{
    if(msg_ptr == NULL) {
        AaSysLogPrint(LOGLEVEL_ERR, "msg_ptr is NULL");
        return kParamErr;
    }

    SMsgHeader* msg = (SMsgHeader*)msg_ptr;

    if(receiver >= MsgQueue_MAX) {
        AaSysLogPrint(LOGLEVEL_ERR, "target 0x%02x incorrect", receiver);
        return kParamErr;
    }

    msg->target = receiver;

    return kNoErr;
}


OSStatus AaSysComForward(void* msg_ptr, SAaSysComSicad sender, SAaSysComSicad receiver)
{
    OSStatus err;
        
    if(msg_ptr == NULL) {
        AaSysLogPrint(LOGLEVEL_ERR, "msg_ptr is NULL");
        return kParamErr;
    }
    
    if(sender >= MsgQueue_MAX || receiver >= MsgQueue_MAX) {
        AaSysLogPrint(LOGLEVEL_ERR, "sender 0x%02x or receiver 0x%02x failed", sender, receiver);
        return kParamErr;
    }

    SMsgHeader* msg = (SMsgHeader*)msg_ptr;

    SMsgHeader* msg_fw = malloc(sizeof(SMsgHeader) + msg->pl_size);
    if(msg_fw == NULL) {
        AaSysLogPrint(LOGLEVEL_ERR, "forward message 0x%04x failed", msg->msg_id);
        return kNoMemoryErr;
    }

    memcpy(msg_fw, msg_ptr, (sizeof(SMsgHeader) + msg->pl_size));
    err = AaSysComSetSender(msg_fw, sender);
    if(kNoErr != err) {
        AaSysLogPrint(LOGLEVEL_ERR, "forward message failed when set sender");
        return err;
    }
    err = AaSysComSetReceiver(msg_fw, receiver);
    if(kNoErr != err) {
        AaSysLogPrint(LOGLEVEL_ERR, "forward message failed when set target");
        return err;
    }

    err = AaSysComSend(msg_fw);

    return err;
}

void* AaSysComReceiveHandler(SAaSysComSicad receiver, u32 timeout)
{
    bool failed = false;
    SMsgInternalHeader header;

    if(kNoErr == mico_rtos_pop_from_queue(&msg_queue[receiver], &header, timeout)) {
        if(header.msg_id >= API_MESSAGE_ID_MAX) {
            AaSysLogPrint(LOGLEVEL_ERR, "receive message id 0x%04x failed", header.msg_id);
            failed = true;
        }

        if(header.target >= MsgQueue_MAX || header.sender >= MsgQueue_MAX) {
            AaSysLogPrint(LOGLEVEL_ERR, "receive target 0x%02x or sender 0x%02x failed", header.target, header.sender);
            failed = true;
        }

        SMsgHeader* msg_ptr = header.body;
        if(msg_ptr == NULL) {
            AaSysLogPrint(LOGLEVEL_ERR, "receive messgae body is NULL");
            failed = true;
        }
        else if(header.body_size != (sizeof(SMsgHeader) + msg_ptr->pl_size)) {
            AaSysLogPrint(LOGLEVEL_ERR, "receive message body size %d is not match sizeof(SMsgHeader) %d + pl_size %d", 
                    header.body_size, sizeof(SMsgHeader), msg_ptr->pl_size);
            failed = true;
        }

        if(failed == true) {
            if(header.body != NULL) free(header.body);
            return NULL;
        }

        AaSysLogPrint(LOGLEVEL_DBG, "receive message id 0x%04x from %s success", 
                header.msg_id, 
                AaSysComPrintThreadName(header.sender));
        
        return (void*)header.body;
    }
    else {
        return NULL;
    }
}



// end of file


