
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

// ���嶨ʱ��Ԥ��Ƶ����ʱ��ʵ��ʱ��Ƶ��Ϊ��160MHz/��GENERAL_TIMx_PRESCALER+1��
#define GENERAL_TIM_PRESCALER            80  // ʵ��ʱ��Ƶ��Ϊ��2MHz

// ���嶨ʱ�����ڣ�����ʱ����ʼ������GENERAL_TIMx_PERIODֵ�Ǹ��¶�ʱ�������ɶ�Ӧ�¼����ж�
#define GENERAL_TIM_PERIOD               1000  // ��ʱ�������ж�Ƶ��Ϊ��2MHz/1000=1KHz����0.5ms��ʱ����

#define GENERAL_TIM_CH1_PULSE            900   // ��ʱ��ͨ��1ռ�ձ�Ϊ��GENERAL_TIM_CH1_PULSE/GENERAL_TIM_PERIOD*100%=900/1000*100%=90%
#define GENERAL_TIM_CH2_PULSE            600   // ��ʱ��ͨ��2ռ�ձ�Ϊ��GENERAL_TIM_CH2_PULSE/GENERAL_TIM_PERIOD*100%=600/1000*100%=60%

//ͨ������
#define  FAN_CH0   0
#define  FAN_CH1   1    


/* �������� ------------------------------------------------------------------*/

/*******************************************************************************
* ������  : FAN_deviceInit
* ��  ��  : ��ʼ��������Դ����
* ��  ��  : ��
* ��  ��  : ��
* �� �� ֵ: ��
*******************************************************************************/
void FAN_deviceInit(void);

/*******************************************************************************
* ������  : FAN_setSpeed
* ��  ��  : ���÷�����ת�ٶ�
* ��  ��  : channel:ͨ���ţ�FAN_CH0��FAN_CH1 
*           speed: �ٶ�ֵ(0-100,����=0ʱֹͣ��=100Ϊ����ٶ�)
* ��  ��  : ��
* �� �� ֵ: ERROR��  ���ó���
*           SUCCESS�����óɹ�
*******************************************************************************/
uint8_t FAN_setSpeed(uint8_t channel, uint8_t speed);

//void StartFanTask();



#ifdef __cplusplus
}
#endif

#endif /* __FAN_H */




