

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
osThreadId _runled_id_fork;



static void LedDeviceInit(void);
static void RunLedThread(void const *argument);
static void RunLedThreadFork(void const *argument);
static void HandleLedIndicationMsg(void* msg);


/**
  * @brief  run led thread
  * @param  thread not used
  * @retval None
  */
static void RunLedThread(void const *argument)
{
    (void) argument;
    void* msg;
    u8 cnt = 0;

    AaSysLogPrintF(LOGLEVEL_INF, SystemStartup, "%s started", __FUNCTION__);

    if(SysCom_RunLed1 != AaSysComRegister(SysCom_RunLed1, MAKECHAR(RunLedThread)"Queue", 8))
    {
        AaSysLogPrintF( LOGLEVEL_ERR, SystemStartup, "%s %d: AaSysComRegister failed", __FUNCTION__, __LINE__);
    }
    else 
    {
        AaSysLogPrintF( LOGLEVEL_INF, SystemStartup, "%s %d: AaSysComRegister success", __FUNCTION__, __LINE__);
    }

    for (;;)
    {
        LedToggle();
        osDelay(1000);
//        AaSysLogPrintF(LOGLEVEL_DBG, SystemStartup, "System running");

        msg = AaSysComCreate(API_MESSAGE_ID_LED_INDICATION, SysCom_RunLed1, SysCom_RunLed2, sizeof(SLedIndication));
        if(msg != NULL)
        {
            SLedIndication* pl = AaSysComGetPayload(msg);
            if(pl != NULL)
            {
                pl->led_status = (cnt++)%2 ? true : false;
            }

            AaSysComSend(msg, osWaitForever);
        }
    }
}


static void RunLedThreadFork(void const *argument)
{
    (void) argument;
    void* msg;

    AaSysLogPrintF(LOGLEVEL_INF, SystemStartup, "%s started", __FUNCTION__);

    if(SysCom_RunLed2 != AaSysComRegister(SysCom_RunLed2, MAKECHAR(RunLedThreadFork)"Queue", 8))
    {
        AaSysLogPrintF( LOGLEVEL_ERR, SystemStartup, "%s %d: AaSysComRegister failed", __FUNCTION__, __LINE__);
    }
    else 
    {
        AaSysLogPrintF( LOGLEVEL_INF, SystemStartup, "%s %d: AaSysComRegister success", __FUNCTION__, __LINE__);
    }

    for(;;)
    {
        msg = AaSysComReceiveHandler(SysCom_RunLed2, osWaitForever);
        if(msg != NULL)
        {
            if(AaSysComGetReceiver(msg) == SysCom_RunLed2)
            {
                SMsgHeader* header = (SMsgHeader*)msg;
                switch(header->msg_id)
                {
                    case API_MESSAGE_ID_LED_INDICATION:
                        HandleLedIndicationMsg(msg);
                        break;
                    default: 
                        AaSysLogPrintF( LOGLEVEL_ERR, SystemStartup, "%s %d: get unknow msg id", 
                                        __FUNCTION__, __LINE__, header->msg_id);
                        break;
                }
            }
            else 
            {
                AaSysLogPrintF( LOGLEVEL_ERR, SystemStartup, "%s %d: unknow msg wrong receiver", __FUNCTION__, __LINE__);
            }

            AaSysComDestory(msg);
        }
    }
}


static void HandleLedIndicationMsg(void* msg)
{
    SLedIndication* pl = AaSysComGetPayload(msg);
    AaSysLogPrintF( LOGLEVEL_DBG, SystemStartup, "%s %d: get led status %s", 
                    __FUNCTION__, __LINE__, pl->led_status ? "true" : "false");
}

/**
  * @brief  start led from system level
  * @param  none
  * @retval None
  */
u8 StartRunLedTask()
{
    LedDeviceInit();
    

    osThreadDef(RunLed, RunLedThread, osPriorityNormal, 0, configMINIMAL_STACK_SIZE);
    _runled_id = AaThreadCreateStartup(osThread(RunLed), NULL);
    if(_runled_id == NULL) {
        AaSysLogPrintF( LOGLEVEL_ERR, SystemStartup, "%s %d: RunLedThread initialize failed",
                        __FUNCTION__, __LINE__);
        return 1;
    }
    AaSysLogPrintF(LOGLEVEL_INF, SystemStartup, "create RunLedThread success");


    osThreadDef(RunLedFork, RunLedThreadFork, osPriorityNormal, 0, configMINIMAL_STACK_SIZE);
    _runled_id_fork = AaThreadCreateStartup(osThread(RunLedFork), NULL);
    if(_runled_id_fork == NULL) {
        AaSysLogPrintF( LOGLEVEL_ERR, SystemStartup, "%s %d: RunLedThreadFork initialize failed",
                        __FUNCTION__, __LINE__);
        return 1;
    }
    AaSysLogPrintF(LOGLEVEL_INF, SystemStartup, "create RunLedThreadFork success");


    return 0;
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





