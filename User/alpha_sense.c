

/***

History:
[2016-08-11 Ted]: Create

*/

#include <math.h>
#include "cmsis_os.h"
#include "alpha_sense.h"
#include "alpha_sense_if.h"
#include "ads1222.h"
#include "dataHandler.h"
#include "PingPang.h"
#include "feature_name.h"
#include "dataHandler.h"
#include "gps.h"


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


SSense alpha_co;
// SSense alpha_o3;
// SSense alpha_no2;
// SSense alpha_so2;



static void Ads1222Thread(void const *argument);
static void AlphaSThread(void const *argument);
static void HandleAdsSampleIndicationMsg(void* msg);
static u32 GetAvg(u32* buf, u16 len);

void Ads1222_StoreResult(EASType type, EASElec sel, double volt)
{
     static ADs1222Rst gas[5];
     ADs1222Rst *tmp;
     s32 val = 0;
     
     if (!IsClockSynced())
     {
         return;
     }
     
     tmp = &gas[type];
     
     if (tmp->flag_w == 0 && AS_ELECTRODE_WORKER == sel)
     {
         tmp->w = (u32)(volt + 0.5);
         tmp->flag_w = 1;
     }
     
     if (tmp->flag_a == 0 && AS_ELECTRODE_AUXILIARY == sel)
     {
         tmp->a = (u32)(volt + 0.5);
         tmp->flag_a = 1;
     }
     
     if (tmp->flag_a == 1 && tmp->flag_w == 1)
     {
         tmp->flag_a = 0;
         tmp->flag_w = 0;
         
         val = CalGasVal(tmp->w, tmp->a, type);
         if (val == -1)
         {
             return;
         }
         
         if (val < 0)
           val = 0;
         
         switch(type)
         {
         case AS_TYPE_CO:
           AFX_LOG_P4("CO: w:%d, a: %d, vol:%lf, rst: %d", tmp->w, tmp->a, volt, val);
           if (val > 2000000)
           {
               val = 2000000;
           }
           StoreGasInfo(val, &g_co);
           break;
         case AS_TYPE_NO2:
           AFX_LOG_P4("NO2: w:%d, a: %d, vol:%lf, rst: %d", tmp->w, tmp->a, volt, val);
           if (val > 50000)
           {
               val = 50000;
           }
           StoreGasInfo(val, &g_no2);
           break;
         case AS_TYPE_O3:
           AFX_LOG_P4("O3: w:%d, a: %d, vol:%lf, rst: %d", tmp->w, tmp->a, volt, val);
           if (val > 50000)
           {
               val = 50000;
           }
           StoreGasInfo(val, &g_o3);
           break;
         case AS_TYPE_SO2:
           AFX_LOG_P4("SO2: w:%d, a: %d, vol:%lf, rst: %d", tmp->w, tmp->a, volt, val);
           if (val > 200000)
           {
               val = 200000;
           }
           StoreGasInfo(val, &g_so2);
           break;
         default:
           AFX_LOG_P1("Error Gas Type: %d", type);
           break;
         }
     }
     
}


/**
  * @brief  sample ads1222 data thread
  * @param  thread not used
  * @retval None
  */
