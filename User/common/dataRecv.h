#ifndef _DATA_RECV_H
#define _DATA_RECV_H

#ifdef __cplusplus
extern "C" {
#endif
  
#include "AaInclude.h"


void HandleGsmRecv(u8 *buf, u16 len);
u32 stringToInt(u8 *buf, u16 len);
void ProcessRecvData(u8 *buf, int cmd);

void ProcessServerCall(u8 *buf);
void ProcessSavePower(u8 *buf);
void ProcessChangeAddr(u8 *buf);
void ProcessChangeSimpleInterval(u8 *buf);
void ProcessChangeReportInterval(u8 *buf);
void ProcessChangeGetModuleStatus(u8 *buf);
void ProcessGetSoftVersion(u8 *buf);
void ProcessGetHardVersion(u8 *buf);

  
#ifdef __cplusplus
}
#endif
  
#endif

