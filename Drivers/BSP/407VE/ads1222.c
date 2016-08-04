/*******************************************************************************
* ads1222.c
*
* Copyright (C) 2015-2017 xxx Technology CO.,LTD.
*
* Author : menki
* Version: V1.0.0  2016-8-2 Create
*
* Desc: 实现ADS1222传感器底层驱动程序
*
* Modification: 
*    Date    :  
*    Revision:
*    Author  :
*    Contents:
*
*******************************************************************************/

/* ========================================================================== */
/*                             头文件区                                       */
/* ========================================================================== */
#include "ads1222.h"
#include <stdio.h>
#include <stdlib.h>
#include "cmsis_os.h"
#include "AaInclude.h"
#include "main.h"

/* ========================================================================== */
/*                           宏和类型定义区                                   */
/* ========================================================================== */
#define ADS1222_DelayMs(x)   HAL_Delay(x)
/* ========================================================================== */
/*                          数据结构定义区                                    */
/* ========================================================================== */

/* ========================================================================== */
/*                          函数声明区                                        */
/* ========================================================================== */
static void ADS1222_DOUT_IPU(uint8_t chipId);
static void ADS1222_DOUT_Out_PP(uint8_t chipId);

/* ========================================================================== */
/*                          全局变量定义区                                    */
/* ========================================================================== */

/* ========================================================================== */
/*                          函数定义区                                        */
/* ========================================================================== */
/*static void ADS1222_DelayMs(uint16_t time)
{
    portTickType xDelay = time / portTICK_RATE_MS;
    vTaskDelay( xDelay );
}*/

/**
  * 函数功能: 
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
static void ADS1222_DelayUs(uint16_t time)
{
    uint8_t i;
    //taskENTER_CRITICAL(); //or portENTER_CRITICAL();
    while(time)
    {    
        for (i = 0; i < 25; i++)
        {
        }
        time--;
    }
    //taskEXIT_CRITICAL();  //or portEXIT_CRITICAL();
}

/**
  * 函数功能: ADS1222 初始化函数
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
void ADS1222_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    //DOUT CLK ENABLE
    ADS1222_A_DOUT_GPIO_CLK_ENABLE();
    ADS1222_B_DOUT_GPIO_CLK_ENABLE();
    ADS1222_C_DOUT_GPIO_CLK_ENABLE();
    ADS1222_D_DOUT_GPIO_CLK_ENABLE();
    
    //SCLK CLK ENABLE
    ADS1222_A_SCLK_GPIO_CLK_ENABLE();
    ADS1222_B_SCLK_GPIO_CLK_ENABLE();
    ADS1222_C_SCLK_GPIO_CLK_ENABLE();
    ADS1222_D_SCLK_GPIO_CLK_ENABLE();
    
    //GPIO配置为输出
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
    
    ADS1222_DOUT_Out_PP(ADS1222_CHIP_A);
    ADS1222_A_DOUT_HIGH();  // 拉高GPIO 
    
    ADS1222_DOUT_Out_PP(ADS1222_CHIP_B);
    ADS1222_B_DOUT_HIGH();  // 拉高GPIO 
    
    ADS1222_DOUT_Out_PP(ADS1222_CHIP_C);
    ADS1222_C_DOUT_HIGH();  // 拉高GPIO 
    
    
    ADS1222_DOUT_Out_PP(ADS1222_CHIP_D);
    ADS1222_D_DOUT_HIGH();  // 拉高GPIO 
    
    /* The signal is selected though the input mux, which is controlled by MUX */
    ADS1222_MUX_LOW(); //默认通道0
    
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
  * 函数功能: 使ADS1222_DOUT引脚变为上拉输入模式
  * 输入参数: chipId: ads1222芯片编号
  * 返 回 值: 无
  * 说    明：无
  */
