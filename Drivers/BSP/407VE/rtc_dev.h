
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __RTC_DEV_H
#define __RTC_DEV_H

#ifdef __cplusplus
extern "C" {
#endif



#include "stm32f4xx_hal.h"
#include "AaInclude.h"
#include <time.h>



u8 RtcInit();
void HAL_RTC_MspInit(RTC_HandleTypeDef *hrtc);
void HAL_RTC_MspDeInit(RTC_HandleTypeDef *hrtc);
u8 RTC_SetCalendar(const struct tm tblock);
u8 RTC_SetTime(time_t tp);
void RTC_GetCalendar(struct tm *tblock);
void RTC_CalendarReset();



#ifdef __cplusplus
}
#endif

#endif /* __RTC_DEV_H */




