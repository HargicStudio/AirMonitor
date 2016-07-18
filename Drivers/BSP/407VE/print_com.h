
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __PRINT_COM_H
#define __PRINT_COM_H

#ifdef __cplusplus
extern "C" {
#endif


#include "stm32f4xx_hal.h"
#include "AaInclude.h"



/* User can use this section to tailor USART_DBG/UART_DBG instance used and associated
   resources */
/* Definition for USART_DBG clock resources */
#define USART_DBG                           USART3
#define USART_DBG_CLK_ENABLE()              __HAL_RCC_USART3_CLK_ENABLE()
#define DMA_DBG_CLK_ENABLE()                __HAL_RCC_DMA1_CLK_ENABLE()
#define USART_DBG_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()
#define USART_DBG_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE() 
    
#define USART_DBG_FORCE_RESET()             __HAL_RCC_USART3_FORCE_RESET()
#define USART_DBG_RELEASE_RESET()           __HAL_RCC_USART3_RELEASE_RESET()
    
/* Definition for USART_DBG Pins */
#define USART_DBG_TX_PIN                    GPIO_PIN_10
#define USART_DBG_TX_GPIO_PORT              GPIOB
#define USART_DBG_TX_AF                     GPIO_AF7_USART3
#define USART_DBG_RX_PIN                    GPIO_PIN_11
#define USART_DBG_RX_GPIO_PORT              GPIOB
#define USART_DBG_RX_AF                     GPIO_AF7_USART3
    
/* Definition for USART_DBG's DMA
   Serial tx debug print will transfer by DMA
   Serial rx shell cmd line will transfer by Interrupt
 */
#define USART_DBG_TX_DMA_CHANNEL            DMA_CHANNEL_4
#define USART_DBG_TX_DMA_STREAM             DMA1_Stream3         
// #define USART_DBG_RX_DMA_CHANNEL            DMA_CHANNEL_4
// #define USART_DBG_RX_DMA_STREAM             DMA1_Stream1
    
/* Definition for USART_DBG's NVIC */
#define USART_DBG_DMA_TX_IRQn               DMA1_Stream3_IRQn
// #define USART_DBG_DMA_RX_IRQn               DMA1_Stream1_IRQn
#define USART_DBG_DMA_TX_IRQHandler         DMA1_Stream3_IRQHandler
// #define USART_DBG_DMA_RX_IRQHandler         DMA1_Stream1_IRQHandler
#define USART_DBG_IRQn                      USART3_IRQn
#define USART_DBG_IRQHandler                USART3_IRQHandler



/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */


void StdUsartInit(void);
void GetBipAndSendByDMA(char* addr, u32 len);
void GetBipAndSendByIT(char* addr, u32 len);
void GetBipAndSendByPolling(char* addr, u32 len);

void HAL_UART_DBG_MspInit(UART_HandleTypeDef *huart);
void HAL_UART_DBG_MspDeInit(UART_HandleTypeDef *huart);
void HAL_UART_DBG_TxCpltCallback(UART_HandleTypeDef *huart);
void HAL_UART_DBG_RxCpltCallback(UART_HandleTypeDef *huart);
void HAL_UART_DBG_ErrorCallback(UART_HandleTypeDef *huart);




#ifdef __cplusplus
}
#endif

#endif /* __PRINT_COM_H */



