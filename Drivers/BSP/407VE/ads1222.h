/*******************************************************************************
* ads1222.h
*
* Copyright (C) 2015-2017 xxx Technology CO.,LTD.
*
* Author : menki
* Version: V1.0.0  2016-8-2 Create
*
* Desc: ÊµÏÖADS1222´«¸ÐÆ÷µ×²ãÇý¶¯³ÌÐò
*
* Modification: 
*    Date    :  
*    Revision:
*    Author  :
*    Contents:
*
*******************************************************************************/


#ifndef __ADS1222_H__
#define	__ADS1222_H__

/* ========================================================================== */
/*                             Í·ÎÄ¼þÇø                                       */
/* ========================================================================== */
#include "cmsis_os.h"
#include "AaInclude.h"
#include "main.h"
/* ========================================================================== */
/*                           ºêºÍÀàÐÍ¶¨ÒåÇø                                   */
/* ========================================================================== */
//ADS1222 SCLKÁ¬½ÓÒý½Å¶¨Òå 
#define ADS1222_A_SCLK_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOC_CLK_ENABLE()
#define ADS1222_A_SCLK_PORT                  GPIOC
#define ADS1222_A_SCLK_PIN                   GPIO_PIN_0
#define ADS1222_A_SCLK_LOW()          HAL_GPIO_WritePin(ADS1222_A_SCLK_PORT, ADS1222_A_SCLK_PIN, GPIO_PIN_RESET) 
#define ADS1222_A_SCLK_HIGH()         HAL_GPIO_WritePin(ADS1222_A_SCLK_PORT, ADS1222_A_SCLK_PIN, GPIO_PIN_SET)


#define ADS1222_B_SCLK_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOC_CLK_ENABLE()
#define ADS1222_B_SCLK_PORT                  GPIOC
#define ADS1222_B_SCLK_PIN                   GPIO_PIN_2
#define ADS1222_B_SCLK_LOW()          HAL_GPIO_WritePin(ADS1222_B_SCLK_PORT, ADS1222_B_SCLK_PIN, GPIO_PIN_RESET) 
#define ADS1222_B_SCLK_HIGH()         HAL_GPIO_WritePin(ADS1222_B_SCLK_PORT, ADS1222_B_SCLK_PIN, GPIO_PIN_SET)


#define ADS1222_C_SCLK_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOB_CLK_ENABLE()
#define ADS1222_C_SCLK_PORT                  GPIOB
#define ADS1222_C_SCLK_PIN                   GPIO_PIN_12
#define ADS1222_C_SCLK_LOW()          HAL_GPIO_WritePin(ADS1222_C_SCLK_PORT, ADS1222_C_SCLK_PIN, GPIO_PIN_RESET) 
#define ADS1222_C_SCLK_HIGH()         HAL_GPIO_WritePin(ADS1222_C_SCLK_PORT, ADS1222_C_SCLK_PIN, GPIO_PIN_SET)


#define ADS1222_D_SCLK_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOB_CLK_ENABLE()
#define ADS1222_D_SCLK_PORT                  GPIOB
#define ADS1222_D_SCLK_PIN                   GPIO_PIN_14
#define ADS1222_D_SCLK_LOW()          HAL_GPIO_WritePin(ADS1222_D_SCLK_PORT, ADS1222_D_SCLK_PIN, GPIO_PIN_RESET) 
#define ADS1222_D_SCLK_HIGH()         HAL_GPIO_WritePin(ADS1222_D_SCLK_PORT, ADS1222_D_SCLK_PIN, GPIO_PIN_SET)


