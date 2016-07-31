

/***

History:
[2016-07-21 Ted]: Create

*/


#include "dnc3_dev.h"
#include <stdio.h>
#include "cmsis_os.h"



/**
  * @brief  initialize GPIO.
  * @param  none
  * @retval None
  */
void Dnc3GpioInit()
{
  GPIO_InitTypeDef  gpioinitstruct = {0};
  
  /* Enable the Clock */
  DN7C3_FAN_CTRL_CLK_ENABLE();
  DN7C3_FAN_DET_CLK_ENABLE();

  /* Configure the pin */
  gpioinitstruct.Pin    = DN7C3_FAN_CTRL_PIN;
  gpioinitstruct.Mode   = GPIO_MODE_OUTPUT_PP;
  gpioinitstruct.Pull   = GPIO_PULLUP;
  gpioinitstruct.Speed  = GPIO_SPEED_LOW;
  HAL_GPIO_Init(DN7C3_FAN_CTRL_PORT, &gpioinitstruct);

  /* Configure the pin */
  gpioinitstruct.Pin    = DN7C3_FAN_DET_PIN;
  gpioinitstruct.Mode   = GPIO_MODE_INPUT;
  gpioinitstruct.Pull   = GPIO_PULLUP;
  gpioinitstruct.Speed  = GPIO_SPEED_FAST;
  HAL_GPIO_Init(DN7C3_FAN_DET_PORT, &gpioinitstruct);
}


/**
  * @brief  initialize GPIO.
  * @param  none
  * @retval None
  */
void Dnc3PwmInit()
{

}


/**
  * @brief  initialize GPIO.
  * @param  none
  * @retval None
  */
void Dnc3I2CInit()
{
    
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





