/**
  ******************************************************************************
  * @file    UART/UART_Printf/Src/stm32f1xx_hal_msp.c
  * @author  MCD Application Team
  * @version V1.3.0
  * @date    18-December-2015
  * @brief   HAL MSP module.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2015 STMicroelectronics</center></h2>
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
#include "gps_dev.h"
#include "gps.h"

/** @addtogroup STM32F1xx_HAL_Examples
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
// Uart type
UART_HandleTypeDef UartHandle_gps;

extern char recv_char_gps;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup HAL_MSP_Private_Functions
  * @{
  */


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
void GpsUsartInit()
{
    /* Put the USART peripheral in the Asynchronous mode (UART Mode) */
    /* UART1 configured as follow:
      - Word Length = 8 Bits
      - Stop Bit    = One Stop bit
      - Parity      = ODD parity
      - BaudRate    = 9600 baud
      - Hardware flow control disabled (RTS and CTS signals) */
    UartHandle_gps.Instance          = USART_GPS;

    UartHandle_gps.Init.BaudRate     = 9600;
    UartHandle_gps.Init.WordLength   = UART_WORDLENGTH_8B;
    UartHandle_gps.Init.StopBits     = UART_STOPBITS_1;
    UartHandle_gps.Init.Parity       = UART_PARITY_NONE;
    UartHandle_gps.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
    UartHandle_gps.Init.Mode         = UART_MODE_TX_RX;
    UartHandle_gps.Init.OverSampling = UART_OVERSAMPLING_8;

    if(HAL_UART_Init(&UartHandle_gps) != HAL_OK)
    {
        /* Initialization Error */
    }
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
 *      2016-5-21 Huang Shengda
 */  
void GpsDataSendByIT(u8* addr, u32 len)
{
    if(HAL_UART_Transmit_IT(&UartHandle_gps, (uint8_t *)addr, len) != HAL_OK) {
        /* Transfer error in transmission process */
    }
}


/** @defgroup HAL_MSP_Private_Functions
  * @{
  */

/**
  * @brief UART MSP Initialization 
  *        This function configures the hardware resources used in this example: 
  *           - Peripheral's clock enable
  *           - Peripheral's GPIO Configuration  
  *           - DMA configuration for transmission request by peripheral 
  *           - NVIC configuration for DMA interrupt request enable
  * @param huart: UART handle pointer
  * @retval None
  */
void HAL_UART_GPS_MspInit(UART_HandleTypeDef *huart)
{
  GPIO_InitTypeDef  GPIO_InitStruct;
  
  /*##-1- Enable peripherals and GPIO Clocks #################################*/
  /* Enable GPIO clock */
  USART_GPS_TX_GPIO_CLK_ENABLE();
  USART_GPS_RX_GPIO_CLK_ENABLE();
  /* Enable USARTx clock */
  USART_GPS_CLK_ENABLE(); 
  
  /*##-2- Configure peripheral GPIO ##########################################*/
  /* UART TX GPIO pin configuration  */
  GPIO_InitStruct.Pin       = USART_GPS_TX_PIN;
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull      = GPIO_PULLUP;
  GPIO_InitStruct.Speed     = GPIO_SPEED_FAST;
  GPIO_InitStruct.Alternate = USART_GPS_TX_AF;
  
  HAL_GPIO_Init(USART_GPS_TX_GPIO_PORT, &GPIO_InitStruct);
    
  /* UART RX GPIO pin configuration  */
  GPIO_InitStruct.Pin = USART_GPS_RX_PIN;
  GPIO_InitStruct.Alternate = USART_GPS_RX_AF;
    
  HAL_GPIO_Init(USART_GPS_RX_GPIO_PORT, &GPIO_InitStruct);
    
    
  /*##-4- Configure the NVIC for DMA #########################################*/
  
  /* NVIC configuration for USART TC interrupt */
  HAL_NVIC_SetPriority(USART_GPS_IRQn, SYSTEM_IRQ_PRIORITY_LOW_2, 0);
  HAL_NVIC_EnableIRQ(USART_GPS_IRQn);
}


/**
  * @brief UART MSP De-Initialization 
  *        This function frees the hardware resources used in this example:
  *          - Disable the Peripheral's clock
  *          - Revert GPIO, DMA and NVIC configuration to their default state
  * @param huart: UART handle pointer
  * @retval None
  */
void HAL_UART_GPS_MspDeInit(UART_HandleTypeDef *huart)
{
  /*##-1- Reset peripherals ##################################################*/
  USART_GPS_FORCE_RESET();
  USART_GPS_RELEASE_RESET();

  /*##-2- Disable peripherals and GPIO Clocks #################################*/
  /* Configure UART Tx as alternate function  */
  HAL_GPIO_DeInit(USART_GPS_TX_GPIO_PORT, USART_GPS_TX_PIN);
  /* Configure UART Rx as alternate function  */
  HAL_GPIO_DeInit(USART_GPS_RX_GPIO_PORT, USART_GPS_RX_PIN);
  
  HAL_NVIC_DisableIRQ(USART_GPS_IRQn);
}


/**
  * @brief  Tx Transfer completed callback
  * @param  huart: UART handle. 
  * @note   This example shows a simple way to report end of DMA Tx transfer, and 
  *         you can add your own implementation. 
  * @retval None
  */
void HAL_UART_GPS_TxCpltCallback(UART_HandleTypeDef *huart)
{
    //GpsWaitForSendCplt();
}

/**
  * @brief  Rx Transfer completed callback
  * @param  huart: UART handle
  * @note   This example shows a simple way to report end of DMA Rx transfer, and 
  *         you can add your own implementation.
  * @retval None
  */
void HAL_UART_GPS_RxCpltCallback(UART_HandleTypeDef *huart)
{
    GpsRecvDataFromISR(huart);
}

/**
  * @brief  UART error callbacks
  * @param  huart: UART handle
  * @note   This example shows a simple way to report transfer error, and you can
  *         add your own implementation.
  * @retval None
  */
void HAL_UART_GPS_ErrorCallback(UART_HandleTypeDef *huart)
{
    HAL_UART_GPS_MspDeInit(&UartHandle_gps);
    GpsUsartInit();
    HAL_UART_GPS_MspInit(&UartHandle_gps);
    HAL_UART_Receive_IT(&UartHandle_gps, (u8*)&recv_char_gps, 1);
}


 


/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
