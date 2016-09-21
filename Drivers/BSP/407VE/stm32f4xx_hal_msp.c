/**
  ******************************************************************************
  * @file    UART/UART_Hyperterminal_IT/Src/stm32f4xx_hal_msp.c
  * @author  MCD Application Team
  * @version V1.2.5
  * @date    29-January-2016
  * @brief   HAL MSP module.    
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2016 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************  
  */ 

/* Includes ------------------------------------------------------------------*/
#include "print_com.h"
#include "gps_dev.h"
#include "gsm_dev.h"
#include "cp15_dev.h"
#include "ads1222.h"

/** @addtogroup STM32F4xx_HAL_Examples
  * @{
  */

/** @defgroup HAL_MSP
  * @brief HAL MSP module.
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup HAL_MSP_Private_Functions
  * @{
  */

/**
  * @brief UART MSP Initialization 
  *        This function configures the hardware resources used in this example: 
  *           - Peripheral's clock enable
  *           - Peripheral's GPIO Configuration  
  *           - NVIC configuration for UART interrupt request enable
  * @param huart: UART handle pointer
  * @retval None
  */
void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{  
    if(huart->Instance == USART_DBG) {
        HAL_UART_DBG_MspInit(huart);
    } else if(huart->Instance == USART_GPS) {
        HAL_UART_GPS_MspInit(huart);
    } else if (huart->Instance == USART_GSM) {
        HAL_UART_GSM_MspInit(huart);
    } else if (huart->Instance == UART_CP15) {
        HAL_UART_CP15_MspInit(huart);
    }
}

/**
  * @brief UART MSP De-Initialization 
  *        This function frees the hardware resources used in this example:
  *          - Disable the Peripheral's clock
  *          - Revert GPIO and NVIC configuration to their default state
  * @param huart: UART handle pointer
  * @retval None
  */
void HAL_UART_MspDeInit(UART_HandleTypeDef *huart)
{
    if(huart->Instance == USART_DBG) {
        HAL_UART_DBG_MspDeInit(huart);
    } else if(huart->Instance == USART_GPS) {
        HAL_UART_GPS_MspDeInit(huart);
    } else if (huart->Instance == USART_GSM) {
        HAL_UART_GSM_MspDeInit(huart);
    } else if (huart->Instance == UART_CP15) {
        HAL_UART_CP15_MspDeInit(huart);
    }
}

/**
  * @brief  Tx Transfer completed callback
  * @param  huart: UART handle. 
  * @note   This example shows a simple way to report end of DMA Tx transfer, and 
  *         you can add your own implementation. 
  * @retval None
  */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart->Instance == USART_DBG) {
        HAL_UART_DBG_TxCpltCallback(huart);
    } else if(huart->Instance == USART_GPS) {
        HAL_UART_GPS_TxCpltCallback(huart);
    } else if (huart->Instance == USART_GSM) {
        HAL_UART_GSM_TxCpltCallback(huart);
    } else if (huart->Instance == UART_CP15) {
        HAL_UART_CP15_TxCpltCallback(huart);
    }
}

/**
  * @brief  Rx Transfer completed callback
  * @param  huart: UART handle
  * @note   This example shows a simple way to report end of DMA Rx transfer, and 
  *         you can add your own implementation.
  * @retval None
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart->Instance == USART_DBG) {
        HAL_UART_DBG_RxCpltCallback(huart);
    } else if(huart->Instance == USART_GPS) {
        HAL_UART_GPS_RxCpltCallback(huart);
    } else if(huart->Instance == USART_GSM) {
        HAL_UART_GSM_RxCpltCallback(huart);
    } else if (huart->Instance == UART_CP15) {
        HAL_UART_CP15_RxCpltCallback(huart);
    }
}

/**
  * @brief  UART error callbacks
  * @param  huart: UART handle
  * @note   This example shows a simple way to report transfer error, and you can
  *         add your own implementation.
  * @retval None
  */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    if(huart->Instance == USART_DBG) {
        HAL_UART_DBG_ErrorCallback(huart);
    } else if(huart->Instance == USART_GPS) {
        HAL_UART_GPS_ErrorCallback(huart);
    } else if(huart->Instance == USART_GSM) {
        HAL_UART_GSM_ErrorCallback(huart);
    } else if (huart->Instance == UART_CP15) {
        HAL_UART_CP15_ErrorCallback(huart);
    }
}

/**
  * @brief  GPIO EXTI callbacks
  * @param  
  * @note   
  * @retval None
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if(GPIO_Pin == ADS1222_A_DOUT_PIN) {
        HAL_GPIO_SenseA_EXTI_Callback(GPIO_Pin);
    } else if (GPIO_Pin == ADS1222_B_DOUT_PIN) {
        HAL_GPIO_SenseB_EXTI_Callback(GPIO_Pin);
    } else if (GPIO_Pin == (ADS1222_C_DOUT_PIN | ADS1222_D_DOUT_PIN)) {
        HAL_GPIO_SenseCD_EXTI_Callback(GPIO_Pin);
    }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