#if 0
static void Ads1222Thread(void const *argument)
{
    (void) argument;
    u32 sense_data[4];
    u8 channel;
    void* msg;

    AaSysLogPrintF(LOGLEVEL_INF, FeatureAlpha, "%s started", __FUNCTION__);

    // !!! should config the device according to board
    u8 exit_sense = ADS1222_CHIP_A | ADS1222_CHIP_B | ADS1222_CHIP_C | ADS1222_CHIP_D;
    
    Ads1222_SetExistSense(exit_sense);
    AaSysLogPrintF( LOGLEVEL_INF, FeatureAlpha, "%s %d: set exist sense 0x%02x", 
                    __FUNCTION__, __LINE__, exit_sense);

    if(SysCom_Ads1222 != AaSysComRegister(SysCom_Ads1222, MAKECHAR(Ads1222Thread)"Queue", 8))
    {
        AaSysLogPrintF( LOGLEVEL_ERR, FeatureAlpha, "%s %d: AaSysComRegister failed", __FUNCTION__, __LINE__);
    }
    else 
    {
        AaSysLogPrintF( LOGLEVEL_INF, FeatureAlpha, "%s %d: AaSysComRegister success", __FUNCTION__, __LINE__);
    }
    
    AaSysLogPrintF(LOGLEVEL_INF, FeatureAlpha, "%s startup success", __FUNCTION__);

    for (;;)
    {
        osDelay(5000);
        Ads1222_EnableExti(exit_sense);
        osSemaphoreWait(_ads1222_convcplt_sem_id, osWaitForever);

        channel = Ads1222_GetChannel();
        if(Ads1222Err_NoErr != ADS1222_AdRead(&sense_data[0], &sense_data[1], &sense_data[2], &sense_data[3]))
        {
            continue;
        }

        if(IsChipExist(exit_sense, ADS1222_CHIP_A))
        {
            msg = AaSysComCreate(API_MESSAGE_ID_AIR_SAMPLE_INDICATION, SysCom_Ads1222, SysCom_AlphaS, sizeof(SAirSamp));
            if(msg != NULL)
            {
                SAirSamp* pl = AaSysComGetPayload(msg);
                if(pl != NULL)
                {
                    pl->chan = AS_TYPE_CO;
                    pl->elec = (channel == ADS1222_CH0 ? AS_ELECTRODE_WORKER : AS_ELECTRODE_AUXILIARY);
                    pl->ad_samp = sense_data[0];
                    pl->volt = 5/(pow(2, 23) - 1) * sense_data[0] * 2 * 1000;
                    
                    Ads1222_StoreResult(pl->chan, pl->elec, pl->volt);
                }

                AaSysComSend(msg, osWaitForever);
            }
        }

        if(IsChipExist(exit_sense, ADS1222_CHIP_B))
        {
            msg = AaSysComCreate(API_MESSAGE_ID_AIR_SAMPLE_INDICATION, SysCom_Ads1222, SysCom_AlphaS, sizeof(SAirSamp));
            if(msg != NULL)
            {
                SAirSamp* pl = AaSysComGetPayload(msg);
                if(pl != NULL)
                {
                    pl->chan = AS_TYPE_NO2;
                    pl->elec = (channel == ADS1222_CH0 ? AS_ELECTRODE_WORKER : AS_ELECTRODE_AUXILIARY);
                    pl->ad_samp = sense_data[1];
                    pl->volt = 5/(pow(2, 23) - 1) * sense_data[1] * 2  * 1000;
                    
                    Ads1222_StoreResult(pl->chan, pl->elec, pl->volt);
                }

                AaSysComSend(msg, osWaitForever);
            }
        }

        if(IsChipExist(exit_sense, ADS1222_CHIP_C))
        {
            msg = AaSysComCreate(API_MESSAGE_ID_AIR_SAMPLE_INDICATION, SysCom_Ads1222, SysCom_AlphaS, sizeof(SAirSamp));
            if(msg != NULL)
            {
                SAirSamp* pl = AaSysComGetPayload(msg);
                if(pl != NULL)
                {
                    pl->chan = AS_TYPE_O3;
                    pl->elec = (channel == ADS1222_CH0 ? AS_ELECTRODE_WORKER : AS_ELECTRODE_AUXILIARY);
                    pl->ad_samp = sense_data[2];
                    pl->volt = 5/(pow(2, 23) - 1) * sense_data[2] * 2  * 1000;
                    
                    Ads1222_StoreResult(pl->chan, pl->elec, pl->volt);
                }

                AaSysComSend(msg, osWaitForever);
            }
        }

        if(IsChipExist(exit_sense, ADS1222_CHIP_D))
        {
            msg = AaSysComCreate(API_MESSAGE_ID_AIR_SAMPLE_INDICATION, SysCom_Ads1222, SysCom_AlphaS, sizeof(SAirSamp));
            if(msg != NULL)
            {
                SAirSamp* pl = AaSysComGetPayload(msg);
                if(pl != NULL)
                {
                    pl->chan = AS_TYPE_SO2;
                    pl->elec = (channel == ADS1222_CH0 ? AS_ELECTRODE_WORKER : AS_ELECTRODE_AUXILIARY);
                    pl->ad_samp = sense_data[3];
                    pl->volt = 5/(pow(2, 23) - 1) * sense_data[3] * 2  * 1000;
                    
                    Ads1222_StoreResult(pl->chan, pl->elec, pl->volt);
                }

                AaSysComSend(msg, osWaitForever);
            }
        }
        
        Ads1222_ExchangeChannel();
    }
}
#endif

