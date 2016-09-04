
/***

History:
[2016-09-03 Ted]: Create

*/

#ifndef _SYSCOM_CFG_H_
#define _SYSCOM_CFG_H_

#ifdef __cplusplus
 extern "C" {
#endif 


#include "cmsis_os.h"



typedef enum 
{
    SysCom_Unknow = 0,
    SysCom_RunLed1 = 1,
    SysCom_RunLed2,
    SysCom_Auto,
    SysCom_MAX = 1000,
} ESysComID;




#ifdef __cplusplus
}
#endif

#endif // _SYSCOM_CFG_H_

// end of file


