

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
#include "dataHandler.h"


/** RunLedThread handler id */  
osThreadId _dn7c3_id;



static void Dn7c3Thread(void const *argument);

void HandleDn7c3(double volt)
{
    double tempIn = GetTempIn()/10.0;
    double tempBase = ConfigGetpm10BaseC()/10.0;
    double baseV = ConfigGetpm10BaseV();      // mv
    double nValue = ConfigGetpm10N()/100.0;
    double curBaseV = 0;
    double n = 0;    // ÎÂ¶È²¹³¥ÏµÊý
    double C = 0;
    double Vo = volt;
    
    if (tempIn >= -10.0 && tempIn < 40.0)
    {
        n = 6.0;
    }
    else if (tempIn >= 40 && tempIn <= 60)
    {
        n = 1.5;
    }
    else
    {
        AaSysLogPrintF(LOGLEVEL_INF, FeatureDn7c3, "Temp abnormal! %f", tempIn);
        return;
    }
    
    curBaseV = baseV - n * (tempIn - tempBase);
    
    C =  nValue * (volt * 1000 - curBaseV);
    
    if (C < 0 || C > 1000)
    {
        /*AaSysLogPrintF(LOGLEVEL_INF, FeatureDn7c3, "Error! C: %lf, BV:%lf, n:%lf,"
                       "CV:%lf, tempI:%lf, Vo: %lf",
                   C, baseV, n, curBaseV, tempIn, volt);*/
        /* Following print will crash
        AaSysLogPrintF(LOGLEVEL_INF, FeatureDn7c3, "Error!  C: %lf, BV:%lf, n:%lf",
                   C, baseV, n);
        AaSysLogPrintF(LOGLEVEL_INF, FeatureDn7c3, "Error!  CV:%lf, tempI:%lf, Vo: %lf",
                   curBaseV, tempIn, volt);*/
        /*
          AaSysLogPrintF(LOGLEVEL_INF, FeatureDn7c3, "Error!  C: %lf, BV:%lf, n:%lf, "
                   "CV:%lf, tempI:%lf, Vo=%lf",
                   C, baseV, n,
                   curBaseV, tempIn, Vo);*/
        return;
    }
    /*   following print will crash
    AaSysLogPrintF(LOGLEVEL_INF, FeatureDn7c3, "Dn7c3 C: %lf, BV:%lf, n:%lf",
                   C, baseV, n);
    AaSysLogPrintF(LOGLEVEL_INF, FeatureDn7c3, "Dn7c3 CV:%lf, tempI:%lf, Vo: %lf",
                   curBaseV, tempIn, volt);*/
    
    AaSysLogPrintF(LOGLEVEL_INF, FeatureDn7c3, "Dn7c3 C: %lf, BV:%lf, n:%lf, "
                   "CV:%lf, tempI:%lf, Vo=%lf",
                   C, baseV, n,
                   curBaseV, tempIn, Vo);
    
    StorePmInfo((u16) C, &g_pm10sharp);
}


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
    u32 interval = 5000;

    AaSysLogPrintF(LOGLEVEL_INF, FeatureDn7c3, "%s started", __FUNCTION__);

    Dnc3Start();

    for (;;)
    {
        osDelay(interval);
        adc = AdcGet();
        volt = 2.5/pow(2, 12)*adc*2;
        AaSysLogPrintF(LOGLEVEL_DBG, FeatureDn7c3, "%s: get adc 0x%x, volt %lf", __FUNCTION__, adc, volt);
        
        HandleDn7c3(volt);
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





