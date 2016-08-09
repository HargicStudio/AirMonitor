#ifndef _MODULE_STATUS_H
#define _MODULE_STATUS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"
	
typedef enum MODULE_e
{
    MDU_GSM = 0,
    MDU_GPS = 1,
    MDU_IN_TEMP_WET = 2,
    MDU_OUT_TEMP_WET = 3,
    MDU_PM25 = 4,
    MDU_PM10_SHARP = 5,
    MDU_CO = 6,
    MDU_SO2 = 7,
    MDU_NO2 = 8,
    MDU_O3 = 9,
    MDU_FAN1 = 10,
    MDU_FAN2 = 11,
    MDU_CARD = 12,
    

}MODULE_e;

typedef enum STATUS_e
{
    STU_NORMAL,			/* 设备正常 */
    STU_NO,			        /* 设备不存在 */
    STU_ERROR,                      /* 设备出错 */
	
}STATUS_e;

void SetModuleStu(MODULE_e type, STATUS_e stu);
STATUS_e GetModuleStu(MODULE_e type);
u32 GetAllModuleStu(void);

	
#ifdef __cplusplus
}
#endif

#endif 
