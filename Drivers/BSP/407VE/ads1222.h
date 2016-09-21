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


typedef enum 
{
    Ads1222Err_NoErr,
    Ads1222Err_Error,
    Ads1222Err_NotCplt
} Ads1222Err;


#define ADS1222_A_DOUT_PORT                  GPIOC
#define ADS1222_A_DOUT_PIN                   GPIO_PIN_1

#define ADS1222_B_DOUT_PORT                  GPIOC
#define ADS1222_B_DOUT_PIN                   GPIO_PIN_3

#define ADS1222_C_DOUT_PORT                  GPIOB
#define ADS1222_C_DOUT_PIN                   GPIO_PIN_13

#define ADS1222_D_DOUT_PORT                  GPIOB
#define ADS1222_D_DOUT_PIN                   GPIO_PIN_15

#define ADS1222_A_DOUT_EXTI_IRQHandler      EXTI1_IRQHandler
#define ADS1222_B_DOUT_EXTI_IRQHandler      EXTI3_IRQHandler
#define ADS1222_C_DOUT_EXTI_IRQHandler      EXTI15_10_IRQHandler
#define ADS1222_D_DOUT_EXTI_IRQHandler      EXTI15_10_IRQHandler
#define ADS1222_CD_DOUT_EXTI_IRQHandler      EXTI15_10_IRQHandler



#define IsChipExist(cfg, chip)      ((cfg & chip) == chip)



/** 
 * The brief description. 
 * The detail description. 
 */  
#define ADS1222_CHIP_A          (1<<0)
#define ADS1222_CHIP_B          (1<<1)
#define ADS1222_CHIP_C          (1<<2)
#define ADS1222_CHIP_D          (1<<3)
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
void Ads1222_EnableExti(u8 chipId);

/*******************************************************************************
* º¯ÊýÃû  : ADS1222_AdRead
* Ãè  Êö  : »ñÈ¡ADS1222£®Ò»´ÎÍêÕûµÄÊý¾Ý´«ÊäÎª24bit£¬¸ßÎ»ÏÈ³ö
* Êä  Èë  : chipId: ads1222Ð¾Æ¬±àºÅ
*           channel: ads1222Ð¾Æ¬Í¨µÀºÅ 
* Êä  ³ö  : ÎÞ
* ·µ »Ø Öµ: ¶ÁÈ¡24bit Ô­Ê¼ADÖµ
*******************************************************************************/
Ads1222Err ADS1222_AdRead(u32* data_a, u32* data_b, u32* data_c, u32* data_d);

/**
  * @brief EXTI line detection callbacks
  * @param GPIO_Pin: Specifies the pins connected EXTI line
  * @retval None
  */
void HAL_GPIO_SenseA_EXTI_Callback(uint16_t GPIO_Pin);
void HAL_GPIO_SenseB_EXTI_Callback(uint16_t GPIO_Pin);
void HAL_GPIO_SenseCD_EXTI_Callback(uint16_t GPIO_Pin);

u8 Ads1222_GetChannel();
void Ads1222_SetChannel(u8 channel);
void Ads1222_ExchangeChannel();

void Ads1222_SetExistSense(u8 chip);
u8 Ads122_GetExistSense();



#endif /* __ADS1222_H__ */
