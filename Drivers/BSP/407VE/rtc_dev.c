

/***

History:
[2016-08-21 Ted]: Create

*/


#include "rtc_dev.h"
#include <stdio.h>
#include "cmsis_os.h"



#define RTCGetWeekday(year, yday)       ((year-1+(year-1)/4-(year-1)/100+(year-1)/400+yday)%7)

/* Defines related to Clock configuration */    
#define RTC_ASYNCH_PREDIV  0x7F   /* LSE as RTC clock */
#define RTC_SYNCH_PREDIV   0x00FF /* LSE as RTC clock */


#define RTC_DRIVER_MAGICWORD        0x32F2


/* RTC handler declaration */
RTC_HandleTypeDef RtcHandle;



/** 
 * This is a brief description. 
 * This is a detail description. 
 * @param[in]   inArgName input argument description. 
 * @param[out]  outArgName output argument description.  
 * @retval  
 * @retval  
 * @par 
 *      
 * @par 
 *      
 * @par History
 *      2016-08-21 Huang Shengda
 */  
u8 RtcInit()
{
    /*##-1- Configure the RTC peripheral #######################################*/
    RtcHandle.Instance = RTC;

    /* Configure RTC prescaler and RTC data registers */
    /* RTC configured as follow:
      - Hour Format    = Format 24
      - Asynch Prediv  = Value according to source clock
      - Synch Prediv   = Value according to source clock
      - OutPut         = Output Disable
      - OutPutPolarity = High Polarity
      - OutPutType     = Open Drain */ 
    RtcHandle.Init.HourFormat = RTC_HOURFORMAT_24;
    RtcHandle.Init.AsynchPrediv = RTC_ASYNCH_PREDIV;
    RtcHandle.Init.SynchPrediv = RTC_SYNCH_PREDIV;
    RtcHandle.Init.OutPut = RTC_OUTPUT_DISABLE;
    RtcHandle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
    RtcHandle.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;

    if(HAL_RTC_Init(&RtcHandle) != HAL_OK) {
        /* Initialization Error */
        return 1;
    }

    /*##-2- Check if Data stored in BackUp register0: No Need to reconfigure RTC#*/
    /* Read the BackUp Register 0 Data */
    if(HAL_RTCEx_BKUPRead(&RtcHandle, RTC_BKP_DR0) != RTC_DRIVER_MAGICWORD) {  
        /* Configure RTC Calendar */
        RTC_CalendarReset();
    } else {
        /* Check if the Power On Reset flag is set */  
        if(__HAL_RCC_GET_FLAG(RCC_FLAG_PORRST) != RESET) {
            
        }
        /* Check if Pin Reset flag is set */
        if(__HAL_RCC_GET_FLAG(RCC_FLAG_PINRST) != RESET) {
            
        }

        /* Clear Reset Flag */
        __HAL_RCC_CLEAR_RESET_FLAGS();
    }

    return 0;
}


/**
  * @brief RTC MSP Initialization 
  *        This function configures the hardware resources used in this example
  * @param hrtc: RTC handle pointer
  * 
  * @note  Care must be taken when HAL_RCCEx_PeriphCLKConfig() is used to select 
  *        the RTC clock source; in this case the Backup domain will be reset in  
  *        order to modify the RTC Clock source, as consequence RTC registers (including 
  *        the backup registers) and RCC_BDCR register are set to their reset values.
  *             
  * @retval None
  */
