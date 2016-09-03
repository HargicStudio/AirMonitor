/**
  ******************************************************************************
  * @file    FatFs/FatFs_uSD_RTOS/Inc/main.h 
  * @author  MCD Application Team
  * @version V1.3.3
  * @date    29-January-2016
  * @brief   Header for main.c module
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2016 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __Gsm_h
#define __Gsm_h

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "AaInclude.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/


u8 StartGsmTask();
void GsmWaitForSendCplt();
void GsmRecvDataFromISR(UART_HandleTypeDef *huart);
    
bool ProcessAtResponse(u8 *buf, u16 len);
bool SendDataToServer(void);
bool SendResponseToServer(void);
void ReceiveNormalData(u8 data);
void ReceiveTransparentData(u8 data);
bool SendRecallDataToServer(void);
void GsmSendToUSART(u8 * data, u16 len);



#endif /* __Gsm_h */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
