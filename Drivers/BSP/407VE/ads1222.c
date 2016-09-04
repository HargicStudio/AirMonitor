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



static void delay_us(uint16_t us);
static void delay_10us(uint16_t _10us);

static void Ads1222_SenseA_EXTIGpioConfig();
static void Ads1222_SenseA_InputGpioConfig();
static void Ads1222_SenseA_OutputGpioConfig();



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
 */  
static void Ads1222_SenseA_EXTIGpioConfig()
{
    GPIO_InitTypeDef   GPIO_InitStructure;

    /* Enable /DRDY clock */
    ADS1222_A_DOUT_GPIO_CLK_ENABLE();

    /* Configure PA0 pin as input floating */
    GPIO_InitStructure.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    GPIO_InitStructure.Pin = ADS1222_A_DOUT_PIN;
    HAL_GPIO_Init(ADS1222_A_DOUT_PORT, &GPIO_InitStructure);

    /* Enable and set EXTI Line0 Interrupt to the lowest priority */
    // !!! this is very important
    // !!! we must clear EXTI bit first or it will trigger next interrupt when IRQ enabled
    __HAL_GPIO_EXTI_CLEAR_IT(ADS1222_A_DOUT_PIN);
    HAL_NVIC_SetPriority(ADS1222_A_DOUT_EXTI_IRQn, SYSTEM_IRQ_PRIORITY_HIGH_2, 0);
    HAL_NVIC_EnableIRQ(ADS1222_A_DOUT_EXTI_IRQn);
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

/**
  * º¯Êý¹¦ÄÜ: Ê¹ADS1222_DOUTÒý½Å±äÎªÉÏÀ­ÊäÈëÄ£Ê½
  * ÊäÈë²ÎÊý: chipId: ads1222Ð¾Æ¬±àºÅ
  * ·µ »Ø Öµ: ÎÞ
  * Ëµ    Ã÷£ºÎÞ
  */
void ADS1222_DOUT_IPU(uint8_t chipId)
{    
    switch(chipId) {
        case ADS1222_CHIP_A: Ads1222_SenseA_InputGpioConfig(); break;
        case ADS1222_CHIP_B: break;
        case ADS1222_CHIP_C: break;
        case ADS1222_CHIP_D: break;
        default:break;
    }
}

/**
  * º¯Êý¹¦ÄÜ: Ê¹ADS1222_DOUTÒý½Å±äÎªÍÆÍìÊä³öÄ£Ê½
  * ÊäÈë²ÎÊý: chipId: ads1222Ð¾Æ¬±àºÅ
  * ·µ »Ø Öµ: ÎÞ
  * Ëµ    Ã÷£ºÎÞ
  */
void ADS1222_DOUT_Out_PP(uint8_t chipId)
{    
     switch(chipId) {
        case ADS1222_CHIP_A: Ads1222_SenseA_OutputGpioConfig(); break;
        case ADS1222_CHIP_B: break;
        case ADS1222_CHIP_C: break;
        case ADS1222_CHIP_D: break;
        default:break;
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
void Ads1222_EnableExti()
{
    Ads1222_SenseA_EXTIGpioConfig();
}

/*******************************************************************************
* º¯ÊýÃû  : ADS1222_AdRead
* Ãè  Êö  : »ñÈ¡ADS1222£®Ò»´ÎÍêÕûµÄÊý¾Ý´«ÊäÎª24bit£¬¸ßÎ»ÏÈ³ö
* Êä  Èë  : chipId: ads1222Ð¾Æ¬±àºÅ
*           channel: ads1222Ð¾Æ¬Í¨µÀºÅ 
* Êä  ³ö  : ÎÞ
* ·µ »Ø Öµ: ¶ÁÈ¡24bit Ô­Ê¼ADÖµ
*******************************************************************************/
unsigned long ADS1222_AdRead(uint8_t chipId)
{
    uint8_t i = 0;
    unsigned long adValue = 0;

    // enter critical section
    // first will disable global interrupt
    // then will suspend all threads
    // taskENTER_CRITICAL();
    osThreadSuspendAll();

    // //Ñ¡Ôñ²É¼¯Í¨µÀ
    // if (channel == ADS1222_CH0) {
    //     ADS1222_MUX_LOW();  //AIN1
    // } else if (channel == ADS1222_CH1) {
    //     ADS1222_MUX_HIGH(); //AIN2
    // }
    // delay_us(2);
    
    switch(chipId) {
        case ADS1222_CHIP_A:
        {
            // ADS1222_A_SCLK_LOW();
            // delay_10us(2); 

            // ADS1222_DOUT_Out_PP(chipId); //DOUTÅäÖÃÊä³öÇ¿ÖÆÀ­¸ß
            // ADS1222_A_DOUT_HIGH();
            // delay_us(1);
            
            ADS1222_DOUT_IPU(chipId);
            
            // while(ADS1222_A_DOUT_IN() == GPIO_PIN_SET); //µÈ´ýADÊý¾Ý²É¼¯Íê³É
            // ADS1222_DelayMs(200);
            
            for(i=0; i<24; i++) {
                adValue = adValue << 1;
                
                ADS1222_A_SCLK_HIGH();
                delay_us(2);
                // sampling when low level
                ADS1222_A_SCLK_LOW();
                
                if (ADS1222_A_DOUT_IN() == GPIO_PIN_SET) {
                    adValue |= 0x00000001;  //or adValue+=1;     
                }
                delay_us(2);
            }
            /* ×îºóÒ»¸öÂö³å½áÊø */
            ADS1222_A_SCLK_HIGH();
            delay_us(2);
            ADS1222_A_SCLK_LOW();
            delay_us(1);
            
            break;
        }
        
        case ADS1222_CHIP_B:
        {
            // ADS1222_B_SCLK_LOW();
            // delay_us(20); 

            // ADS1222_DOUT_IPU(chipId);
            
            // while(ADS1222_B_DOUT_IN() == GPIO_PIN_SET); //µÈ´ýADÊý¾Ý²É¼¯Íê³É
            // ADS1222_DelayMs(200);
            
            for(i=0; i<24; i++) {
                adValue = adValue << 1;
                
                ADS1222_B_SCLK_HIGH();
                delay_us(2);
                ADS1222_B_SCLK_LOW();
                
                if (ADS1222_B_DOUT_IN() == GPIO_PIN_SET) {
                    adValue |= 0x00000001;  //or adValue+=1;     
                }
                delay_us(2);
            }
            /* ×îºóÒ»¸öÂö³å½áÊø */
            ADS1222_B_SCLK_HIGH();
            delay_us(2);
            ADS1222_B_SCLK_LOW();
            delay_us(1);
            
            break;
        }
        
        case ADS1222_CHIP_C:
        {
            // ADS1222_C_SCLK_LOW();
            // delay_us(20); 

            // delay_us(20);
            // ADS1222_DOUT_IPU(chipId);
            
            // while(ADS1222_C_DOUT_IN() == GPIO_PIN_SET); //µÈ´ýADÊý¾Ý²É¼¯Íê³É
            // ADS1222_DelayMs(200);
            
            for(i=0; i<24; i++) {
                adValue = adValue << 1;
                
                ADS1222_C_SCLK_HIGH();
                delay_us(2);
                ADS1222_C_SCLK_LOW();
                
                if (ADS1222_C_DOUT_IN() == GPIO_PIN_SET) {
                    adValue |= 0x00000001;  //or adValue+=1;     
                }
                delay_us(2);
            }
            /* ×îºóÒ»¸öÂö³å½áÊø */
            ADS1222_C_SCLK_HIGH();
            delay_us(2);
            ADS1222_C_SCLK_LOW();
            delay_us(1);
            
            break;
        }

        case ADS1222_CHIP_D:
        {
            // ADS1222_D_SCLK_LOW();
            // delay_us(20); 

            // delay_us(20);
            // ADS1222_DOUT_IPU(chipId);
            
            // while(ADS1222_D_DOUT_IN() == GPIO_PIN_SET); //µÈ´ýADÊý¾Ý²É¼¯Íê³É
            // ADS1222_DelayMs(200);
            
            for(i=0; i<24; i++) {
                adValue = adValue << 1;
                
                ADS1222_D_SCLK_HIGH();
                delay_us(2);
                ADS1222_D_SCLK_LOW();
                
                if (ADS1222_D_DOUT_IN() == GPIO_PIN_SET) {
                    adValue |= 0x00000001;  //or adValue+=1;     
                }

                delay_us(2);
            }
            /* ×îºóÒ»¸öÂö³å½áÊø */
            ADS1222_D_SCLK_HIGH();
            delay_us(2);
            ADS1222_D_SCLK_LOW();
            delay_us(1);
            
            break;
        }
        default: break;
    }

    // exit critical section
    // first will resume all threads
    // then will re-enable global interrupt
    osThreadResumeAll();
    // taskEXIT_CRITICAL();

    return adValue;
}

/**
  * @brief EXTI line detection callbacks
  * @param GPIO_Pin: Specifies the pins connected EXTI line
  * @retval None
  */
void HAL_GPIO_SenseA_EXTI_Callback(uint16_t GPIO_Pin)
{
    Ads1222_ConvComplete();
}

