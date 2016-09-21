/*******************************************************************************
* ads1222.c
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

/* ========================================================================== */
/*                             Í·ÎÄ¼þÇø                                       */
/* ========================================================================== */
#include "ads1222.h"
#include <stdio.h>
#include <stdlib.h>
#include "cmsis_os.h"
#include "AaInclude.h"
#include "main.h"
#include "alpha_sense.h"
#include "led_dev.h"



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

#define ADS1222_A_DOUT_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOC_CLK_ENABLE()
#define ADS1222_A_DOUT_LOW()        HAL_GPIO_WritePin(ADS1222_A_DOUT_PORT, ADS1222_A_DOUT_PIN, GPIO_PIN_RESET) 
#define ADS1222_A_DOUT_HIGH()       HAL_GPIO_WritePin(ADS1222_A_DOUT_PORT, ADS1222_A_DOUT_PIN, GPIO_PIN_SET)
#define ADS1222_A_DOUT_IN()	        HAL_GPIO_ReadPin(ADS1222_A_DOUT_PORT, ADS1222_A_DOUT_PIN)
#define ADS1222_A_DOUT_EXTI_IRQn            EXTI1_IRQn

#define ADS1222_B_DOUT_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOC_CLK_ENABLE()
#define ADS1222_B_DOUT_LOW()        HAL_GPIO_WritePin(ADS1222_B_DOUT_PORT, ADS1222_B_DOUT_PIN, GPIO_PIN_RESET) 
#define ADS1222_B_DOUT_HIGH()       HAL_GPIO_WritePin(ADS1222_B_DOUT_PORT, ADS1222_B_DOUT_PIN, GPIO_PIN_SET)
#define ADS1222_B_DOUT_IN()	        HAL_GPIO_ReadPin(ADS1222_B_DOUT_PORT, ADS1222_B_DOUT_PIN)
#define ADS1222_B_DOUT_EXTI_IRQn            EXTI3_IRQn

#define ADS1222_C_DOUT_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOB_CLK_ENABLE()
#define ADS1222_C_DOUT_LOW()        HAL_GPIO_WritePin(ADS1222_C_DOUT_PORT, ADS1222_C_DOUT_PIN, GPIO_PIN_RESET) 
#define ADS1222_C_DOUT_HIGH()       HAL_GPIO_WritePin(ADS1222_C_DOUT_PORT, ADS1222_C_DOUT_PIN, GPIO_PIN_SET)
#define ADS1222_C_DOUT_IN()	        HAL_GPIO_ReadPin(ADS1222_C_DOUT_PORT, ADS1222_C_DOUT_PIN)
#define ADS1222_C_DOUT_EXTI_IRQn            EXTI15_10_IRQn

#define ADS1222_D_DOUT_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOB_CLK_ENABLE()
#define ADS1222_D_DOUT_LOW()        HAL_GPIO_WritePin(ADS1222_D_DOUT_PORT, ADS1222_D_DOUT_PIN, GPIO_PIN_RESET) 
#define ADS1222_D_DOUT_HIGH()       HAL_GPIO_WritePin(ADS1222_D_DOUT_PORT, ADS1222_D_DOUT_PIN, GPIO_PIN_SET)
#define ADS1222_D_DOUT_IN()	        HAL_GPIO_ReadPin(ADS1222_D_DOUT_PORT, ADS1222_D_DOUT_PIN)
#define ADS1222_D_DOUT_EXTI_IRQn            EXTI15_10_IRQn

#define ADS1222_CD_DOUT_EXTI_IRQn            EXTI15_10_IRQn

#define ADS1222_MUX_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOB_CLK_ENABLE()
#define ADS1222_MUX_PORT                  GPIOB
#define ADS1222_MUX_PIN                   GPIO_PIN_0
#define ADS1222_MUX_LOW()        HAL_GPIO_WritePin(ADS1222_MUX_PORT, ADS1222_MUX_PIN, GPIO_PIN_RESET) 
#define ADS1222_MUX_HIGH()       HAL_GPIO_WritePin(ADS1222_MUX_PORT, ADS1222_MUX_PIN, GPIO_PIN_SET)
#define ADS1222_MUX_TOGGLE()     HAL_GPIO_TogglePin(ADS1222_MUX_PORT, ADS1222_MUX_PIN);
#define Ads1222_MUX_PinRead()       HAL_GPIO_ReadPin(ADS1222_MUX_PORT, ADS1222_MUX_PIN)

