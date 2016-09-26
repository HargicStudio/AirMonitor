

/***

History:
[2016-07-21 Ted]: Create
[2016-09-23 Ted]: Trigger ADC by Timer PWM

*/


#include "dnc3_dev.h"
#include <stdio.h>
#include "cmsis_os.h"



#define  PERIOD_VALUE       (52500 - 1)  /* 10 ms  */
#define  PULSE1_VALUE       1680        /* 320 us  */
#define  PULSE2_VALUE       1470        /* 280 us  */


/* Private variables ---------------------------------------------------------*/
/* ADC handler declaration */
ADC_HandleTypeDef    AdcHandle;


/* Variable used to get converted value */
__IO uint16_t uhADCxConvertedValue = 0;


/* Timer handler declaration */
TIM_HandleTypeDef    TimHandle;


static u8 ADC_Config(void);
static u8 TIM_PWM_Config();



/**
  * @brief  initialize GPIO.
  * @param  none
  * @retval None
  */
u8 Dnc3Init()
{
    u8 ret;
    
    ret = TIM_PWM_Config();
    if (ret != 0)
    {
        return 0;
    }
    
    ret = ADC_Config();

    return ret;
}


/**
  * @brief  initialize GPIO.
  * @param  none
  * @retval None
  */
u8 Dnc3Start()
{
    if(HAL_ADC_Start_IT(&AdcHandle) != HAL_OK)
    {
        /* Start Conversation Error */
        return 1;
    }

    /* Start channel 1 */
    if(HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_1) != HAL_OK)
    {
        /* PWM Generation Error */
        return 2;
    }
    /* Start channel 2 */
    if(HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_2) != HAL_OK)
    {
        /* PWM Generation Error */
        return 3;
    }

    return 0;
}

u16 AdcGet()
{
    return uhADCxConvertedValue;
}


static u8 ADC_Config(void)
{
  ADC_ChannelConfTypeDef sConfig;
  
   /* ADC Initialization */
  AdcHandle.Instance          = ADCx;
  
  AdcHandle.Init.ClockPrescaler = ADC_CLOCKPRESCALER_PCLK_DIV8;
  AdcHandle.Init.Resolution = ADC_RESOLUTION_12B;
  AdcHandle.Init.ScanConvMode = ENABLE;
  AdcHandle.Init.ContinuousConvMode = DISABLE;
  AdcHandle.Init.DiscontinuousConvMode = DISABLE;
  AdcHandle.Init.NbrOfDiscConversion = 0;
  AdcHandle.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_FALLING;
  AdcHandle.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T8_TRGO;
  AdcHandle.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  AdcHandle.Init.NbrOfConversion = 1;
  AdcHandle.Init.DMAContinuousRequests = ENABLE;
  AdcHandle.Init.EOCSelection = ADC_EOC_SEQ_CONV;
      
  if(HAL_ADC_Init(&AdcHandle) != HAL_OK)
  {
    /* ADC Initialization Error */
    return 1;
  }
  
  /* Configure ADC3 regular channel */  
  sConfig.Channel = ADCx_CHANNEL;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  sConfig.Offset = 0;
  
  if(HAL_ADC_ConfigChannel(&AdcHandle, &sConfig) != HAL_OK)
  {
    /* Channel Configuration Error */
    return 2;
  }

  return 0;
}