//ADS1222 DOUTÁ¬½ÓÒý½Å¶¨Òå 
#define ADS1222_A_DOUT_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOC_CLK_ENABLE()
#define ADS1222_A_DOUT_PORT                  GPIOC
#define ADS1222_A_DOUT_PIN                   GPIO_PIN_1
#define ADS1222_A_DOUT_LOW()     HAL_GPIO_WritePin(ADS1222_A_DOUT_PORT, ADS1222_A_DOUT_PIN, GPIO_PIN_RESET) 
#define ADS1222_A_DOUT_HIGH()    HAL_GPIO_WritePin(ADS1222_A_DOUT_PORT, ADS1222_A_DOUT_PIN, GPIO_PIN_SET)
#define ADS1222_A_DOUT_IN()	     HAL_GPIO_ReadPin(ADS1222_A_DOUT_PORT, ADS1222_A_DOUT_PIN)
#define ADS1222_A_DOUT_EXTI_IRQn            EXTI1_IRQn
#define ADS1222_A_DOUT_EXTI_IRQHandler      EXTI1_IRQHandler


#define ADS1222_B_DOUT_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOC_CLK_ENABLE()
#define ADS1222_B_DOUT_PORT                  GPIOC
#define ADS1222_B_DOUT_PIN                   GPIO_PIN_3
#define ADS1222_B_DOUT_LOW()     HAL_GPIO_WritePin(ADS1222_B_DOUT_PORT, ADS1222_B_DOUT_PIN, GPIO_PIN_RESET) 
#define ADS1222_B_DOUT_HIGH()    HAL_GPIO_WritePin(ADS1222_B_DOUT_PORT, ADS1222_B_DOUT_PIN, GPIO_PIN_SET)
#define ADS1222_B_DOUT_IN()	     HAL_GPIO_ReadPin(ADS1222_B_DOUT_PORT, ADS1222_B_DOUT_PIN)


#define ADS1222_C_DOUT_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOB_CLK_ENABLE()
#define ADS1222_C_DOUT_PORT                  GPIOB
#define ADS1222_C_DOUT_PIN                   GPIO_PIN_13
#define ADS1222_C_DOUT_LOW()     HAL_GPIO_WritePin(ADS1222_C_DOUT_PORT, ADS1222_C_DOUT_PIN, GPIO_PIN_RESET) 
#define ADS1222_C_DOUT_HIGH()    HAL_GPIO_WritePin(ADS1222_C_DOUT_PORT, ADS1222_C_DOUT_PIN, GPIO_PIN_SET)
#define ADS1222_C_DOUT_IN()	     HAL_GPIO_ReadPin(ADS1222_C_DOUT_PORT, ADS1222_C_DOUT_PIN)


#define ADS1222_D_DOUT_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOB_CLK_ENABLE()
#define ADS1222_D_DOUT_PORT                  GPIOB
#define ADS1222_D_DOUT_PIN                   GPIO_PIN_15
#define ADS1222_D_DOUT_LOW()     HAL_GPIO_WritePin(ADS1222_D_DOUT_PORT, ADS1222_D_DOUT_PIN, GPIO_PIN_RESET) 
#define ADS1222_D_DOUT_HIGH()    HAL_GPIO_WritePin(ADS1222_D_DOUT_PORT, ADS1222_D_DOUT_PIN, GPIO_PIN_SET)
#define ADS1222_D_DOUT_IN()	     HAL_GPIO_ReadPin(ADS1222_D_DOUT_PORT, ADS1222_D_DOUT_PIN)



//ADS1222 MUXÒý½Å¶¨Òå 
#define ADS1222_MUX_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOB_CLK_ENABLE()
#define ADS1222_MUX_PORT                  GPIOB
#define ADS1222_MUX_PIN                   GPIO_PIN_0

#define ADS1222_MUX_LOW()        HAL_GPIO_WritePin(ADS1222_MUX_PORT, ADS1222_MUX_PIN, GPIO_PIN_RESET) 
#define ADS1222_MUX_HIGH()       HAL_GPIO_WritePin(ADS1222_MUX_PORT, ADS1222_MUX_PIN, GPIO_PIN_SET)
#define Ads1222_MUX_PinRead()       HAL_GPIO_ReadPin(ADS1222_MUX_PORT, ADS1222_MUX_PIN)


