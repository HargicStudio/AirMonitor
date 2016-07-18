
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __GPS_DEV_H
#define __GPS_DEV_H

#ifdef __cplusplus
extern "C" {
#endif


#include "stm32f4xx_hal.h"
#include "AaInclude.h"


/* User can use this section to tailor USARTx/UARTx instance used and associated
   resources */
/* Definition for USARTx clock resources */
#define USART_GPS                           USART1
#define USART_GPS_CLK_ENABLE()              __HAL_RCC_USART1_CLK_ENABLE();
#define USART_GPS_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define USART_GPS_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE() 
    
#define USART_GPS_FORCE_RESET()             __HAL_RCC_USART1_FORCE_RESET()
#define USART_GPS_RELEASE_RESET()           __HAL_RCC_USART1_RELEASE_RESET()
    
/* Definition for USARTx Pins */
#define USART_GPS_TX_PIN                    GPIO_PIN_9
#define USART_GPS_TX_GPIO_PORT              GPIOA
#define USART_GPS_TX_AF                     GPIO_AF7_USART1
#define USART_GPS_RX_PIN                    GPIO_PIN_10
#define USART_GPS_RX_GPIO_PORT              GPIOA
#define USART_GPS_RX_AF                     GPIO_AF7_USART1
    
/* Definition for USARTx's NVIC */
#define USART_GPS_IRQn                      USART1_IRQn
#define USART_GPS_IRQHandler                USART1_IRQHandler



/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

void GpsUsartInit();
void GpsDataSendByIT(u8* addr, u32 len);
void HAL_UART_GPS_MspInit(UART_HandleTypeDef *huart);
void HAL_UART_GPS_MspDeInit(UART_HandleTypeDef *huart);
void HAL_UART_GPS_TxCpltCallback(UART_HandleTypeDef *huart);
void HAL_UART_GPS_RxCpltCallback(UART_HandleTypeDef *huart);
void HAL_UART_GPS_ErrorCallback(UART_HandleTypeDef *huart);



#ifdef __cplusplus
}
#endif

#endif /* __GPS_DEV_H */



