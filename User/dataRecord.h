#ifndef _DATA_RECORD_H_
#define _DATA_RECORD_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32f4xx_hal.h"
#include "AaInclude.h"
   
s32 StartDataRecordTask(void);
   
bool ReadConfigFile(void);
   
#ifdef __cplusplus
}
#endif
   
#endif