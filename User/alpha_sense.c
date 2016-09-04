

/***

History:
[2016-08-11 Ted]: Create

*/

#include <math.h>
#include "cmsis_os.h"
#include "alpha_sense.h"
#include "ads1222.h"
#include "dataHandler.h"
#include "PingPang.h"


#define ALPHASENSE_SamplePerChannel     PINGPANG_BUFFER_DEEP




typedef struct Sense_t {
    SPingpang   ppbuf;
    u32         adc_avg_worker;
    u32         adc_avg_auxi;
} SSense;


/** Ads1222Thread handler id */  
osThreadId _ads1222_id;

/** Ads1222Thread handler id */  
osThreadId _alphasense_id;

/** signal for DMA/IT that data send complete */  
static osSemaphoreDef(ads1222_convcplt_sem);
osSemaphoreId _ads1222_convcplt_sem_id;

/** signal for alpha that data collecting complete */  
static osSemaphoreDef(alpha_sem);
osSemaphoreId _alpha_sem_id;


SSense alpha_co;
// SSense alpha_o3;
// SSense alpha_no2;
// SSense alpha_so2;

u8 g_chan = 0;


u32 buf_worker[ALPHASENSE_SamplePerChannel];
u32 buf_auxi[ALPHASENSE_SamplePerChannel];



static void Ads1222Thread(void const *argument);
static void AlphaSThread(void const *argument);
static u32 GetAvg(u32* buf, u16 len);


/**
  * @brief  sample ads1222 data thread
  * @param  thread not used
  * @retval None
  */
static void Ads1222Thread(void const *argument)
{
    (void) argument;
    u32 worker;
    u32 auxi;
    double volt;
    u16 cnt = 0;

    PingPangInit(&alpha_co.ppbuf);
    alpha_co.adc_avg_worker = 0;
    alpha_co.adc_avg_auxi = 0;

    AaSysLogPrintF(LOGLEVEL_INF, FeatureAlpha, "Ads1222Thread startup success");

    for (;;)
    {
        Ads1222_EnableExti();
        osSemaphoreWait(_ads1222_convcplt_sem_id, osWaitForever);

        // PingPangPut(&alpha_co.ppbuf, ADS1222_AdRead(ADS1222_CHIP_A));

        if(Ads1222_GetChannel() == ADS1222_CH0) {
            buf_worker[cnt] = ADS1222_AdRead(ADS1222_CHIP_A);
        } else {
            buf_auxi[cnt] = ADS1222_AdRead(ADS1222_CHIP_A);
        }
        
        // change channel
        if((++cnt % ALPHASENSE_SamplePerChannel) == 0) {
            cnt = 0;
            // change channel
            // PingPangExchange(&alpha_co.ppbuf);
            if(Ads1222_GetChannel() == ADS1222_CH0) {
                worker = GetAvg(buf_worker, ALPHASENSE_SamplePerChannel);
                volt = 5/(pow(2, 23) - 1) * worker * 2;
                // AaSysLogPrintF(LOGLEVEL_DBG, FeatureAlpha, "get worker %d", worker);
                AaSysLogPrintF(LOGLEVEL_DBG, SystemStartup, "get co worker voltagte %lf", volt);

                g_chan = 0;
                Ads1222_SetChannel(ADS1222_CH1);
            } else {
                auxi = GetAvg(buf_auxi, ALPHASENSE_SamplePerChannel);
                volt = 5/(pow(2, 23) - 1) * auxi * 2;
                // AaSysLogPrintF(LOGLEVEL_DBG, FeatureAlpha, "get auxi %d", auxi);
                AaSysLogPrintF(LOGLEVEL_DBG, SystemStartup, "get co auxi voltagte %lf", volt);

                g_chan = 1;
                Ads1222_SetChannel(ADS1222_CH0);
            }
            // send signal that chan0 is ready
            // osSemaphoreRelease(_alpha_sem_id);
        }
    }
}

void Ads1222_ConvComplete()
{
    osSemaphoreRelease(_ads1222_convcplt_sem_id);
}


