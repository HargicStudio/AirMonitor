
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LED_DEV_H
#define __LED_DEV_H

#ifdef __cplusplus
extern "C" {
#endif



#include "stm32f4xx_hal.h"



#define LED_PIN                         GPIO_PIN_9
#define LED_GPIO_PORT                   GPIOB



void LedGpioInit(void);
void LedOn();
void LedOff();
void LedToggle();




#ifdef __cplusplus
}
#endif

#endif /* __LED_DEV_H */