static void ADS1222_DOUT_IPU(uint8_t chipId)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    
    switch(chipId)
    {
        case ADS1222_CHIP_A:
        {
          GPIO_InitStruct.Pin   = ADS1222_A_DOUT_PIN;
          GPIO_InitStruct.Mode  = GPIO_MODE_INPUT;
          GPIO_InitStruct.Pull  = GPIO_PULLUP;
          HAL_GPIO_Init(ADS1222_A_DOUT_PORT, &GPIO_InitStruct);
          break;
        }
        
        case ADS1222_CHIP_B:
        {
          GPIO_InitStruct.Pin   = ADS1222_B_DOUT_PIN;
          GPIO_InitStruct.Mode  = GPIO_MODE_INPUT;
          GPIO_InitStruct.Pull  = GPIO_PULLUP;
          HAL_GPIO_Init(ADS1222_B_DOUT_PORT, &GPIO_InitStruct);
          break;
        }

        case ADS1222_CHIP_C:
        {
          GPIO_InitStruct.Pin   = ADS1222_C_DOUT_PIN;
          GPIO_InitStruct.Mode  = GPIO_MODE_INPUT;
          GPIO_InitStruct.Pull  = GPIO_PULLUP;
          HAL_GPIO_Init(ADS1222_C_DOUT_PORT, &GPIO_InitStruct);
          break;
        }

        case ADS1222_CHIP_D:
        {
          GPIO_InitStruct.Pin   = ADS1222_D_DOUT_PIN;
          GPIO_InitStruct.Mode  = GPIO_MODE_INPUT;
          GPIO_InitStruct.Pull  = GPIO_PULLUP;
          HAL_GPIO_Init(ADS1222_D_DOUT_PORT, &GPIO_InitStruct);
          break;
        }
        
        default:break;
    }
}

/**
  * 函数功能: 使ADS1222_DOUT引脚变为推挽输出模式
  * 输入参数: chipId: ads1222芯片编号
  * 返 回 值: 无
  * 说    明：无
  */
static void ADS1222_DOUT_Out_PP(uint8_t chipId)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    
     switch(chipId)
    {
        case ADS1222_CHIP_A:
        {
          GPIO_InitStruct.Pin = ADS1222_A_DOUT_PIN;
          GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
          GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
          HAL_GPIO_Init(ADS1222_A_DOUT_PORT, &GPIO_InitStruct); 
          break;
        }
        
        case ADS1222_CHIP_B:
        {
          GPIO_InitStruct.Pin = ADS1222_B_DOUT_PIN;
          GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
          GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
          HAL_GPIO_Init(ADS1222_B_DOUT_PORT, &GPIO_InitStruct);
          break;
        }

        case ADS1222_CHIP_C:
        {
          GPIO_InitStruct.Pin = ADS1222_C_DOUT_PIN;
          GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
          GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
          HAL_GPIO_Init(ADS1222_C_DOUT_PORT, &GPIO_InitStruct);
          break;
        }

        case ADS1222_CHIP_D:
        {
          GPIO_InitStruct.Pin = ADS1222_D_DOUT_PIN;
          GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
          GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
          HAL_GPIO_Init(ADS1222_D_DOUT_PORT, &GPIO_InitStruct);
          break;
        }
        default:break;
    }

}