static u8 TIM_PWM_Config()
{
  TIM_MasterConfigTypeDef sMasterConfig;
  /* Timer Output Compare Configuration Structure declaration */
  // !!! should be initialized
  TIM_OC_InitTypeDef sConfig = {0};
  /* Counter Prescaler value */
  uint32_t uhPrescalerValue = 0;
    
  /* Compute the prescaler value to have TIM3 counter clock equal to 21 MHz */
  uhPrescalerValue = (uint32_t) ((SystemCoreClock /2) / 2625000) - 1;
  
  /* Initialize TIMx peripheral as follow:
       + Prescaler = (SystemCoreClock/2)/21000000
       + Period = 665
       + ClockDivision = 0
       + Counter direction = Up
  */
  TimHandle.Instance = TIMx;
  
  TimHandle.Init.Prescaler = uhPrescalerValue;
  TimHandle.Init.Period = PERIOD_VALUE;
  TimHandle.Init.ClockDivision = 0;
  TimHandle.Init.CounterMode = TIM_COUNTERMODE_UP;
  if(HAL_TIM_PWM_Init(&TimHandle) != HAL_OK)
  {
    /* Initialization Error */
    return 1;
  }
  
  /*##-2- Configure the PWM channels #########################################*/ 
  /* Common configuration for all channels */
  sConfig.OCMode = TIM_OCMODE_PWM1;
  sConfig.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfig.OCFastMode = TIM_OCFAST_DISABLE;

  /* Set the pulse value for channel 1 */
  sConfig.Pulse = PULSE1_VALUE;  
  if(HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_1) != HAL_OK)
  {
    /* Configuration Error */
    return 2;
  }
  
  /* Set the pulse value for channel 2 */
  sConfig.Pulse = PULSE2_VALUE;
  if(HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_2) != HAL_OK)
  {
    /* Configuration Error */
    return 3;
  }

  /* TIM8 TRGO selection */
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_OC2REF;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  
  if(HAL_TIMEx_MasterConfigSynchronization(&TimHandle, &sMasterConfig) != HAL_OK)
  {
    /* TIM8 TRGO selection Error */
    return 4;
  }

  return 0;
}

/**
  * @brief  Conversion complete callback in non blocking mode 
  * @param  AdcHandle : ADC handle
  * @note   This example shows a simple way to report end of conversion, and 
  *         you can add your own implementation.    
  * @retval None
  */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* AdcHandle)
{
  /* Get the converted value of regular channel */
  uhADCxConvertedValue = HAL_ADC_GetValue(AdcHandle);
}


/**
  * @brief ADC MSP Initialization 
  *        This function configures the hardware resources used in this example: 
  *           - Peripheral's clock enable
  *           - Peripheral's GPIO Configuration  
  * @param hadc: ADC handle pointer
  * @retval None
  */
void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
{
  GPIO_InitTypeDef          GPIO_InitStruct;
  
  /*##-1- Enable peripherals and GPIO Clocks #################################*/
  /* Enable GPIO clock ****************************************/
  ADCx_CHANNEL_GPIO_CLK_ENABLE();
  /* ADC3 Periph clock enable */
  ADCx_CLK_ENABLE();
    
  /*##-2- Configure peripheral GPIO ##########################################*/ 
  /* ADC3 Channel8 GPIO pin configuration */
  GPIO_InitStruct.Pin = ADCx_CHANNEL_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(ADCx_CHANNEL_GPIO_PORT, &GPIO_InitStruct);
  
  /*##-3- Configure the NVIC #################################################*/
  /* NVIC configuration for DMA transfer complete interrupt (USART1_TX) */
  HAL_NVIC_SetPriority(ADCx_IRQn, SYSTEM_IRQ_PRIORITY_HIGH_2, 0);
  HAL_NVIC_EnableIRQ(ADCx_IRQn);
}
  
/**
  * @brief ADC MSP De-Initialization 
  *        This function frees the hardware resources used in this example:
  *          - Disable the Peripheral's clock
  *          - Revert GPIO to their default state
  * @param hadc: ADC handle pointer
  * @retval None
  */
void HAL_ADC_MspDeInit(ADC_HandleTypeDef *hadc)
{
  
  /*##-1- Reset peripherals ##################################################*/
  ADCx_FORCE_RESET();
  ADCx_RELEASE_RESET();

  /*##-2- Disable peripherals and GPIO Clocks ################################*/
  /* De-initialize the ADC3 Channel8 GPIO pin */
  HAL_GPIO_DeInit(ADCx_CHANNEL_GPIO_PORT, ADCx_CHANNEL_PIN);
}

/**
  * @brief TIM MSP Initialization 
  *        This function configures the hardware resources used in this example: 
  *           - Peripheral's clock enable
  *           - Peripheral's GPIO Configuration  
  * @param htim: TIM handle pointer
  * @retval None
  */
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
{
  GPIO_InitTypeDef   GPIO_InitStruct;
  /*##-1- Enable peripherals and GPIO Clocks #################################*/
  /* TIMx Peripheral clock enable */
  TIMx_CLK_ENABLE();
    
  /* Enable GPIO Channels Clock */
  TIMx_CHANNEL_GPIO_PORT();

  /* Common configuration for all channels */
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF3_TIM8;
  
  GPIO_InitStruct.Pin = GPIO_PIN_CHANNEL1;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin = GPIO_PIN_CHANNEL2;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}


// end of file





