

/***

History:
[2016-05-21 Ted]: Create

*/


#include "led_dev.h"
#include <stdio.h>
#include "cmsis_os.h"



/**
  * @brief  initialize LED GPIO.
  * @param  none
  * @retval None
  */
void LedGpioInit()
{
  GPIO_InitTypeDef  gpioinitstruct = {0};
  
  /* Enable the GPIO_LED Clock */
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /* Configure the GPIO_LED pin */
  gpioinitstruct.Pin    = LED_PIN;
  gpioinitstruct.Mode   = GPIO_MODE_OUTPUT_PP;
  gpioinitstruct.Pull   = GPIO_PULLUP;
  gpioinitstruct.Speed  = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(LED_GPIO_PORT, &gpioinitstruct);
}


/**
  * @brief  Turns selected LED On.
  * @param  none
  * @retval None
  */
void LedOn()
{
  HAL_GPIO_WritePin(LED_GPIO_PORT, LED_PIN, GPIO_PIN_RESET); 
}

/**
  * @brief  Turns selected LED Off.
  * @param  none
  * @retval None
  */
void LedOff()
{
  HAL_GPIO_WritePin(LED_GPIO_PORT, LED_PIN, GPIO_PIN_SET); 
}

/**
  * @brief  Toggles the selected LED.
  * @param  none
  * @retval None
  */
void LedToggle()
{
  HAL_GPIO_TogglePin(LED_GPIO_PORT, LED_PIN);
}


// end of file





