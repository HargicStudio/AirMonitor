
/***

History:
[2015-09-30 Ted]: Create.
[2016-04-21 Ted]: transplant code to FreeRTOS platform as stdio usart.
[2016-05-21 Ted]: support BipBuffer and DMA for uart tx debug.
[2016-07-17 Ted]: Refactory AaSysLog, support multi PrintX.

*/

#ifndef _AAaSysLogSystem_h
#define _AAaSysLogSystem_h

#ifdef __cplusplus
 extern "C" {
#endif 


#include "cmsis_os.h"
#include "glo_def.h"
#include "AaInclude.h"



/** 
 * The brief description. 
 * The detail description. 
 */  
typedef struct SAaSysLog_t {
    /*!
     *  @brief  When data has stored into Bip buffer, Daemon will get the data from Bip buffer.
     *          This callback will be active only when Bip buffer has constructed.
     */
    void (*processGetBip_callback)(char* addr, u32 len);

    /*!
     *  @brief  Implementation how to hendle data.
     *          Now it can be process by three method: 
     *              (1) print by polling without Bip buffer.
     *                  This method can be used without Bip buffer but need memory heap initialized.
     *              (2) print by polling with store into Bip buffer.
     *                  This method can be used only when Bip buffer has constructed and memory heap initialized.
     *              (3) print by IT/DMA with store into Bip buffer.
     *                  This method only can be used when Bip buffer has constructed, memory heap has initialized and AaSysLog Daemon created.
     */
    void (*processPrint_callback)(char* addr, u32 len);

} SAaSysLog;



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
 *      2016-5-21 Huang Shengda
 */  
u8 AaSysLogCEInit();

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
 *      2016-5-21 Huang Shengda
 */  
u8 AaSysLogCreateDeamon();

/** 
 * This is a brief description. 
 * This AaSysLogInit should depand on AaMemInit
 * @param[in]   inArgName input argument description. 
 * @param[out]  always 0 
 * @retval  
 * @retval  
 * @par 
 *      
 * @par 
 *      
 * @par History
 *      2016-5-21 Huang Shengda
 */ 
u8 AaSysLogGetBipRegister(void(*function)(char*, u32));

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
 *      2016-5-21 Huang Shengda
 */  
u8 AaSysLogSendCplt();

/** 
 * This is a brief description. 
 * This AaSysLogInit should depand on AaMemInit
 * @param[in]   inArgName input argument description. 
 * @param[out]  always 0 
 * @retval  
 * @retval  
 * @par 
 *      
 * @par 
 *      
 * @par History
 *      2016-07-18 Huang Shengda
 */  
void AaSysLogBipBufferList(void);

/** 
 * This is a brief description. 
 * 
 * @param[in]   inArgName input argument description. 
 * @param[out]  always 0 
 * @retval  
 * @retval  
 * @par 
 *      
 * @par 
 *      
 * @par History
 *      2016-05-21 Huang Shengda
        2016-07-17 Huang Shengda
 */  
u8 AaSysLogProcessPrintRegister(void (*function)(char*, u32));

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
 *      2016-5-21 Huang Shengda
 */  
void AaSysLogPrintByPolling(char* str, u32 len);

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
void AaSysLogStoreBipBufferByPolling(char* str, u32 len);

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
void AaSysLogStoreBipBufferNormal(char* str, u32 len);



#ifdef __cplusplus
}
#endif

#endif // _AAaSysLogSystem_h

// end of file
