#ifndef _DATA_RECV_H
#define _DATA_RECV_H

#ifdef __cplusplus
extern "C" {
#endif
  
#include "AaInclude.h"

typedef enum ACK_TYPE_e
{
    REPLY_501,
    REPLY_502,
    
}ACK_TYPE_e;
  
void HandleGsmRecv(u8 *buf, u16 len);

void ProcessRecvData(u8 *buf, int cmd);

void ProcessServerCall(u8 *buf);
void ProcessSavePower(u8 *buf);
void ProcessChangeAddr(u8 *buf);
void ProcessChangeSimpleInterval(u8 *buf);
void ProcessChangeReportInterval(u8 *buf);
void ProcessChangeGetModuleStatus(u8 *buf);
void ProcessGetSoftVersion(u8 *buf);
void ProcessGetHardVersion(u8 *buf);
void ProcessAdjust(u8 *buf);
void ProcessConfig(u8 *buf);
void ProcessReboot(u8 *buf);

void ProcessServerResp(u8 *buf);

void ProcessRecall(u8 *buf);

void ProcessServerTime(u8 *buf);

void ProcessGetPosition(u8 *buf);

void ConstructResponse(u8 *cmd, u8* addr, u16 type);

void ConstructCommonResponse(u8 *cmd, u8* addr, u8 *opt, u8 optLen);

void ProcessStopRecall(u8 *buf);

void ProcessSerGetStationInfo(u8 *buf);

void ProcessSerConfigStation(u8 *buf);

  
#ifdef __cplusplus
}
#endif
  
#endif