#define ADS1222_TEMPEN_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOB_CLK_ENABLE()
#define ADS1222_TEMPEN_PORT                  GPIOB
#define ADS1222_TEMPEN_PIN                   GPIO_PIN_1
#define ADS1222_TEMPEN_LOW()     HAL_GPIO_WritePin(ADS1222_TEMPEN_PORT, ADS1222_TEMPEN_PIN, GPIO_PIN_RESET) 
#define ADS1222_TEMPEN_HIGH()    HAL_GPIO_WritePin(ADS1222_TEMPEN_PORT, ADS1222_TEMPEN_PIN, GPIO_PIN_SET)

#define ADS1222_BUFEN_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOB_CLK_ENABLE()
#define ADS1222_BUFEN_PORT                  GPIOB
#define ADS1222_BUFEN_PIN                   GPIO_PIN_2

#define ADS1222_BUFEN_LOW()      HAL_GPIO_WritePin(ADS1222_BUFEN_PORT, ADS1222_BUFEN_PIN, GPIO_PIN_RESET) 
#define ADS1222_BUFEN_HIGH()     HAL_GPIO_WritePin(ADS1222_BUFEN_PORT, ADS1222_BUFEN_PIN, GPIO_PIN_SET)




static u8 _sense_exist_bit = 0x00;

static u8 _sense_conv_cplt = 0x00;



static void delay_us(uint16_t us);
static void delay_10us(uint16_t _10us);
static void ADS1222_DOUT_IPU(uint8_t chipId);

static void Ads1222_SenseA_EXTIGpioConfig();
static void Ads1222_SenseB_EXTIGpioConfig();
static void Ads1222_SenseC_EXTIGpioConfig();
static void Ads1222_SenseD_EXTIGpioConfig();
static void Ads1222_SenseA_InputGpioConfig();
static void Ads1222_SenseB_InputGpioConfig();
static void Ads1222_SenseC_InputGpioConfig();
static void Ads1222_SenseD_InputGpioConfig();
static void Ads1222_SenseA_OutputGpioConfig();
static void Ads1222_SenseB_OutputGpioConfig();
static void Ads1222_SenseC_OutputGpioConfig();
static void Ads1222_SenseD_OutputGpioConfig();



/** 
 * This is a brief description. 
 * delay microsecond, used in F407 168MHz system clock. 
 * @param[in]   inArgName input argument description. 
 * @param[out]  outArgName output argument description.  
 * @retval  
 * @retval  
 * @par 
 *      
 * @par 
 *      
 * @par History
 *      2016-08-06 Huang Shengda
 */  
static void delay_us(uint16_t us)
{
    uint16_t cnt;
    while(us--) {
        cnt = 21;
        while(cnt--);
    }
}

/** 
 * This is a brief description. 
 * delay microsecond base on 10 us, only used in F407 168MHz system clock. 
 * @param[in]   In multiples of 10 us. 
 * @param[out]  outArgName output argument description.  
 * @retval  
 * @retval  
 * @par 
 *      
 * @par 
 *      
 * @par History
 *      2016-08-06 Huang Shengda
 */  
static void delay_10us(uint16_t _10us)
{
    uint16_t cnt;
    while(_10us--) {
        cnt = 274;
        while(cnt--);
    }
}

/**
  * º¯Êý¹¦ÄÜ: ADS1222 ³õÊ¼»¯º¯Êý
  * ÊäÈë²ÎÊý: ÎÞ
  * ·µ »Ø Öµ: ÎÞ
  * Ëµ    Ã÷£ºÎÞ
  */
