

/***

History:
[2016-05-21 Ted]: Create

*/


#include "dnc3_dev.h"
#include "dn7c3.h"
#include <stdio.h>
#include <math.h>
#include "cmsis_os.h"
#include "AaInclude.h"
#include "feature_name.h"


/** RunLedThread handler id */  
osThreadId _dn7c3_id;



static void Dn7c3Thread(void const *argument);


/**
  * @brief  run led thread
  * @param  thread not used
  * @retval None
  */
static void Dn7c3Thread(void const *argument)
{
    (void) argument;
    u16 adc = 0;
    double volt;

    AaSysLogPrintF(LOGLEVEL_INF, FeatureDn7c3, "%s started", __FUNCTION__);

    Dnc3Start();

    for (;;)
    {
        osDelay(1000);
        adc = AdcGet();
        volt = 2.5/pow(2, 12)*adc*2;
        AaSysLogPrintF(LOGLEVEL_DBG, FeatureDn7c3, "%s: get adc 0x%x, volt %lf", __FUNCTION__, adc, volt);
    }
}

/**
  * @brief  start led from system level
  * @param  none
  * @retval None
  */
u8 StartDn7c3Task()
{
    if (0 != Dnc3Init())
    {
        AaSysLogPrintF( LOGLEVEL_ERR, FeatureDn7c3, "%s %d: dnc3 device initialize failed",
                        __FUNCTION__, __LINE__);
        return 1;
    }
    AaSysLogPrintF( LOGLEVEL_INF, FeatureDn7c3, "%s %d: dnc3 device initialize success",
                    __FUNCTION__, __LINE__);
   

    osThreadDef(Dn7c3, Dn7c3Thread, osPriorityNormal, 0, configMINIMAL_STACK_SIZE);
    _dn7c3_id = AaThreadCreateStartup(osThread(Dn7c3), NULL);
    if(_dn7c3_id == NULL) {
        AaSysLogPrintF( LOGLEVEL_ERR, FeatureDn7c3, "%s %d: Dn7c3Thread initialize failed",
                        __FUNCTION__, __LINE__);
        return 2;
    }
    AaSysLogPrintF(LOGLEVEL_INF, FeatureDn7c3, "create Dn7c3Thread success");


    return 0;
}



// end of file