void HAL_RTC_MspInit(RTC_HandleTypeDef *hrtc)
{
  RCC_OscInitTypeDef        RCC_OscInitStruct;
  RCC_PeriphCLKInitTypeDef  PeriphClkInitStruct;
  
  /*##-1- Configue LSE as RTC clock soucre ###################################*/ 
  RCC_OscInitStruct.OscillatorType =  RCC_OSCILLATORTYPE_LSI | RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_OFF;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  { 
    // Error_Handler();
  }
  
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  if(HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  { 
    // Error_Handler();
  }
  
  /*##-2- Enable RTC peripheral Clocks #######################################*/ 
  /* Enable RTC Clock */ 
  __HAL_RCC_RTC_ENABLE(); 
}

/**
  * @brief RTC MSP De-Initialization 
  *        This function frees the hardware resources used in this example:
  *          - Disable the Peripheral's clock
  * @param hrtc: RTC handle pointer
  * @retval None
  */
void HAL_RTC_MspDeInit(RTC_HandleTypeDef *hrtc)
{
  /*##-1- Reset peripherals ##################################################*/
   __HAL_RCC_RTC_DISABLE();
}

/**
  * @brief  Configure the current time and date.
  * @param  None
  * @retval None
  */
u8 RTC_SetCalendar(const struct tm tblock)
{
    RTC_DateTypeDef sdatestructure;
    RTC_TimeTypeDef stimestructure;

    /*##-1- Configure the Date #################################################*/
    sdatestructure.Year = tblock.tm_year - 1990;
    sdatestructure.Month = tblock.tm_mon;
    sdatestructure.Date = tblock.tm_mday;

    if(tblock.tm_wday != 0)
    {
        sdatestructure.WeekDay = tblock.tm_wday;
    }
    else
    {
        sdatestructure.WeekDay = RTCGetWeekday(tblock.tm_year, tblock.tm_yday);
    }

    if(HAL_RTC_SetDate(&RtcHandle,&sdatestructure,RTC_FORMAT_BIN) != HAL_OK) {
        return 1;
    } 

    /*##-2- Configure the Time #################################################*/
    if(RtcHandle.Init.HourFormat == RTC_HOURFORMAT_24)
    {
        stimestructure.Hours = tblock.tm_hour;
    }
    else 
    {
        if(tblock.tm_hour < 12)
        {
            stimestructure.Hours = tblock.tm_hour;
            stimestructure.TimeFormat = RTC_HOURFORMAT12_AM;
        } 
        else if(tblock.tm_hour >= 12 && tblock.tm_hour < 24)
        {
            stimestructure.Hours = tblock.tm_hour - 12;
            stimestructure.TimeFormat = RTC_HOURFORMAT12_PM;
        }
    }
    stimestructure.Minutes = tblock.tm_min;
    stimestructure.Seconds = tblock.tm_sec;
    stimestructure.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    stimestructure.StoreOperation = RTC_STOREOPERATION_RESET;

    if(HAL_RTC_SetTime(&RtcHandle,&stimestructure,RTC_FORMAT_BIN) != HAL_OK) {
        return 2;
    }

    /*##-3- Writes a data in a RTC Backup data Register0 #######################*/
    HAL_RTCEx_BKUPWrite(&RtcHandle,RTC_BKP_DR0, RTC_DRIVER_MAGICWORD);  

    return 0;
}

u8 RTC_SetTime(time_t tp)
{
    struct tm *p_rtc = localtime(&tp);

    RTC_DateTypeDef sdatestructure;
    RTC_TimeTypeDef stimestructure;

    /*##-1- Configure the Date #################################################*/
    sdatestructure.Year = p_rtc->tm_year - 90;
    sdatestructure.Month = p_rtc->tm_mon + 1;
    sdatestructure.Date = p_rtc->tm_mday;
    sdatestructure.WeekDay = RTCGetWeekday(p_rtc->tm_year + 1990, p_rtc->tm_yday);;

    if(HAL_RTC_SetDate(&RtcHandle,&sdatestructure,RTC_FORMAT_BIN) != HAL_OK) {
        return 1;
    } 

    /*##-2- Configure the Time #################################################*/
    if(RtcHandle.Init.HourFormat == RTC_HOURFORMAT_24)
    {
        stimestructure.Hours = p_rtc->tm_hour;
    }
    else 
    {
        if(p_rtc->tm_hour < 12)
        {
            stimestructure.Hours = p_rtc->tm_hour;
            stimestructure.TimeFormat = RTC_HOURFORMAT12_AM;
        } 
        else if(p_rtc->tm_hour >= 12 && p_rtc->tm_hour < 24)
        {
            stimestructure.Hours = p_rtc->tm_hour - 12;
            stimestructure.TimeFormat = RTC_HOURFORMAT12_PM;
        }
    }
    stimestructure.Minutes = p_rtc->tm_min;
    stimestructure.Seconds = p_rtc->tm_sec;
    stimestructure.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    stimestructure.StoreOperation = RTC_STOREOPERATION_RESET;

    if(HAL_RTC_SetTime(&RtcHandle,&stimestructure,RTC_FORMAT_BIN) != HAL_OK) {
        return 2;
    }

    /*##-3- Writes a data in a RTC Backup data Register0 #######################*/
    HAL_RTCEx_BKUPWrite(&RtcHandle,RTC_BKP_DR0, RTC_DRIVER_MAGICWORD);  

    return 0;
}

/**
  * @brief  Display the current time and date.
  * @param  showtime : pointer to buffer
  * @param  showdate : pointer to buffer
  * @retval None
  */
void RTC_GetCalendar(struct tm *tblock)
{
    RTC_DateTypeDef sdatestructureget;
    RTC_TimeTypeDef stimestructureget;
  
    /* Get the RTC current Time */
    HAL_RTC_GetTime(&RtcHandle, &stimestructureget, RTC_FORMAT_BIN);
    /* Get the RTC current Date */
    HAL_RTC_GetDate(&RtcHandle, &sdatestructureget, RTC_FORMAT_BIN);

    tblock->tm_year = sdatestructureget.Year + 1990;
    tblock->tm_mon = sdatestructureget.Month;
    tblock->tm_mday = sdatestructureget.Date;
    tblock->tm_wday = sdatestructureget.WeekDay;
    tblock->tm_hour = stimestructureget.Hours;
    tblock->tm_min = stimestructureget.Minutes;
    tblock->tm_sec = stimestructureget.Seconds;
} 


void RTC_CalendarReset()
{
    struct tm rtc;

    rtc.tm_isdst = -1;
    rtc.tm_year = 1990;
    rtc.tm_yday = 1;
    rtc.tm_mon = 1;
    rtc.tm_mday = 1;
    rtc.tm_wday = RTCGetWeekday(rtc.tm_year, rtc.tm_yday);
    rtc.tm_hour = 0;
    rtc.tm_min = 0;
    rtc.tm_sec = 0;

    RTC_SetCalendar(rtc);
}


// end of file



