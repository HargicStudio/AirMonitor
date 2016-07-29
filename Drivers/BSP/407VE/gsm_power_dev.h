
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LED_DEV_H
#define __LED_DEV_H

#ifdef __cplusplus
extern "C" {
#endif



#include "stm32f4xx_hal.h"



#define GSM_POWER_PIN                   GPIO_PIN_12
#define GSM_POWER_GPIO_PORT             GPIOD



void GsmPowerGpioInit(void);
void GsmPowerUp(void);
void GsmPowerDown(void);




#ifdef __cplusplus
}
#endif

#endif /* __LED_DEV_H */





