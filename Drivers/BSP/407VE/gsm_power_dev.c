

/***

History:
[2016-05-21 Ted]: Create

*/


#include "gsm_power_dev.h"
#include <stdio.h>
#include "cmsis_os.h"



/**
  * @brief  initialize GSM POWER GPIO.
  * @param  none
  * @retval None
  */
void GsmPowerGpioInit(void)
{
  GPIO_InitTypeDef  gpioinitstruct = {0};
  
  /* Enable the GPIO_LED Clock */
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /* Configure the GPIO_LED pin */
  gpioinitstruct.Pin    = GSM_POWER_PIN;
  gpioinitstruct.Mode   = GPIO_MODE_OUTPUT_PP;
  gpioinitstruct.Pull   = GPIO_PULLUP;
  gpioinitstruct.Speed  = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GSM_POWER_GPIO_PORT, &gpioinitstruct);
}


/**
  * @brief  Turns selected GSM POWER PIN UP.
  * @param  none
  * @retval None
  */
void GsmPowerUp(void)
{
  HAL_GPIO_WritePin(GSM_POWER_GPIO_PORT, GSM_POWER_PIN, GPIO_PIN_SET); 
}

/**
  * @brief  Turns selected GSM POWER PIN Down.
  * @param  none
  * @retval None
  */
void GsmPowerDown(void)
{
  HAL_GPIO_WritePin(GSM_POWER_GPIO_PORT, GSM_POWER_PIN, GPIO_PIN_RESET); 
}


// end of file






