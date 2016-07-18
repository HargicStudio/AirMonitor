
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CP15_dev_h
#define __CP15_dev_h

#ifdef __cplusplus
extern "C" {
#endif


#include "stm32f4xx_hal.h"
#include "AaInclude.h"


/* User can use this section to tailor USARTx/UARTx instance used and associated
   resources */
/* Definition for USARTx clock resources */
#define UART_CP15                           UART4
#define UART_CP15_CLK_ENABLE()              __HAL_RCC_UART4_CLK_ENABLE()
#define UART_CP15_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define UART_CP15_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE() 
    
#define UART_CP15_FORCE_RESET()             __HAL_RCC_UART4_FORCE_RESET()
#define UART_CP15_RELEASE_RESET()           __HAL_RCC_UART4_RELEASE_RESET()
    
/* Definition for USARTx Pins */
#define UART_CP15_TX_PIN                    GPIO_PIN_0
#define UART_CP15_TX_GPIO_PORT              GPIOA
#define UART_CP15_TX_AF                     GPIO_AF8_UART4
#define UART_CP15_RX_PIN                    GPIO_PIN_1
#define UART_CP15_RX_GPIO_PORT              GPIOA
#define UART_CP15_RX_AF                     GPIO_AF8_UART4
    
/* Definition for USARTx's NVIC */
#define UART_CP15_IRQn                      UART4_IRQn
#define UART_CP15_IRQHandler                UART4_IRQHandler


/** Description of the macro */  
#define CP15_SET_PIN                         GPIO_PIN_10
#define CP15_SET_GPIO_PORT                   GPIOD
#define CP15_SET_CLK_ENABLE()                __HAL_RCC_GPIOD_CLK_ENABLE()

#define CP15_RESET_PIN                       GPIO_PIN_11
#define CP15_RESET_GPIO_PORT                 GPIOD
#define CP15_RESET_CLK_ENABLE()              __HAL_RCC_GPIOD_CLK_ENABLE()


/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

void CP15UsartInit();
void CP15GpioInit();
void CP15SetPinHigh();
void CP15SetPinLow();
void CP15ResetPinHigh();
void CP15ResetPinLow();
void CP15DataSendByIT(u8* addr, u32 len);
void HAL_UART_CP15_MspInit(UART_HandleTypeDef *huart);
void HAL_UART_CP15_MspDeInit(UART_HandleTypeDef *huart);
void HAL_UART_CP15_TxCpltCallback(UART_HandleTypeDef *huart);
void HAL_UART_CP15_RxCpltCallback(UART_HandleTypeDef *huart);
void HAL_UART_CP15_ErrorCallback(UART_HandleTypeDef *huart);



#ifdef __cplusplus
}
#endif

#endif /* __CP15_dev_h */