static void Ads1222Thread(void const *argument)
{
    (void) argument;
    u32 sense_data[4];
    u8 channel;
    //void* msg;
    SAirSamp pl = {0};
    u32 interval = 20000;
    u8 sel = 0;

    AaSysLogPrintF(LOGLEVEL_INF, FeatureAlpha, "%s started", __FUNCTION__);

    // !!! should config the device according to board
    u8 exit_sense = ADS1222_CHIP_A | ADS1222_CHIP_B | ADS1222_CHIP_C | ADS1222_CHIP_D;
    
    Ads1222_SetExistSense(exit_sense);
    AaSysLogPrintF( LOGLEVEL_INF, FeatureAlpha, "%s %d: set exist sense 0x%02x", 
                    __FUNCTION__, __LINE__, exit_sense);

    if(SysCom_Ads1222 != AaSysComRegister(SysCom_Ads1222, MAKECHAR(Ads1222Thread)"Queue", 8))
    {
        AaSysLogPrintF( LOGLEVEL_ERR, FeatureAlpha, "%s %d: AaSysComRegister failed", __FUNCTION__, __LINE__);
    }
    else 
    {
        AaSysLogPrintF( LOGLEVEL_INF, FeatureAlpha, "%s %d: AaSysComRegister success", __FUNCTION__, __LINE__);
    }
    
    AaSysLogPrintF(LOGLEVEL_INF, FeatureAlpha, "%s startup success", __FUNCTION__);

    for (;;)
    {
        
        if ( (sel = sel%3) == 0)
        {
           osDelay(interval);
        }
        sel++;
        
        /* 采集原理，W电极和A电极交替，所以在一次完整的采集过程，要尽量快，然后才开始等待间隔 */
        /* 采集间隔默认20s */
        
        Ads1222_EnableExti(exit_sense);
        osSemaphoreWait(_ads1222_convcplt_sem_id, osWaitForever);

        channel = Ads1222_GetChannel();
        if(Ads1222Err_NoErr != ADS1222_AdRead(&sense_data[0], &sense_data[1], &sense_data[2], &sense_data[3]))
        {
            continue;
        }

        if(IsChipExist(exit_sense, ADS1222_CHIP_A))
        {
            pl.chan = AS_TYPE_CO;
            pl.elec = (channel == ADS1222_CH0 ? AS_ELECTRODE_WORKER : AS_ELECTRODE_AUXILIARY);
            pl.ad_samp = sense_data[0];
            pl.volt = 5/(pow(2, 23) - 1) * sense_data[0] * 2 * 1000;
            
            Ads1222_StoreResult(pl.chan, pl.elec, pl.volt);

        }

        if(IsChipExist(exit_sense, ADS1222_CHIP_B))
        {
            pl.chan = AS_TYPE_NO2;
            pl.elec = (channel == ADS1222_CH0 ? AS_ELECTRODE_WORKER : AS_ELECTRODE_AUXILIARY);
            pl.ad_samp = sense_data[1];
            pl.volt = 5/(pow(2, 23) - 1) * sense_data[1] * 2  * 1000;
            
            Ads1222_StoreResult(pl.chan, pl.elec, pl.volt);
        }

        if(IsChipExist(exit_sense, ADS1222_CHIP_C))
        {
            pl.chan = AS_TYPE_O3;
            pl.elec = (channel == ADS1222_CH0 ? AS_ELECTRODE_WORKER : AS_ELECTRODE_AUXILIARY);
            pl.ad_samp = sense_data[2];
            pl.volt = 5/(pow(2, 23) - 1) * sense_data[2] * 2  * 1000;
            
            Ads1222_StoreResult(pl.chan, pl.elec, pl.volt);
        }

        if(IsChipExist(exit_sense, ADS1222_CHIP_D))
        {
            pl.chan = AS_TYPE_SO2;
            pl.elec = (channel == ADS1222_CH0 ? AS_ELECTRODE_WORKER : AS_ELECTRODE_AUXILIARY);
            pl.ad_samp = sense_data[3];
            pl.volt = 5/(pow(2, 23) - 1) * sense_data[3] * 2  * 1000;
            
            Ads1222_StoreResult(pl.chan, pl.elec, pl.volt);
        }
        
        Ads1222_ExchangeChannel();
    }
}

void Ads1222_ConvComplete()
{
    osSemaphoreRelease(_ads1222_convcplt_sem_id);
}

#if 0
static void AlphaSThread(void const *argument)
{
    (void) argument;
    void* msg;

    AaSysLogPrintF(LOGLEVEL_INF, FeatureAlpha, "%s started", __FUNCTION__);
    
    if(SysCom_AlphaS != AaSysComRegister(SysCom_AlphaS, MAKECHAR(AlphaSThread)"Queue", 16))
    {
        AaSysLogPrintF( LOGLEVEL_ERR, FeatureAlpha, "%s %d: AaSysComRegister failed", __FUNCTION__, __LINE__);
    }
    else 
    {
        AaSysLogPrintF( LOGLEVEL_INF, FeatureAlpha, "%s %d: AaSysComRegister success", __FUNCTION__, __LINE__);
    }

    AaSysLogPrintF(LOGLEVEL_INF, FeatureAlpha, "%s startup success", __FUNCTION__);

    for(;;)
    {
        msg = AaSysComReceiveHandler(SysCom_AlphaS, osWaitForever);
        if(msg != NULL)
        {
            if(AaSysComGetReceiver(msg) == SysCom_AlphaS)
            {
                SMsgHeader* header = (SMsgHeader*)msg;
                switch(header->msg_id)
                {
                    case API_MESSAGE_ID_AIR_SAMPLE_INDICATION:
                        HandleAdsSampleIndicationMsg(msg);
                        break;
                    default: 
                        AaSysLogPrintF( LOGLEVEL_ERR, FeatureAlpha, "%s %d: get unknow msg id", 
                                        __FUNCTION__, __LINE__, header->msg_id);
                        break;
                }
            }
            else 
            {
                AaSysLogPrintF( LOGLEVEL_ERR, FeatureAlpha, "%s %d: unknow msg wrong receiver", __FUNCTION__, __LINE__);
            }

            AaSysComDestory(msg);
        }
    }
}
#endif

