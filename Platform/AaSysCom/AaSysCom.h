


/***

History:
2016-04-30: Ted: Create

*/

#ifndef _AASYSCOM_H_
#define _AASYSCOM_H_

#ifdef __cplusplus
 extern "C" {
#endif 



#include "syscom_cfg.h"
#include "ApiInternalMsg.h"



typedef u32 SAaSysComMsgId;



typedef struct SMsgHeader_t {
    u32             magic;
    SAaSysComMsgId  msg_id;
    ESysComID       target;
    ESysComID       sender;
    u16             pl_size;
} SMsgHeader;



typedef enum
{
    SysCom_Ok = 0,
    SysCom_ErrParam,
    SysCom_ErrMem,
    SysCom_ErrProc,
    SysCom_ErrRTOS,
} ESysComStatus;



u8 AaSysComCEInit();
ESysComID AaSysComRegister(ESysComID syscom, char* name, u32 q_size);
ESysComStatus AaSysComUnregister(ESysComID syscom);
void* AaSysComCreate(SAaSysComMsgId msgid, ESysComID sender, ESysComID receiver, u16 pl_size);
void AaSysComDestory(void* msg_ptr);
void* AaSysComGetPayload(void* msg_ptr);
ESysComStatus AaSysComSend(void* msg_ptr, u32 timeout);
ESysComID AaSysComGetSender(void* msg_ptr);
ESysComStatus AaSysComSetSender(void* msg_ptr, ESysComID sender);
ESysComID AaSysComGetReceiver(void* msg_ptr);
ESysComStatus AaSysComSetReceiver(void* msg_ptr, ESysComID receiver);
ESysComStatus AaSysComForward(void* msg_ptr, ESysComID sender, ESysComID receiver, u32 timeout);
void* AaSysComReceiveHandler(ESysComID receiver, u32 timeout);

   
#ifdef __cplusplus
}
#endif

#endif // _AASYSCOM_H_

// end of file


