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

#define GSM_SERIAL_RX_BUFFER_NUM        (2)
#define GSM_SERIAL_RX_BUFFER_SIZE       (256)
    
typedef struct GSM_BUF_t
{
    u8 bufData[GSM_SERIAL_RX_BUFFER_NUM][GSM_SERIAL_RX_BUFFER_SIZE];
    u8 *curWriteBuf;
    u8 *curReadBuf;
    u16 curWriteNum;
    u16 curReadNum;
    u8 switchFlag;
    
}GSM_BUF_t;

u8 StartGsmTask();
void GsmWaitForSendCplt();
void GsmRecvDataFromISR(UART_HandleTypeDef *huart);
    
bool ProcessAtResponse(u8 *buf, u16 len);
bool SendDataToServer(void);
bool SendResponseToServer(void);



#endif /* __Gsm_h */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
