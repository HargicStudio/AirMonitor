
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __Gsm_dev_h
#define __Gsm_dev_h

#ifdef __cplusplus
extern "C" {
#endif


#include "stm32f4xx_hal.h"
#include "AaInclude.h"


/* User can use this section to tailor USARTx/UARTx instance used and associated
   resources */
/* Definition for USARTx clock resources */
#define USART_GSM                           USART2
#define USART_GSM_CLK_ENABLE()              __HAL_RCC_USART2_CLK_ENABLE()
#define USART_GSM_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define USART_GSM_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE() 
    
#define USART_GSM_FORCE_RESET()             __HAL_RCC_USART1_FORCE_RESET()
#define USART_GSM_RELEASE_RESET()           __HAL_RCC_USART1_RELEASE_RESET()
    
/* Definition for USARTx Pins */
#define USART_GSM_TX_PIN                    GPIO_PIN_2
#define USART_GSM_TX_GPIO_PORT              GPIOA
#define USART_GSM_TX_AF                     GPIO_AF7_USART2
#define USART_GSM_RX_PIN                    GPIO_PIN_3
#define USART_GSM_RX_GPIO_PORT              GPIOA
#define USART_GSM_RX_AF                     GPIO_AF7_USART2
    
/* Definition for USARTx's NVIC */
#define USART_GSM_IRQn                      USART2_IRQn
#define USART_GSM_IRQHandler                USART2_IRQHandler



/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

void GsmUsartInit();
void GsmDataSendByIT(u8* addr, u32 len);
void HAL_UART_GSM_MspInit(UART_HandleTypeDef *huart);
void HAL_UART_GSM_MspDeInit(UART_HandleTypeDef *huart);
void HAL_UART_GSM_TxCpltCallback(UART_HandleTypeDef *huart);
void HAL_UART_GSM_RxCpltCallback(UART_HandleTypeDef *huart);
void HAL_UART_GSM_ErrorCallback(UART_HandleTypeDef *huart);



#ifdef __cplusplus
}
#endif

#endif /* __Gsm_dev_h */



