
/***

History:
[2016-07-17 Ted]: Create

*/

#ifndef _AAaSysLogInterface_h
#define _AAaSysLogInterface_h

#ifdef __cplusplus
 extern "C" {
#endif 


#include "cmsis_os.h"
#include "glo_def.h"
#include "AaInclude.h"



/** 
 * debug log level
 * The detail description. 
 */  
typedef enum ELogLevel_t {
    LOGLEVEL_ALL = 0,
    LOGLEVEL_DBG = 1,
    LOGLEVEL_INF = 2,
    LOGLEVEL_WRN = 3,
    LOGLEVEL_ERR = 4,
} ELogLevel;



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
char* AaSysLogGetLevelString(ELogLevel level);

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
ELogLevel AaSysLogGetInputLevel();

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
void AaSysLogPrintD(const char* fmt, ...);

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
void AaSysLogPrintF(ELogLevel level, char* feature_id, const char* fmt, ...);

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
void AaSysLogPrintM(char* feature_id, const char* data, u32 data_len);


#ifdef __cplusplus
}
#endif

#endif // _AAaSysLogInterface_h

// end of file
