

/***

History:
[2016-05-21 Ted]: Create

*/


#include "led_dev.h"
#include <stdio.h>
#include "cmsis_os.h"
#include "AaInclude.h"



/** RunLedThread handler id */  
osThreadId _runled_id;



static void LedDeviceInit(void);



/**
  * @brief  run led thread
  * @param  thread not used
  * @retval None
  */
static void RunLedThread(void const *argument)
{
  (void) argument;

  AaSysLogPrintF(LOGLEVEL_INF, SystemStartup, "RunLedThread started");

  for (;;)
  {
      LedToggle();
      osDelay(1000);
      AaSysLogPrintF(LOGLEVEL_DBG, SystemStartup, "System running");
  }
}


/**
  * @brief  start led from system level
  * @param  none
  * @retval None
  */
void StartRunLedTask()
{
    LedDeviceInit();

    osThreadDef(RunLed, RunLedThread, osPriorityNormal, 0, configMINIMAL_STACK_SIZE);
    _runled_id = AaThreadCreateStartup(osThread(RunLed), NULL);
}


/**
  * @brief  initialize LED GPIO.
  * @param  none
  * @retval None
  */
static void LedDeviceInit()
{
  LedGpioInit();

  /* Reset PIN to switch off the LED */
  LedOff();

  AaSysLogPrintF(LOGLEVEL_INF, SystemStartup, "led device initialize success");
}



// end of file