void ADS1222_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    
    //SCLK CLK ENABLE
    ADS1222_A_SCLK_GPIO_CLK_ENABLE();
    ADS1222_B_SCLK_GPIO_CLK_ENABLE();
    ADS1222_C_SCLK_GPIO_CLK_ENABLE();
    ADS1222_D_SCLK_GPIO_CLK_ENABLE();
    
    //GPIOÅäÖÃÎªÊä³ö
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    
    //SCLK
    GPIO_InitStruct.Pin = ADS1222_A_SCLK_PIN;
    HAL_GPIO_Init(ADS1222_A_SCLK_PORT, &GPIO_InitStruct); 

    GPIO_InitStruct.Pin = ADS1222_B_SCLK_PIN;
    HAL_GPIO_Init(ADS1222_B_SCLK_PORT, &GPIO_InitStruct); 
    
    GPIO_InitStruct.Pin = ADS1222_C_SCLK_PIN;
    HAL_GPIO_Init(ADS1222_C_SCLK_PORT, &GPIO_InitStruct); 

    GPIO_InitStruct.Pin = ADS1222_D_SCLK_PIN;
    HAL_GPIO_Init(ADS1222_D_SCLK_PORT, &GPIO_InitStruct); 
    
    //MUX
    ADS1222_MUX_GPIO_CLK_ENABLE(); 
    GPIO_InitStruct.Pin = ADS1222_MUX_PIN;
    HAL_GPIO_Init(ADS1222_MUX_PORT, &GPIO_InitStruct); 
    
    //TEMPEN
    ADS1222_TEMPEN_GPIO_CLK_ENABLE();
    GPIO_InitStruct.Pin = ADS1222_TEMPEN_PIN;
    HAL_GPIO_Init(ADS1222_TEMPEN_PORT, &GPIO_InitStruct); 
    
    //BUFEN
    ADS1222_BUFEN_GPIO_CLK_ENABLE();
    GPIO_InitStruct.Pin = ADS1222_BUFEN_PIN;
    HAL_GPIO_Init(ADS1222_BUFEN_PORT, &GPIO_InitStruct); 

    ADS1222_A_SCLK_LOW();
    ADS1222_B_SCLK_LOW();
    ADS1222_C_SCLK_LOW();
    ADS1222_D_SCLK_LOW();
    
    /* The signal is selected though the input mux, which is controlled by MUX */
    ADS1222_MUX_LOW(); //Ä¬ÈÏÍ¨µÀ0
    
    /*  On-chip diodes provide temperature-sensing capabili-ty.
        By setting the TEMPEN pin high, the selected analog
        inputs are disconnected and the inputs to the A/D
        converter are connected to the anodes of two diodes.
    */
    ADS1222_TEMPEN_LOW();
    
    /* With the buffer disabled by setting the BUFEN pin low */
    ADS1222_BUFEN_LOW();
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
 *      2016-08-10 Huang Shengda
 *      2016-09-19 huang Shengda: support Sense A/B/C/D
 */  
static void Ads1222_SenseA_EXTIGpioConfig()
{
    GPIO_InitTypeDef   GPIO_InitStructure;

    /* Enable /DRDY clock */
    ADS1222_A_DOUT_GPIO_CLK_ENABLE();

    /* Configure PC1 pin as input floating */
    GPIO_InitStructure.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    GPIO_InitStructure.Pin = ADS1222_A_DOUT_PIN;
    HAL_GPIO_Init(ADS1222_A_DOUT_PORT, &GPIO_InitStructure);

    /* Enable and set EXTI Line1 Interrupt to the HIGH2 priority */
    // !!! this is very important
    // !!! we must clear EXTI bit first or it will trigger next interrupt when IRQ enabled
    __HAL_GPIO_EXTI_CLEAR_IT(ADS1222_A_DOUT_PIN);
    HAL_NVIC_SetPriority(ADS1222_A_DOUT_EXTI_IRQn, SYSTEM_IRQ_PRIORITY_HIGH_2, 0);
    HAL_NVIC_EnableIRQ(ADS1222_A_DOUT_EXTI_IRQn);
}

