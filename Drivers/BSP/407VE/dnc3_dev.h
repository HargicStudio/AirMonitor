
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DN7C3_dev_h
#define __DN7C3_dev_h

#ifdef __cplusplus
extern "C" {
#endif



#include "stm32f4xx_hal.h"
#include "AaInclude.h"


/* Definition for ADCx clock resources */
#define ADCx                            ADC1
#define ADCx_CLK_ENABLE()               __HAL_RCC_ADC1_CLK_ENABLE();
#define ADCx_CHANNEL_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOA_CLK_ENABLE()
     
#define ADCx_FORCE_RESET()              __HAL_RCC_ADC_FORCE_RESET()
#define ADCx_RELEASE_RESET()            __HAL_RCC_ADC_RELEASE_RESET()

/* Definition for ADCx Channel Pin */
#define ADCx_CHANNEL_PIN                GPIO_PIN_4
#define ADCx_CHANNEL_GPIO_PORT          GPIOA

/* Definition for ADCx's Channel */
#define ADCx_CHANNEL                    ADC_CHANNEL_4

/* Definition for ADCx's NVIC */
#define ADCx_IRQn                       ADC_IRQn



/* User can use this section to tailor TIMx instance used and associated 
   resources */
/* Definition for TIMx clock resources */
#define TIMx                            TIM8
#define TIMx_CLK_ENABLE()               __HAL_RCC_TIM8_CLK_ENABLE();

#define TIMx_FORCE_RESET()              __HAL_RCC_TIM8_FORCE_RESET()
#define TIMx_RELEASE_RESET()            __HAL_RCC_TIM8_RELEASE_RESET()

/* Definition for USARTx Pins */
#define TIMx_CHANNEL_GPIO_PORT()       __HAL_RCC_GPIOC_CLK_ENABLE()
#define GPIO_PIN_CHANNEL1              GPIO_PIN_6
#define GPIO_PIN_CHANNEL2              GPIO_PIN_7



u8 Dnc3Init();
u8 Dnc3Start();
u16 AdcGet();
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* AdcHandle);
void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc);
void HAL_ADC_MspDeInit(ADC_HandleTypeDef *hadc);
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim);



#ifdef __cplusplus
}
#endif

#endif /* __DN7A3_dev_h */