static void AlphaSThread(void const *argument)
{
    (void) argument;
    u32* data;
    u16 len;
    double volt;

    AaSysLogPrintF(LOGLEVEL_INF, FeatureAlpha, "AlphaSThread startup success");

    for(;;)
    {
        osSemaphoreWait(_alpha_sem_id, osWaitForever);
        if(g_chan == 0) {
            AaSysLogPrintF(LOGLEVEL_DBG, SystemStartup, "worker collect complete");
            PingPandPrint(&alpha_co.ppbuf);
            data = PingPangGet(&alpha_co.ppbuf, &len);
            if(data != NULL) {
                alpha_co.adc_avg_worker = GetAvg(data, len);
                volt = 5/(pow(2, 23) - 1) * alpha_co.adc_avg_worker * 2;
            }
            AaSysLogPrintF(LOGLEVEL_DBG, FeatureAlpha, "get alpha_co.adc_avg_worker %d", alpha_co.adc_avg_worker);
            AaSysLogPrintF(LOGLEVEL_DBG, SystemStartup, "get co worker voltagte %lf", volt);
            PingPandPrint(&alpha_co.ppbuf);

        } else if(g_chan == 1) {
            AaSysLogPrintF(LOGLEVEL_DBG, SystemStartup, "auxi collect complete");
            PingPandPrint(&alpha_co.ppbuf);
            data = PingPangGet(&alpha_co.ppbuf, &len);
            if(data != NULL) {
                alpha_co.adc_avg_auxi = GetAvg(data, len);
                volt = 5/(pow(2, 23) - 1) * alpha_co.adc_avg_auxi * 2;
            }
            AaSysLogPrintF(LOGLEVEL_DBG, FeatureAlpha, "get alpha_co.adc_avg_auxi %d", alpha_co.adc_avg_auxi);
            AaSysLogPrintF(LOGLEVEL_DBG, SystemStartup, "get co auxi voltagte %lf", volt);
            PingPandPrint(&alpha_co.ppbuf);
        }
    }
}


static u32 GetAvg(u32* buf, u16 len)
{
    u16 i;
    u32 ret_val = 0;

    ret_val = *buf;
    for(i=1; i<len; i++) {
        ret_val += *(buf + i);
        ret_val >>= 1;
    }

    return ret_val;
}

/**
  * @brief  start led from system level
  * @param  none
  * @retval None
  */
u8 StartAlphaSenseTask()
{
    ADS1222_Init();


    _ads1222_convcplt_sem_id = osSemaphoreCreate(osSemaphore(ads1222_convcplt_sem), 1);
    if(_ads1222_convcplt_sem_id == NULL) {
        AaSysLogPrintF(LOGLEVEL_ERR, FeatureAlpha, "%s %d: ads1222_convcplt_sem initialize failed",
                __FUNCTION__, __LINE__);
        return 1;
    }
    AaSysLogPrintF(LOGLEVEL_INF, FeatureAlpha, "create ads1222_convcplt_sem success");


    _alpha_sem_id = osSemaphoreCreate(osSemaphore(alpha_sem), 1);
    if(_alpha_sem_id == NULL) {
        AaSysLogPrintF(LOGLEVEL_ERR, FeatureAlpha, "%s %d: alpha_sem initialize failed",
                __FUNCTION__, __LINE__);
        return 2;
    }
    AaSysLogPrintF(LOGLEVEL_INF, FeatureAlpha, "create alpha_sem success");


    osThreadDef(Ads1222, Ads1222Thread, osPriorityNormal, 0, configMINIMAL_STACK_SIZE);
    _ads1222_id = AaThreadCreateStartup(osThread(Ads1222), NULL);
    if(_ads1222_id == NULL) {
        AaSysLogPrintF(LOGLEVEL_ERR, FeatureAlpha, "%s %d: Ads1222Thread initialize failed",
                __FUNCTION__, __LINE__);
        return 3;
    }
    AaSysLogPrintF(LOGLEVEL_INF, FeatureAlpha, "create Ads1222Thread success");


    osThreadDef(AlphaS, AlphaSThread, osPriorityNormal, 0, configMINIMAL_STACK_SIZE);
    _alphasense_id = AaThreadCreateStartup(osThread(AlphaS), NULL);
    if(_alphasense_id == NULL) {
        AaSysLogPrintF(LOGLEVEL_ERR, FeatureAlpha, "%s %d: AlphaSThread initialize failed",
                __FUNCTION__, __LINE__);
        return 4;
    }
    AaSysLogPrintF(LOGLEVEL_INF, FeatureAlpha, "create AlphaSThread success");


    return 0;
}




// end of file