//ADS1222 TEMPENÒý½Å¶¨Òå 
#define ADS1222_TEMPEN_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOB_CLK_ENABLE()
#define ADS1222_TEMPEN_PORT                  GPIOB
#define ADS1222_TEMPEN_PIN                   GPIO_PIN_1

#define ADS1222_TEMPEN_LOW()     HAL_GPIO_WritePin(ADS1222_TEMPEN_PORT, ADS1222_TEMPEN_PIN, GPIO_PIN_RESET) 
#define ADS1222_TEMPEN_HIGH()    HAL_GPIO_WritePin(ADS1222_TEMPEN_PORT, ADS1222_TEMPEN_PIN, GPIO_PIN_SET)



//ADS1222 BUFENÒý½Å¶¨Òå 
#define ADS1222_BUFEN_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOB_CLK_ENABLE()
#define ADS1222_BUFEN_PORT                  GPIOB
#define ADS1222_BUFEN_PIN                   GPIO_PIN_2

#define ADS1222_BUFEN_LOW()      HAL_GPIO_WritePin(ADS1222_BUFEN_PORT, ADS1222_BUFEN_PIN, GPIO_PIN_RESET) 
#define ADS1222_BUFEN_HIGH()     HAL_GPIO_WritePin(ADS1222_BUFEN_PORT, ADS1222_BUFEN_PIN, GPIO_PIN_SET)





//ADS1222Ð¾Æ¬±àºÅ
#define ADS1222_CHIP_A   0
#define ADS1222_CHIP_B   1
#define ADS1222_CHIP_C   2
#define ADS1222_CHIP_D   3
#define Ads1222_ChipID_Invalid  0xFF

//ADS1222Ð¾Æ¬²É¼¯Í¨µÀºÅ
#define  ADS1222_CH0   0
#define  ADS1222_CH1   1

/* ========================================================================== */
/*                          Êý¾Ý½á¹¹¶¨ÒåÇø                                    */
/* ========================================================================== */

/* ========================================================================== */
/*                          º¯ÊýÉùÃ÷Çø                                        */
/* ========================================================================== */

/*******************************************************************************
* º¯ÊýÃû  : ADS1222_Init
* Ãè  Êö  : ¸Ãº¯Êý³õÊ¼»¯ADS1222
* Êä  Èë  : ÎÞ
* Êä  ³ö  : ÎÞ
* ·µ»ØÖµ  : ÎÞ
*******************************************************************************/
void ADS1222_Init(void);

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
 *      2016-08-10 Huang Shengda
 */  
void Ads1222_EnableExti();

/*******************************************************************************
* º¯ÊýÃû  : ADS1222_AdRead
* Ãè  Êö  : »ñÈ¡ADS1222£®Ò»´ÎÍêÕûµÄÊý¾Ý´«ÊäÎª24bit£¬¸ßÎ»ÏÈ³ö
* Êä  Èë  : chipId: ads1222Ð¾Æ¬±àºÅ
*           channel: ads1222Ð¾Æ¬Í¨µÀºÅ 
* Êä  ³ö  : ÎÞ
* ·µ »Ø Öµ: ¶ÁÈ¡24bit Ô­Ê¼ADÖµ
*******************************************************************************/
unsigned long ADS1222_AdRead(uint8_t chipId);

/**
  * @brief EXTI line detection callbacks
  * @param GPIO_Pin: Specifies the pins connected EXTI line
  * @retval None
  */
void HAL_GPIO_SenseA_EXTI_Callback(uint16_t GPIO_Pin);

void ADS1222_DOUT_IPU(uint8_t chipId);
void ADS1222_DOUT_Out_PP(uint8_t chipId);;
u8 Ads1222_GetChannel();
void Ads1222_SetChannel(u8 channel);



#endif /* __ADS1222_H__ */
