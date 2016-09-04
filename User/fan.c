
/***

History:
[2016-07-28 Menki]: Create

*/


#include "fan.h"
#include <stdio.h>
#include "cmsis_os.h"
#include "AaInclude.h"

TIM_HandleTypeDef htimx;

/** RunFanThread handler id */  
osThreadId _runfan_id;


/**
  * ��������: ��ʱ��Ӳ����ʼ������
  * �������: htim����ʱ���������ָ��
  * �� �� ֵ: ��
  * ˵    ��: �ú�����GENERAL_TIMx_Init��������
  */
void HAL_TIM_MspPostInit(TIM_HandleTypeDef* htim)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    /* ��ʱ��ͨ���������Ŷ˿�ʱ��ʹ�� */
    GENERAL_TIM_GPIO_RCC_CLK_ENABLE();
    
    /* ��ʱ��ͨ��1��������IO��ʼ�� */
    GPIO_InitStruct.Pin = GENERAL_TIM_CH1_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF3_TIM9;
    HAL_GPIO_Init(GENERAL_TIM_CH1_PORT, &GPIO_InitStruct);

    /* ��ʱ��ͨ��2��������IO��ʼ�� */
    GPIO_InitStruct.Pin = GENERAL_TIM_CH2_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF3_TIM9;
    HAL_GPIO_Init(GENERAL_TIM_CH2_PORT, &GPIO_InitStruct);

}

/**
  * ��������: ͨ�ö�ʱ����ʼ��������ͨ��PWM���
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ��
  */
void GENERAL_TIMx_Init(void)
{
  TIM_ClockConfigTypeDef sClockSourceConfig;
  TIM_MasterConfigTypeDef sMasterConfig;
  TIM_OC_InitTypeDef sConfigOC;
  
  /* ������ʱ������ʱ��ʹ�� */
  GENERAL_TIM_RCC_CLK_ENABLE();
  
  htimx.Instance = GENERAL_TIMx;
  htimx.Init.Prescaler = GENERAL_TIM_PRESCALER;
  htimx.Init.CounterMode = TIM_COUNTERMODE_UP;
  htimx.Init.Period = GENERAL_TIM_PERIOD;
  htimx.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;
  HAL_TIM_Base_Init(&htimx);
  
  
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  HAL_TIM_ConfigClockSource(&htimx, &sClockSourceConfig);

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  HAL_TIMEx_MasterConfigSynchronization(&htimx, &sMasterConfig);
  
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = GENERAL_TIM_CH1_PULSE;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  HAL_TIM_PWM_ConfigChannel(&htimx, &sConfigOC, TIM_CHANNEL_1);

  sConfigOC.Pulse = GENERAL_TIM_CH2_PULSE;
  HAL_TIM_PWM_ConfigChannel(&htimx, &sConfigOC, TIM_CHANNEL_2);

  HAL_TIM_MspPostInit(&htimx);
}


/*******************************************************************************
* ������  : FAN_deviceInit
* ��  ��  : ��ʼ��������Դ����
* ��  ��  : ��
* ��  ��  : ��
* �� �� ֵ: ��
*******************************************************************************/
void FAN_deviceInit(void)
{
    /* ͨ�ö�ʱ����ʼ��������PWM������� */
    GENERAL_TIMx_Init();

    /* ֹͣͨ��PWM��� */
    HAL_TIM_PWM_Stop(&htimx,TIM_CHANNEL_1);
    HAL_TIM_PWM_Stop(&htimx,TIM_CHANNEL_2);
}

/*******************************************************************************
* ������  : FAN_setSpeed
* ��  ��  : ���÷�����ת�ٶ�
* ��  ��  : channel:ͨ���ţ�FAN_CH0��FAN_CH1 
*           speed: �ٶ�ֵ(0-100,����=0ʱֹͣ��=100Ϊ����ٶ�)
* ��  ��  : ��
* �� �� ֵ: ERROR��  ���ó���
*           SUCCESS�����óɹ�
*******************************************************************************/
uint8_t FAN_setSpeed(uint8_t channel, uint8_t speed)
{
    TIM_OC_InitTypeDef sConfigOC;
    
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;

    if (channel == FAN_CH0)
    {
        /* ֹͣͨ��PWM��� */
        HAL_TIM_PWM_Stop(&htimx,TIM_CHANNEL_1);
        
        sConfigOC.Pulse = speed*10;
        HAL_TIM_PWM_ConfigChannel(&htimx, &sConfigOC, TIM_CHANNEL_1);
        
        /* ����ͨ��PWM��� */
        HAL_TIM_PWM_Start(&htimx,TIM_CHANNEL_1);
   
    }
    else if (channel == FAN_CH1)
    {
        /* ֹͣͨ��PWM��� */
        HAL_TIM_PWM_Stop(&htimx,TIM_CHANNEL_2);
        
        sConfigOC.Pulse = speed*10;
        HAL_TIM_PWM_ConfigChannel(&htimx, &sConfigOC, TIM_CHANNEL_2);
        
        /* ����ͨ��PWM��� */
        HAL_TIM_PWM_Start(&htimx,TIM_CHANNEL_2);
    }
    return SUCCESS;

}

void FanStart()
{
    FAN_deviceInit();
    
    FAN_setSpeed(FAN_CH0, 80);
    
    FAN_setSpeed(FAN_CH1, 80);
}




#if 0
/**
  * @brief  run Fan thread
  * @param  thread not used
  * @retval None
  */
static void RunFanThread(void const *argument)
{
  (void) argument;
  unsigned char i =0;
   //   GPIO_InitTypeDef GPIO_InitStruct;
  AaSysLogPrintF(LOGLEVEL_INF, SystemStartup, "RunFanThread started");

  FAN_deviceInit();

#if 0
    /* ��ʱ��ͨ���������Ŷ˿�ʱ��ʹ�� */
    GENERAL_TIM_GPIO_RCC_CLK_ENABLE();
    
    /* ��ʱ��ͨ��1��������IO��ʼ�� */
    GPIO_InitStruct.Pin = GENERAL_TIM_CH1_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GENERAL_TIM_CH1_PORT, &GPIO_InitStruct);

    /* ��ʱ��ͨ��2��������IO��ʼ�� */
    GPIO_InitStruct.Pin = GENERAL_TIM_CH2_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GENERAL_TIM_CH2_PORT, &GPIO_InitStruct);
#endif
    
  while(1)
  {
      for (i=0; i<110; i+=10)
      {
          FAN_setSpeed(FAN_CH1, i);
          osDelay(3000);
          AaSysLogPrintF(LOGLEVEL_DBG, SystemStartup, "RunFanThread running i=%d\n", i);
      }
     // HAL_GPIO_TogglePin(GENERAL_TIM_CH2_PORT, GENERAL_TIM_CH2_PIN);
      osDelay(2000);
  }
}


/**
  * @brief  start fan from system level
  * @param  none
  * @retval None
  */
void StartFanTask()
{
    osThreadDef(FanLed, RunFanThread, osPriorityNormal, 0, configMINIMAL_STACK_SIZE);
    _runfan_id = AaThreadCreateStartup(osThread(FanLed), NULL);
}

#endif




// end of file





