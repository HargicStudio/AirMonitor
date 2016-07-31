
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __FAN_H
#define __FAN_H
#include "cmsis_os.h"
#include "AaInclude.h"
#include "main.h"

#ifdef __cplusplus
extern "C" {
#endif

#define GENERAL_TIMx                        TIM9
#define GENERAL_TIM_RCC_CLK_ENABLE()        __HAL_RCC_TIM9_CLK_ENABLE()
#define GENERAL_TIM_RCC_CLK_DISABLE()       __HAL_RCC_TIM9_CLK_DISABLE()
#define GENERAL_TIM_GPIO_RCC_CLK_ENABLE()   {__HAL_RCC_GPIOE_CLK_ENABLE();__HAL_RCC_GPIOE_CLK_ENABLE();}
#define GENERAL_TIM_CH1_PORT                GPIOE
#define GENERAL_TIM_CH1_PIN                 GPIO_PIN_5
#define GENERAL_TIM_CH2_PORT                GPIOE
#define GENERAL_TIM_CH2_PIN                 GPIO_PIN_6

// 定义定时器预分频，定时器实际时钟频率为：160MHz/（GENERAL_TIMx_PRESCALER+1）
#define GENERAL_TIM_PRESCALER            80  // 实际时钟频率为：2MHz

// 定义定时器周期，当定时器开始计数到GENERAL_TIMx_PERIOD值是更新定时器并生成对应事件和中断
#define GENERAL_TIM_PERIOD               1000  // 定时器产生中断频率为：2MHz/1000=1KHz，即0.5ms定时周期

#define GENERAL_TIM_CH1_PULSE            900   // 定时器通道1占空比为：GENERAL_TIM_CH1_PULSE/GENERAL_TIM_PERIOD*100%=900/1000*100%=90%
#define GENERAL_TIM_CH2_PULSE            600   // 定时器通道2占空比为：GENERAL_TIM_CH2_PULSE/GENERAL_TIM_PERIOD*100%=600/1000*100%=60%

//通道定义
#define  FAN_CH0   0
#define  FAN_CH1   1    


/* 函数声明 ------------------------------------------------------------------*/

/*******************************************************************************
* 函数名  : FAN_deviceInit
* 描  述  : 初始化风扇资源配置
* 输  入  : 无
* 输  出  : 无
* 返 回 值: 无
*******************************************************************************/
void FAN_deviceInit(void);

/*******************************************************************************
* 函数名  : FAN_setSpeed
* 描  述  : 配置风扇旋转速度
* 输  入  : channel:通道号，FAN_CH0或FAN_CH1 
*           speed: 速度值(0-100,其中=0时停止，=100为最快速度)
* 输  出  : 无
* 返 回 值: ERROR：  设置出错
*           SUCCESS：设置成功
*******************************************************************************/
uint8_t FAN_setSpeed(uint8_t channel, uint8_t speed);

//void StartFanTask();



#ifdef __cplusplus
}
#endif

#endif /* __FAN_H */