static void HandleAdsSampleIndicationMsg(void* msg)
{
    SAirSamp* pl = AaSysComGetPayload(msg);

    if (pl->chan == AS_TYPE_CO)
    {
        if (pl->elec == AS_ELECTRODE_WORKER)
        {
            AaSysLogPrintF( LOGLEVEL_DBG, FeatureAlpha, "get CO worker voltagte %lf sample 0x%08x", 
                            pl->volt, pl->ad_samp);
        }
        else if (pl->elec == AS_ELECTRODE_AUXILIARY)
        {
            AaSysLogPrintF( LOGLEVEL_DBG, FeatureAlpha, "get CO auxiliary voltagte %lf sample 0x%08x", 
                            pl->volt, pl->ad_samp);
        }
    }
    else if (pl->chan == AS_TYPE_NO2)
    {
        if (pl->elec == AS_ELECTRODE_WORKER)
        {
            AaSysLogPrintF( LOGLEVEL_DBG, FeatureAlpha, "get NO2 worker voltagte %lf sample 0x%08x", 
                            pl->volt, pl->ad_samp);
        }
        else if (pl->elec == AS_ELECTRODE_AUXILIARY)
        {
            AaSysLogPrintF( LOGLEVEL_DBG, FeatureAlpha, "get NO2 auxiliary voltagte %lf sample 0x%08x", 
                            pl->volt, pl->ad_samp);
        }
    }
    else if (pl->chan == AS_TYPE_O3)
    {
        if (pl->elec == AS_ELECTRODE_WORKER)
        {
            AaSysLogPrintF( LOGLEVEL_DBG, FeatureAlpha, "get O3 worker voltagte %lf sample 0x%08x", 
                            pl->volt, pl->ad_samp);
        }
        else if (pl->elec == AS_ELECTRODE_AUXILIARY)
        {
            AaSysLogPrintF( LOGLEVEL_DBG, FeatureAlpha, "get O3 auxiliary voltagte %lf sample 0x%08x", 
                            pl->volt, pl->ad_samp);
        }
    }
    else if (pl->chan == AS_TYPE_SO2)
    {
        if (pl->elec == AS_ELECTRODE_WORKER)
        {
            AaSysLogPrintF( LOGLEVEL_DBG, FeatureAlpha, "get SO2 worker voltagte %lf sample 0x%08x", 
                            pl->volt, pl->ad_samp);
        }
        else if (pl->elec == AS_ELECTRODE_AUXILIARY)
        {
            AaSysLogPrintF( LOGLEVEL_DBG, FeatureAlpha, "get SO2 auxiliary voltagte %lf sample 0x%08x", 
                            pl->volt, pl->ad_samp);
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
    osSemaphoreWait(_ads1222_convcplt_sem_id, osWaitForever);
    AaSysLogPrintF(LOGLEVEL_INF, FeatureAlpha, "create ads1222_convcplt_sem success");


    osThreadDef(Ads1222, Ads1222Thread, osPriorityNormal, 0, configMINIMAL_STACK_SIZE*4);
    _ads1222_id = AaThreadCreateStartup(osThread(Ads1222), NULL);
    if(_ads1222_id == NULL) {
        AaSysLogPrintF(LOGLEVEL_ERR, FeatureAlpha, "%s %d: Ads1222Thread initialize failed",
                __FUNCTION__, __LINE__);
        return 3;
    }
    AaSysLogPrintF(LOGLEVEL_INF, FeatureAlpha, "create Ads1222Thread success");

#if 0
    osThreadDef(AlphaS, AlphaSThread, osPriorityNormal, 0, configMINIMAL_STACK_SIZE*4);
    _alphasense_id = AaThreadCreateStartup(osThread(AlphaS), NULL);
    if(_alphasense_id == NULL) {
        AaSysLogPrintF(LOGLEVEL_ERR, FeatureAlpha, "%s %d: AlphaSThread initialize failed",
                __FUNCTION__, __LINE__);
        return 4;
    }
    AaSysLogPrintF(LOGLEVEL_INF, FeatureAlpha, "create AlphaSThread success");
#endif

    return 0;
}




// end of file