/*******************************************************************************
* 函数名  : ADS1222_AdRead
* 描  述  : 获取ADS1222．一次完整的数据传输为24bit，高位先出
* 输  入  : chipId: ads1222芯片编号
*           channel: ads1222芯片通道号 
* 输  出  : 无
* 返 回 值: 读取24bit 原始AD值
*******************************************************************************/
unsigned long ADS1222_AdRead(uint8_t chipId, uint8_t channel)
{
    uint8_t i = 0;
    unsigned long adValue = 0;
    
    //选择采集通道
    if (channel == ADS1222_CH0)
    {
        ADS1222_MUX_LOW();  //AIN1
    }
    else if (channel == ADS1222_CH1)
    {
        ADS1222_MUX_HIGH(); //AIN2
    }
    
    switch(chipId)
    {
        case ADS1222_CHIP_A:
        {
            ADS1222_A_SCLK_LOW();
            ADS1222_DelayUs(20); 

            //ADS1222_DOUT_Out_PP(chipId); //DOUT配置输出强制拉高
           // ADS1222_A_DOUT_HIGH();
           // ADS1222_DelayUs(1);
            
            ADS1222_DOUT_IPU(chipId);
            
            while(ADS1222_A_DOUT_IN() == GPIO_PIN_SET); //等待AD数据采集完成
            //ADS1222_DelayMs(200);
            
            for(i=0; i<24; i++)
            {
                adValue = adValue<<1;
                
                ADS1222_A_SCLK_HIGH();
                ADS1222_DelayUs(2);
                // sampling when low level
                ADS1222_A_SCLK_LOW();
                
                if (ADS1222_A_DOUT_IN() == GPIO_PIN_SET)
                {
                    adValue|=0x00000001;  //or adValue+=1;     
                }

                ADS1222_DelayUs(2);
            }
            /* 最后一个脉冲结束 */
            ADS1222_A_SCLK_HIGH();
            ADS1222_DelayUs(2);
            ADS1222_A_SCLK_LOW();
            ADS1222_DelayMs(1);
            
            break;
        }
        
        case ADS1222_CHIP_B:
        {
            ADS1222_B_SCLK_LOW();
            ADS1222_DelayUs(20); 

            ADS1222_DOUT_IPU(chipId);
            
            while(ADS1222_B_DOUT_IN() == GPIO_PIN_SET); //等待AD数据采集完成
            //ADS1222_DelayMs(200);
            
            for(i=0; i<24; i++)
            {
                adValue = adValue<<1;
                
                ADS1222_B_SCLK_HIGH();
                ADS1222_DelayUs(2);
                ADS1222_B_SCLK_LOW();
                
                if (ADS1222_B_DOUT_IN() == GPIO_PIN_SET)
                {
                    adValue|=0x00000001;  //or adValue+=1;     
                }

                ADS1222_DelayUs(2);
            }
            /* 最后一个脉冲结束 */
            ADS1222_B_SCLK_HIGH();
            ADS1222_DelayUs(2);
            ADS1222_B_SCLK_LOW();
            ADS1222_DelayMs(1);
            
            break;
        }
        
        case ADS1222_CHIP_C:
        {
            ADS1222_C_SCLK_LOW();
            ADS1222_DelayUs(20); 

            ADS1222_DelayUs(20);
            ADS1222_DOUT_IPU(chipId);
            
            while(ADS1222_C_DOUT_IN() == GPIO_PIN_SET); //等待AD数据采集完成
            //ADS1222_DelayMs(200);
            
            for(i=0; i<24; i++)
            {
                adValue = adValue<<1;
                
                ADS1222_C_SCLK_HIGH();
                ADS1222_DelayUs(2);
                ADS1222_C_SCLK_LOW();
                
                if (ADS1222_C_DOUT_IN() == GPIO_PIN_SET)
                {
                    adValue|=0x00000001;  //or adValue+=1;     
                }

                ADS1222_DelayUs(2);
            }
            /* 最后一个脉冲结束 */
            ADS1222_C_SCLK_HIGH();
            ADS1222_DelayUs(2);
            ADS1222_C_SCLK_LOW();
            ADS1222_DelayMs(1);
            
            break;
        }

        case ADS1222_CHIP_D:
        {
            ADS1222_D_SCLK_LOW();
            ADS1222_DelayUs(20); 

            ADS1222_DelayUs(20);
            ADS1222_DOUT_IPU(chipId);
            
            while(ADS1222_D_DOUT_IN() == GPIO_PIN_SET); //等待AD数据采集完成
            //ADS1222_DelayMs(200);
            
            for(i=0; i<24; i++)
            {
                adValue = adValue<<1;
                
                ADS1222_D_SCLK_HIGH();
                ADS1222_DelayUs(2);
                ADS1222_D_SCLK_LOW();
                
                if (ADS1222_D_DOUT_IN() == GPIO_PIN_SET)
                {
                    adValue|=0x00000001;  //or adValue+=1;     
                }

                ADS1222_DelayUs(2);
            }
            /* 最后一个脉冲结束 */
            ADS1222_D_SCLK_HIGH();
            ADS1222_DelayUs(2);
            ADS1222_D_SCLK_LOW();
            ADS1222_DelayMs(1);
            
            break;
        }
        default:break;
    }

    return adValue;
}

