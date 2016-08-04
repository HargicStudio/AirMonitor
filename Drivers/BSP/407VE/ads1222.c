/*******************************************************************************
* ads1222.c
*
* Copyright (C) 2015-2017 xxx Technology CO.,LTD.
*
* Author : menki
* Version: V1.0.0  2016-8-2 Create
*
* Desc: ʵ��ADS1222�������ײ���������
*
* Modification: 
*    Date    :  
*    Revision:
*    Author  :
*    Contents:
*
*******************************************************************************/

/* ========================================================================== */
/*                             ͷ�ļ���                                       */
/* ========================================================================== */
#include "ads1222.h"
#include <stdio.h>
#include <stdlib.h>
#include "cmsis_os.h"
#include "AaInclude.h"
#include "main.h"

/* ========================================================================== */
/*                           ������Ͷ�����                                   */
/* ========================================================================== */
#define ADS1222_DelayMs(x)   HAL_Delay(x)
/* ========================================================================== */
/*                          ���ݽṹ������                                    */
/* ========================================================================== */

/* ========================================================================== */
/*                          ����������                                        */
/* ========================================================================== */
static void ADS1222_DOUT_IPU(uint8_t chipId);
static void ADS1222_DOUT_Out_PP(uint8_t chipId);

/* ========================================================================== */
/*                          ȫ�ֱ���������                                    */
/* ========================================================================== */

/* ========================================================================== */
/*                          ����������                                        */
/* ========================================================================== */
/*static void ADS1222_DelayMs(uint16_t time)
{
    portTickType xDelay = time / portTICK_RATE_MS;
    vTaskDelay( xDelay );
}*/

/**
  * ��������: 
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
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
  * ��������: ADS1222 ��ʼ������
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
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
    
    //GPIO����Ϊ���
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
    ADS1222_A_DOUT_HIGH();  // ����GPIO 
    
    ADS1222_DOUT_Out_PP(ADS1222_CHIP_B);
    ADS1222_B_DOUT_HIGH();  // ����GPIO 
    
    ADS1222_DOUT_Out_PP(ADS1222_CHIP_C);
    ADS1222_C_DOUT_HIGH();  // ����GPIO 
    
    
    ADS1222_DOUT_Out_PP(ADS1222_CHIP_D);
    ADS1222_D_DOUT_HIGH();  // ����GPIO 
    
    /* The signal is selected though the input mux, which is controlled by MUX */
    ADS1222_MUX_LOW(); //Ĭ��ͨ��0
    
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
  * ��������: ʹADS1222_DOUT���ű�Ϊ��������ģʽ
  * �������: chipId: ads1222оƬ���
  * �� �� ֵ: ��
  * ˵    ������
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
  * ��������: ʹADS1222_DOUT���ű�Ϊ�������ģʽ
  * �������: chipId: ads1222оƬ���
  * �� �� ֵ: ��
  * ˵    ������
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
* ������  : ADS1222_AdRead
* ��  ��  : ��ȡADS1222��һ�����������ݴ���Ϊ24bit����λ�ȳ�
* ��  ��  : chipId: ads1222оƬ���
*           channel: ads1222оƬͨ���� 
* ��  ��  : ��
* �� �� ֵ: ��ȡ24bit ԭʼADֵ
*******************************************************************************/
unsigned long ADS1222_AdRead(uint8_t chipId, uint8_t channel)
{
    uint8_t i = 0;
    unsigned long adValue = 0;
    
    //ѡ��ɼ�ͨ��
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

            //ADS1222_DOUT_Out_PP(chipId); //DOUT�������ǿ������
           // ADS1222_A_DOUT_HIGH();
           // ADS1222_DelayUs(1);
            
            ADS1222_DOUT_IPU(chipId);
            
            while(ADS1222_A_DOUT_IN() == GPIO_PIN_SET); //�ȴ�AD���ݲɼ����
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
            /* ���һ��������� */
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
            
            while(ADS1222_B_DOUT_IN() == GPIO_PIN_SET); //�ȴ�AD���ݲɼ����
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
            /* ���һ��������� */
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
            
            while(ADS1222_C_DOUT_IN() == GPIO_PIN_SET); //�ȴ�AD���ݲɼ����
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
            /* ���һ��������� */
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
            
            while(ADS1222_D_DOUT_IN() == GPIO_PIN_SET); //�ȴ�AD���ݲɼ����
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
            /* ���һ��������� */
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

