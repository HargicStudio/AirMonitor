
/***

History:
[2016-07-13 Ted]: Create

*/

#ifndef __AaShell_H
#define __AaShell_H

#ifdef __cplusplus
 extern "C" {
#endif 



#include "stm32f4xx_hal.h"



u8 AaShellCEInit();
void AaShellRecvDataFromISR(UART_HandleTypeDef *huart);



#ifdef __cplusplus
}
#endif

#endif // __AaShell_H

// end of file