static void Ads1222_SenseB_EXTIGpioConfig()
{
    GPIO_InitTypeDef   GPIO_InitStructure;

    /* Enable /DRDY clock */
    ADS1222_B_DOUT_GPIO_CLK_ENABLE();

    /* Configure PC3 pin as input floating */
    GPIO_InitStructure.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    GPIO_InitStructure.Pin = ADS1222_B_DOUT_PIN;
    HAL_GPIO_Init(ADS1222_B_DOUT_PORT, &GPIO_InitStructure);

    /* Enable and set EXTI Line3 Interrupt to the HIGH2 priority */
    // !!! this is very important
    // !!! we must clear EXTI bit first or it will trigger next interrupt when IRQ enabled
    __HAL_GPIO_EXTI_CLEAR_IT(ADS1222_B_DOUT_PIN);
    HAL_NVIC_SetPriority(ADS1222_B_DOUT_EXTI_IRQn, SYSTEM_IRQ_PRIORITY_HIGH_2, 0);
    HAL_NVIC_EnableIRQ(ADS1222_B_DOUT_EXTI_IRQn);
}

static void Ads1222_SenseC_EXTIGpioConfig()
{
    GPIO_InitTypeDef   GPIO_InitStructure;

    /* Enable /DRDY clock */
    ADS1222_C_DOUT_GPIO_CLK_ENABLE();

    /* Configure PB13 pin as input floating */
    GPIO_InitStructure.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    GPIO_InitStructure.Pin = ADS1222_C_DOUT_PIN;
    HAL_GPIO_Init(ADS1222_C_DOUT_PORT, &GPIO_InitStructure);

    /* Enable and set EXTI Line15:10 Interrupt to the HIGH2 priority */
    // !!! this is very important
    // !!! we must clear EXTI bit first or it will trigger next interrupt when IRQ enabled
    __HAL_GPIO_EXTI_CLEAR_IT(ADS1222_C_DOUT_PIN);
    HAL_NVIC_SetPriority(ADS1222_C_DOUT_EXTI_IRQn, SYSTEM_IRQ_PRIORITY_HIGH_2, 0);
    HAL_NVIC_EnableIRQ(ADS1222_C_DOUT_EXTI_IRQn);
}

