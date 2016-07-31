
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DN7C3_dev_h
#define __DN7C3_dev_h

#ifdef __cplusplus
extern "C" {
#endif



#include "stm32f4xx_hal.h"


/** Description of the macro */ 
#define DN7C3_FAN_CTRL_PIN              GPIO_PIN_3
#define DN7C3_FAN_CTRL_PORT             GPIOE

#define DN7C3_FAN_CTRL_CLK_ENABLE()     __HAL_RCC_GPIOE_CLK_ENABLE()

/** Description of the macro */ 
#define DN7C3_FAN_DET_PIN               GPIO_PIN_4
#define DN7C3_FAN_DET_PORT              GPIOE

#define DN7C3_FAN_DET_CLK_ENABLE()      __HAL_RCC_GPIOE_CLK_ENABLE()

/** Description of the macro */ 
#define DN7C3_PWM_PIN                   GPIO_PIN_9
#define DN7C3_PWM_PORT                  GPIOE

/** Description of the macro */ 
#define DN7C3_SCL_PIN                   GPIO_PIN_10
#define DN7C3_SCL_PORT                  GPIOB

#define DN7C3_SDA_PIN                   GPIO_PIN_11
#define DN7C3_SDA_PORT                  GPIOB



#ifdef __cplusplus
}
#endif

#endif /* __DN7A3_dev_h */




