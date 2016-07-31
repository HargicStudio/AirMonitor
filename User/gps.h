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
#ifndef __GPS_H
#define __GPS_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "AaInclude.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
    
#define GPS_SERIAL_RX_BUFFER_NUM        (2)
#define GPS_SERIAL_RX_BUFFER_SIZE       (256)

typedef struct GPS_BUF_t
{
    u8 bufData[GPS_SERIAL_RX_BUFFER_NUM][GPS_SERIAL_RX_BUFFER_SIZE];
    u8 *curWriteBuf;
    u8 *curReadBuf;
    u16 curWriteNum;
    u16 curReadNum;
    u8 switchFlag;
    
}GPS_BUF_t;

u8 StartGpsTask();
void GpsWaitForSendCplt();
void GpsRecvDataFromISR(UART_HandleTypeDef *huart);


#endif /* __GPS_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