static void Ads1222_SenseD_EXTIGpioConfig()
{
    GPIO_InitTypeDef   GPIO_InitStructure;

    /* Enable /DRDY clock */
    ADS1222_D_DOUT_GPIO_CLK_ENABLE();

    /* Configure PB15 pin as input floating */
    GPIO_InitStructure.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    GPIO_InitStructure.Pin = ADS1222_D_DOUT_PIN;
    HAL_GPIO_Init(ADS1222_D_DOUT_PORT, &GPIO_InitStructure);

    /* Enable and set EXTI Line15:10 Interrupt to the HIGH2 priority */
    // !!! this is very important
    // !!! we must clear EXTI bit first or it will trigger next interrupt when IRQ enabled
    __HAL_GPIO_EXTI_CLEAR_IT(ADS1222_D_DOUT_PIN);
    HAL_NVIC_SetPriority(ADS1222_D_DOUT_EXTI_IRQn, SYSTEM_IRQ_PRIORITY_HIGH_2, 0);
    HAL_NVIC_EnableIRQ(ADS1222_D_DOUT_EXTI_IRQn);
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
 *      2016-08-10 Huang Shengda
 *      2016-09-19 huang Shengda: support Sense A/B/C/D
 */  
static void Ads1222_SenseA_InputGpioConfig()
{
    GPIO_InitTypeDef GPIO_InitStruct;

    // disable exti
    HAL_NVIC_DisableIRQ(ADS1222_A_DOUT_EXTI_IRQn);

    // configuration as input
    GPIO_InitStruct.Pin   = ADS1222_A_DOUT_PIN;
    GPIO_InitStruct.Mode  = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull  = GPIO_PULLUP;
    HAL_GPIO_Init(ADS1222_A_DOUT_PORT, &GPIO_InitStruct);
}

static void Ads1222_SenseB_InputGpioConfig()
{
    GPIO_InitTypeDef GPIO_InitStruct;

    // disable exti
    HAL_NVIC_DisableIRQ(ADS1222_B_DOUT_EXTI_IRQn);

    // configuration as input
    GPIO_InitStruct.Pin   = ADS1222_B_DOUT_PIN;
    GPIO_InitStruct.Mode  = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull  = GPIO_PULLUP;
    HAL_GPIO_Init(ADS1222_B_DOUT_PORT, &GPIO_InitStruct);
}

static void Ads1222_SenseC_InputGpioConfig()
{
    GPIO_InitTypeDef GPIO_InitStruct;

    // disable exti
    HAL_NVIC_DisableIRQ(ADS1222_C_DOUT_EXTI_IRQn);

    // configuration as input
    GPIO_InitStruct.Pin   = ADS1222_C_DOUT_PIN;
    GPIO_InitStruct.Mode  = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull  = GPIO_PULLUP;
    HAL_GPIO_Init(ADS1222_C_DOUT_PORT, &GPIO_InitStruct);
}

static void Ads1222_SenseD_InputGpioConfig()
{
    GPIO_InitTypeDef GPIO_InitStruct;

    // disable exti
    HAL_NVIC_DisableIRQ(ADS1222_D_DOUT_EXTI_IRQn);

    // configuration as input
    GPIO_InitStruct.Pin   = ADS1222_D_DOUT_PIN;
    GPIO_InitStruct.Mode  = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull  = GPIO_PULLUP;
    HAL_GPIO_Init(ADS1222_D_DOUT_PORT, &GPIO_InitStruct);
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
 *      2016-08-10 Huang Shengda
 *      2016-09-19 huang Shengda: support Sense A/B/C/D
 */  
static void Ads1222_SenseA_OutputGpioConfig()
{
    GPIO_InitTypeDef GPIO_InitStruct;

    // disable exti
    HAL_NVIC_DisableIRQ(ADS1222_A_DOUT_EXTI_IRQn);

    // configuration as output
    GPIO_InitStruct.Pin = ADS1222_A_DOUT_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(ADS1222_A_DOUT_PORT, &GPIO_InitStruct); 
}

static void Ads1222_SenseB_OutputGpioConfig()
{
    GPIO_InitTypeDef GPIO_InitStruct;

    // disable exti
    HAL_NVIC_DisableIRQ(ADS1222_B_DOUT_EXTI_IRQn);

    // configuration as output
    GPIO_InitStruct.Pin = ADS1222_B_DOUT_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(ADS1222_B_DOUT_PORT, &GPIO_InitStruct); 
}

static void Ads1222_SenseC_OutputGpioConfig()
{
    GPIO_InitTypeDef GPIO_InitStruct;

    // disable exti
    HAL_NVIC_DisableIRQ(ADS1222_C_DOUT_EXTI_IRQn);

    // configuration as output
    GPIO_InitStruct.Pin = ADS1222_C_DOUT_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(ADS1222_C_DOUT_PORT, &GPIO_InitStruct); 
}

static void Ads1222_SenseD_OutputGpioConfig()
{
    GPIO_InitTypeDef GPIO_InitStruct;

    // disable exti
    HAL_NVIC_DisableIRQ(ADS1222_D_DOUT_EXTI_IRQn);

    // configuration as output
    GPIO_InitStruct.Pin = ADS1222_D_DOUT_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(ADS1222_D_DOUT_PORT, &GPIO_InitStruct); 
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
 *      2016-09-19 Huang Shengda
 */  
static void ADS1222_DOUT_IPU(u8 chipId)
{    
    if ((chipId & ADS1222_CHIP_A) == ADS1222_CHIP_A)
    {
        Ads1222_SenseA_InputGpioConfig();
    }

    if ((chipId & ADS1222_CHIP_B) == ADS1222_CHIP_B)
    {
        Ads1222_SenseB_InputGpioConfig();
    }

    if ((chipId & ADS1222_CHIP_C) == ADS1222_CHIP_C)
    {
        Ads1222_SenseC_InputGpioConfig();
    }

    if ((chipId & ADS1222_CHIP_D) == ADS1222_CHIP_D)
    {
        Ads1222_SenseD_InputGpioConfig();
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
 *      2016-09-19 Huang Shengda
 */  
static void ADS1222_DOUT_Out_PP(u8 chipId)
{    
    if ((chipId & ADS1222_CHIP_A) == ADS1222_CHIP_A)
    {
        Ads1222_SenseA_OutputGpioConfig();
    }

    if ((chipId & ADS1222_CHIP_B) == ADS1222_CHIP_B)
    {
        Ads1222_SenseB_OutputGpioConfig();
    }

    if ((chipId & ADS1222_CHIP_C) == ADS1222_CHIP_C)
    {
        Ads1222_SenseC_OutputGpioConfig();
    }

    if ((chipId & ADS1222_CHIP_D) == ADS1222_CHIP_D)
    {
        Ads1222_SenseD_OutputGpioConfig();
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
 *      2016-08-10 Huang Shengda
 *      2016-09-19 Huang Shengda: support Sense A/B/C/D
 */  
void Ads1222_EnableExti(u8 chipId)
{
    if ((chipId & ADS1222_CHIP_A) == ADS1222_CHIP_A)
    {
        Ads1222_SenseA_EXTIGpioConfig();
    }

    if ((chipId & ADS1222_CHIP_B) == ADS1222_CHIP_B)
    {
        Ads1222_SenseB_EXTIGpioConfig();
    }

    if ((chipId & ADS1222_CHIP_C) == ADS1222_CHIP_C)
    {
        Ads1222_SenseC_EXTIGpioConfig();
    }

    if ((chipId & ADS1222_CHIP_D) == ADS1222_CHIP_D)
    {
        Ads1222_SenseD_EXTIGpioConfig();
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
 *      2016-08-11 Huang Shengda
 */ 
u8 Ads1222_GetChannel()
{
    if(Ads1222_MUX_PinRead() == GPIO_PIN_RESET) {
        return ADS1222_CH0;
    } else {
        return ADS1222_CH1;
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
 *      2016-08-11 Huang Shengda
 */
void Ads1222_SetChannel(u8 channel)
{
    if(channel == ADS1222_CH0) {
        ADS1222_MUX_LOW();
    } else {
        ADS1222_MUX_HIGH();
    }
}

void Ads1222_ExchangeChannel()
{
    ADS1222_MUX_TOGGLE();
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
 *      2016-09-19 Huang Shengda
 */
void Ads1222_SetExistSense(u8 chip)
{
    _sense_exist_bit = 0x00;
    _sense_exist_bit |= chip;
}

u8 Ads122_GetExistSense()
{
    return _sense_exist_bit;
}

/*******************************************************************************
* º¯ÊýÃû  : ADS1222_AdRead
* Ãè  Êö  : »ñÈ¡ADS1222£®Ò»´ÎÍêÕûµÄÊý¾Ý´«ÊäÎª24bit£¬¸ßÎ»ÏÈ³ö
* Êä  Èë  : chipId: ads1222Ð¾Æ¬±àºÅ
*           channel: ads1222Ð¾Æ¬Í¨µÀºÅ 
* Êä  ³ö  : ÎÞ
* ·µ »Ø Öµ: ¶ÁÈ¡24bit Ô­Ê¼ADÖµ
*******************************************************************************/
Ads1222Err ADS1222_AdRead(u32* data_a, u32* data_b, u32* data_c, u32* data_d)
{
    u8 i = 0;

    ADS1222_DOUT_IPU(0x0f);

    // enter critical section
    // first will disable global interrupt
    // then will suspend all threads
    // taskENTER_CRITICAL();
    osThreadSuspendAll();

    *data_a = 0;
    *data_b = 0;
    *data_c = 0;
    *data_d = 0;

    for(i=0; i<24; i++)
    {
        // shift data
        *data_a = *data_a << 1;
        *data_b = *data_b << 1;
        *data_c = *data_c << 1;
        *data_d = *data_d << 1;

        // sclk put high
        ADS1222_A_SCLK_HIGH();
        ADS1222_B_SCLK_HIGH();
        ADS1222_C_SCLK_HIGH();
        ADS1222_D_SCLK_HIGH();

        delay_us(1);

        // sclk put low
        ADS1222_A_SCLK_LOW();
        ADS1222_B_SCLK_LOW();
        ADS1222_C_SCLK_LOW();
        ADS1222_D_SCLK_LOW();

        delay_us(1);

        // read bit
        if (ADS1222_A_DOUT_IN() == GPIO_PIN_SET)
        {
            *data_a |= 0x00000001;  //or adValue+=1;     
        }
        if (ADS1222_B_DOUT_IN() == GPIO_PIN_SET)
        {
            *data_b |= 0x00000001;  //or adValue+=1;     
        }
        if (ADS1222_C_DOUT_IN() == GPIO_PIN_SET)
        {
            *data_c |= 0x00000001;  //or adValue+=1;     
        }
        if (ADS1222_D_DOUT_IN() == GPIO_PIN_SET)
        {
            *data_d |= 0x00000001;  //or adValue+=1;     
        }
    }

    // sclk put high
    ADS1222_A_SCLK_HIGH();
    ADS1222_B_SCLK_HIGH();
    ADS1222_C_SCLK_HIGH();
    ADS1222_D_SCLK_HIGH();

    delay_us(1);

    // sclk put low
    ADS1222_A_SCLK_LOW();
    ADS1222_B_SCLK_LOW();
    ADS1222_C_SCLK_LOW();
    ADS1222_D_SCLK_LOW();

    delay_us(1);

    // exit critical section
    // first will resume all threads
    // then will re-enable global interrupt
    osThreadResumeAll();
    // taskEXIT_CRITICAL();

    return Ads1222Err_NoErr;
}

/**
  * @brief EXTI line1 detection callbacks
  * @param GPIO_Pin: Specifies the pins connected EXTI line
  * @retval None
  * @par History
  *      2016-09-19 Huang Shengda
  */
void HAL_GPIO_SenseA_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin != ADS1222_A_DOUT_PIN)
    {
        return ;
    }

    if ((_sense_exist_bit & ADS1222_CHIP_A) == 0x00)
    {
        return ;
    }

    _sense_conv_cplt |= ADS1222_CHIP_A;

    if ((_sense_conv_cplt & _sense_exist_bit) == _sense_exist_bit)
    {
        Ads1222_ConvComplete();
    }
}

void HAL_GPIO_SenseB_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin != ADS1222_B_DOUT_PIN)
    {
        return ;
    }

    if ((_sense_exist_bit & ADS1222_CHIP_B) == 0x00)
    {
        return ;
    }

    _sense_conv_cplt |= ADS1222_CHIP_B;

    if ((_sense_conv_cplt & _sense_exist_bit) == _sense_exist_bit)
    {
        Ads1222_ConvComplete();
    }
}

void HAL_GPIO_SenseCD_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin != (ADS1222_C_DOUT_PIN | ADS1222_D_DOUT_PIN))
    {
        return ;
    }

    if ((_sense_exist_bit & ADS1222_CHIP_C) == ADS1222_CHIP_C)
    {
        if (ADS1222_C_DOUT_IN() == GPIO_PIN_RESET)
        {
            _sense_conv_cplt |= ADS1222_CHIP_C;
        }
    }
    
    if ((_sense_exist_bit & ADS1222_CHIP_D) == ADS1222_CHIP_D)
    {
        if (ADS1222_D_DOUT_IN() == GPIO_PIN_RESET)
        {
            _sense_conv_cplt |= ADS1222_CHIP_D;
        }
    }

    if ((_sense_conv_cplt & _sense_exist_bit) == _sense_exist_bit)
    {
        Ads1222_ConvComplete();
    }
}


