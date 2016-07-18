/***

History:
[2016-07-13 Ted]: Create

*/


#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>
#include "AaMem.h"


/** Description of the macro */  
#define PRINT_STRING_MAX_LENGTH             250

#define PRINT_FEATURE_STRING_MAX_LENGTH     8



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

    len += sprintf(str + len, "%02x %dT %s/%s/%s ", idx, osKernelSysTick(), AaSysLogGetLevelString(level), feature_str, thread_str);

    va_start(args, fmt);
    len += vsprintf(str + len, fmt, args);
    va_end(args);

    len += sprintf(str + len, "\r\n");

    _aasyslog_mng.processPrint_callback(str, len);    

    AaMemFree(str);
}



// end of file
