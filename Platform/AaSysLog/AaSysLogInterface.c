/***

History:
[2016-07-13 Ted]: Create

*/


#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>
#include "AaMem.h"
#include "rtc_dev.h"
#include <time.h>


/** Description of the macro */  
#define PRINT_STRING_MAX_LENGTH             250

/** Description of the macro */  
#define PRINT_FEATURE_STRING_MAX_LENGTH     12

/** Description of the macro */  
#define PRINT_MEMORY_NUMBER_PER_LINE        8


/** syslog index, will increase for every package log */
static u8 _aasyslog_index = 0;

/** log level for input filter */
static ELogLevel _aasyslog_input_level = LOGLEVEL_ALL;


extern SAaSysLog _aasyslog_mng;



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
 *      2016-07-17 Huang Shengda
 */  
char* AaSysLogGetLevelString(ELogLevel level)
{    
    char* str_level;

    switch(level) {
        case LOGLEVEL_DBG: str_level = "DBG"; break;
        case LOGLEVEL_INF: str_level = "INF"; break;
        case LOGLEVEL_WRN: str_level = "WRN"; break;
        case LOGLEVEL_ERR: str_level = "ERR"; break;
        default: str_level = "Unknow"; break;
    }

    return str_level;
}

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
 *      2016-07-17 Huang Shengda
 */  
ELogLevel AaSysLogGetInputLevel()
{
    return _aasyslog_input_level;
}

/** 
 * This is standard print without any format. 
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
 *      2016-07-17 Huang Shengda
 */  
void AaSysLogPrintD(const char* fmt, ...)
{
    char* str;
    u32 len = 0;
    va_list args;

    str = AaMemMalloc(PRINT_STRING_MAX_LENGTH);

    va_start(args, fmt);
    len = vsprintf(str, fmt, args);
    va_end(args);

    _aasyslog_mng.processPrint_callback(str, len);    

    AaMemFree(str);
}

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
 *      2016-07-17 Huang Shengda
 */  
void AaSysLogPrintF(ELogLevel level, char* feature_id, const char* fmt, ...)
{
    u32 len = 0;
    char feature_str[PRINT_FEATURE_STRING_MAX_LENGTH] = {0};
    char thread_str[PRINT_FEATURE_STRING_MAX_LENGTH] = {0};
    va_list args;
    u8 idx = _aasyslog_index++;

    if (level < AaSysLogGetInputLevel()) {
        return ;
    }

    char* str = AaMemMalloc(PRINT_STRING_MAX_LENGTH);
    if(str == NULL) {
        return ;
    }

    if(feature_id != NULL)
    {
        if(strlen(feature_id) > PRINT_FEATURE_STRING_MAX_LENGTH)
        {
            memcpy(feature_str, feature_id, PRINT_FEATURE_STRING_MAX_LENGTH - 1);
            feature_str[PRINT_FEATURE_STRING_MAX_LENGTH - 1] = '\0';
        } 
        else 
        {
            memcpy(feature_str, feature_id, strlen(feature_id));
        }
    }
    else 
    {
        memcpy(feature_str, "NULL", strlen("NULL"));
    }

    char* trd = AaThreadGetName(osThreadGetId());
    if(trd != NULL)
    {
        if (strlen(trd) > PRINT_FEATURE_STRING_MAX_LENGTH)
        {
            memcpy(thread_str, trd, PRINT_FEATURE_STRING_MAX_LENGTH - 1);
            thread_str[PRINT_FEATURE_STRING_MAX_LENGTH - 1] = '\0';
        }
        else
        {
            memcpy(thread_str, trd, strlen(trd));
        }
    }
    else 
    {
        memcpy(thread_str, "NULL", strlen("NULL"));
    }

    struct tm rtc;
    RTC_GetCalendar(&rtc);

    len += sprintf(str + len, "%02x <%04d-%02d-%02d %02d:%02d:%02dT><%dT> -%s- %s/%s ", 
            idx, rtc.tm_year, rtc.tm_mon, rtc.tm_mday, rtc.tm_hour, rtc.tm_min, rtc.tm_sec, 
            osKernelSysTick(), thread_str, AaSysLogGetLevelString(level), feature_str);

    va_start(args, fmt);
    len += vsprintf(str + len, fmt, args);
    va_end(args);

    len += sprintf(str + len, "\r\n");

    _aasyslog_mng.processPrint_callback(str, len);    

    AaMemFree(str);
}

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
 *      2016-07-19 Huang Shengda
 */  
void AaSysLogPrintM(char* feature_id, const char* data, u32 data_len)
{
    u32 len = 0;
    char feature_str[PRINT_FEATURE_STRING_MAX_LENGTH] = {0};
    char thread_str[PRINT_FEATURE_STRING_MAX_LENGTH] = {0};
    u8 idx = _aasyslog_index++;
    u32 data_idx = 0;

    char* str = AaMemMalloc((PRINT_STRING_MAX_LENGTH/5 + PRINT_MEMORY_NUMBER_PER_LINE*3)*(data_len/PRINT_MEMORY_NUMBER_PER_LINE + 1));
    if(str == NULL) {
        return ;
    }

    if(strlen(feature_id) > PRINT_FEATURE_STRING_MAX_LENGTH) {
        memcpy(feature_str, feature_id, PRINT_FEATURE_STRING_MAX_LENGTH - 1);
        feature_str[PRINT_FEATURE_STRING_MAX_LENGTH - 1] = '\0';
    } else {
        memcpy(feature_str, feature_id, strlen(feature_id));
    }

    char* trd = AaThreadGetName(osThreadGetId());

    if (strlen(trd) > PRINT_FEATURE_STRING_MAX_LENGTH) {
        memcpy(thread_str, trd, PRINT_FEATURE_STRING_MAX_LENGTH - 1);
        thread_str[PRINT_FEATURE_STRING_MAX_LENGTH - 1] = '\0';
    } else {
        memcpy(thread_str, trd, strlen(trd));
    }

    struct tm rtc;
    
    do {
        RTC_GetCalendar(&rtc);
        len += sprintf(str + len, "%02x <%04d-%02d-%02d %02d:%02d:%02dT><%dT> -%s- %s/%s/ Address 0x%p: ", 
                idx, rtc.tm_year, rtc.tm_mon, rtc.tm_mday, rtc.tm_hour, rtc.tm_min, rtc.tm_sec, 
                osKernelSysTick(), thread_str, AaSysLogGetLevelString(LOGLEVEL_DBG), feature_str, data + data_idx);
        do {
            len += sprintf(str + len, "%02x ", *(data + data_idx));
            if(++data_idx >= data_len) {
                break;
            }
        } while((data_idx % PRINT_MEMORY_NUMBER_PER_LINE) != 0);
        len += sprintf(str + len, "\r\n");
    } while(data_idx < data_len);

    _aasyslog_mng.processPrint_callback(str, len);    

    AaMemFree(str);
}


// end of file
