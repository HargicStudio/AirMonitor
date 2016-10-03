/*******************************************************************************
* am2302.c
*
* Copyright (C) 2015-2017 xxx Technology CO.,LTD.
*
* Author : menki
* Version: V1.0.0  2016-7-25 Create
*
* Desc: ʵ��AM2302��ʪ�ȴ������ײ���������
*
* Modification: 
*    Date    :  
*    Revision:
*    Author  :
*    Contents:
*
* 1������Ӧ�õ�·�н��������߳��ȶ���30��ʱ��5.1K�������裬����30��ʱ����ʵ��������������������ֵ�� 
* 2��ʹ��3.3V��ѹ����ʱ�����߳��Ȳ��ô���30cm��������·ѹ���ᵼ�´��������粻�㣬��ɲ���ƫ� 
* 3����ȡ��������С���ʱ��Ϊ2S����ȡ���ʱ��С��2S�����ܵ�����ʪ�Ȳ�׼��ͨ�Ų��ɹ�������� 
* 4��ÿ�ζ�������ʪ����ֵ����һ�β����Ľ��������ȡʵʱ���ݣ���������ȡ���Σ� ����������ζ�ȡ��������
*    ��ÿ�ζ�ȡ�������������2�뼴�ɻ��׼ȷ�����ݡ�
*
*******************************************************************************/

/* ========================================================================== */
/*                             ͷ�ļ���                                       */
/* ========================================================================== */
#include "am2302.h"
#include <stdio.h>
#include <stdlib.h>
#include "cmsis_os.h"
#include "AaInclude.h"
#include "main.h"
#include "dataHandler.h"

/* ========================================================================== */
/*                           ������Ͷ�����                                   */
/* ========================================================================== */
#define Delay_ms(x)   HAL_Delay(x)
/* ========================================================================== */
/*                          ���ݽṹ������                                    */
/* ========================================================================== */

/* ========================================================================== */
/*                          ����������                                        */
/* ========================================================================== */
static void AM2302_Mode_IPU(uint8_t channel);
static void AM2302_Mode_Out_PP(uint8_t channel);
static uint8_t AM2302_ReadByte(uint8_t channel);
/* ========================================================================== */
/*                          ȫ�ֱ���������                                    */
/* ========================================================================== */
/** RunAm2302Thread handler id */  
osThreadId _runAm2302_id;


/* ========================================================================== */
/*                          ����������                                        */
/* ========================================================================== */
/*static void Delay_ms(uint16_t time)
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
static void AM2302_Delay(uint16_t time)
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
  * ��������: AM2302 ��ʼ������
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  */
void AM2302_Init(uint8_t channel)
{
     if (channel == AM2302_CH0)
    {
        AM2302_CH0_Dout_GPIO_CLK_ENABLE();
        AM2302_Mode_Out_PP(channel);
        AM2302_CH0_Dout_HIGH();  // ����GPIO
    }
    else if (channel == AM2302_CH1)
    {
        AM2302_CH1_Dout_GPIO_CLK_ENABLE();
        AM2302_Mode_Out_PP(channel);
        AM2302_CH1_Dout_HIGH();  // ����GPIO    
    }

}


/**
  * ��������: ʹAM2302-DATA���ű�Ϊ��������ģʽ
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  */
static void AM2302_Mode_IPU(uint8_t channel)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    /* GPIO���� */
    if (channel == AM2302_CH0)
    {
        GPIO_InitStruct.Pin   = AM2302_CH0_Dout_PIN;
        GPIO_InitStruct.Mode  = GPIO_MODE_INPUT;
        GPIO_InitStruct.Pull  = GPIO_PULLUP;
        HAL_GPIO_Init(AM2302_CH0_Dout_PORT, &GPIO_InitStruct);
    }
    else if (channel == AM2302_CH1)
    {
        GPIO_InitStruct.Pin   = AM2302_CH1_Dout_PIN;
        GPIO_InitStruct.Mode  = GPIO_MODE_INPUT;
        GPIO_InitStruct.Pull  = GPIO_PULLUP;
        HAL_GPIO_Init(AM2302_CH1_Dout_PORT, &GPIO_InitStruct);
    }
}

/**
  * ��������: ʹAM2302-DATA���ű�Ϊ�������ģʽ
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  */
static void AM2302_Mode_Out_PP(uint8_t channel)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    /* GPIO���� */	 
    if (channel == AM2302_CH0)
    {
        GPIO_InitStruct.Pin = AM2302_CH0_Dout_PIN;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(AM2302_CH0_Dout_PORT, &GPIO_InitStruct); 
    }
    else if (channel == AM2302_CH1)
    {
        GPIO_InitStruct.Pin = AM2302_CH1_Dout_PIN;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(AM2302_CH1_Dout_PORT, &GPIO_InitStruct); 
    }
}

/**
  * ��������: ��AM2302��ȡһ���ֽڣ�MSB����
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  */
static uint8_t AM2302_ReadByte(uint8_t channel)
{
	uint8_t i, temp=0;
	
     if (channel == AM2302_CH0)
    {
        for(i=0; i<8; i++)    
        {	 
            /*ÿbit��50us�͵�ƽ���ÿ�ʼ����ѯֱ���ӻ����� ��50us �͵�ƽ ����*/  
            while(AM2302_CH0_Data_IN()==GPIO_PIN_RESET);

            /*AM2302 ��26~28us�ĸߵ�ƽ��ʾ��0������70us�ߵ�ƽ��ʾ��1����
             *ͨ����� x us��ĵ�ƽ��������������״ ��x ���������ʱ 
             */
            AM2302_Delay(40); //��ʱx us �����ʱ��Ҫ��������0������ʱ�伴��	   	  

            if(AM2302_CH0_Data_IN()==GPIO_PIN_SET)/* x us����Ϊ�ߵ�ƽ��ʾ���ݡ�1�� */
            {
                /* �ȴ�����1�ĸߵ�ƽ���� */
                while(AM2302_CH0_Data_IN()==GPIO_PIN_SET);

                temp|=(uint8_t)(0x01<<(7-i));  //�ѵ�7-iλ��1��MSB���� 
            }
            else	 // x us��Ϊ�͵�ƽ��ʾ���ݡ�0��
            {			   
                temp&=(uint8_t)~(0x01<<(7-i)); //�ѵ�7-iλ��0��MSB����
            }
        }
    }
    else if (channel == AM2302_CH1)
    {
        for(i=0; i<8; i++)    
        {	 
            /*ÿbit��50us�͵�ƽ���ÿ�ʼ����ѯֱ���ӻ����� ��50us �͵�ƽ ����*/  
            while(AM2302_CH1_Data_IN()==GPIO_PIN_RESET);

            /*AM2302 ��26~28us�ĸߵ�ƽ��ʾ��0������70us�ߵ�ƽ��ʾ��1����
             *ͨ����� x us��ĵ�ƽ��������������״ ��x ���������ʱ 
             */
            AM2302_Delay(40); //��ʱx us �����ʱ��Ҫ��������0������ʱ�伴��	   	  

            if(AM2302_CH1_Data_IN()==GPIO_PIN_SET)/* x us����Ϊ�ߵ�ƽ��ʾ���ݡ�1�� */
            {
                /* �ȴ�����1�ĸߵ�ƽ���� */
                while(AM2302_CH1_Data_IN()==GPIO_PIN_SET);

                temp|=(uint8_t)(0x01<<(7-i));  //�ѵ�7-iλ��1��MSB���� 
            }
            else	 // x us��Ϊ�͵�ƽ��ʾ���ݡ�0��
            {			   
                temp&=(uint8_t)~(0x01<<(7-i)); //�ѵ�7-iλ��0��MSB����
            }
        }
    }
    
	return temp;
    
}


/**
  * ��������: һ�����������ݴ���Ϊ40bit����λ�ȳ�
  * �������: AM2302_Data:AM2302��������
  * �� �� ֵ: ERROR��  ��ȡ����
  *           SUCCESS����ȡ�ɹ�
  * ˵    ����8bit ʪ������ + 8bit ʪ��С�� + 8bit �¶����� + 8bit �¶�С�� + 8bit У��� 
  */
uint8_t AM2302_CH0_Read_TempAndHumidity(AM2302_Data_TypeDef *AM2302_Data)
{  
    uint8_t temp;
    uint16_t humi_temp;
    //u32 cnt = 0;
  
	/*���ģʽ*/
	AM2302_Mode_Out_PP(AM2302_CH0);
	/*��������*/
	AM2302_CH0_Dout_LOW();
	/*��ʱ18ms*/
	Delay_ms(18);
    
        vTaskSuspendAll();  //�رյ���
 
	/*�������� ������ʱ30us*/
	AM2302_CH0_Dout_HIGH(); 

	AM2302_Delay(30);   //��ʱ30us

	/*������Ϊ���� �жϴӻ���Ӧ�ź�*/ 
	AM2302_Mode_IPU(AM2302_CH0);

	/*�жϴӻ��Ƿ��е͵�ƽ��Ӧ�ź� �粻��Ӧ����������Ӧ����������*/   
	if(AM2302_CH0_Data_IN()==GPIO_PIN_RESET)     
	{
            /*��ѯֱ���ӻ����� ��80us �͵�ƽ ��Ӧ�źŽ���*/  
            while(AM2302_CH0_Data_IN()==GPIO_PIN_RESET)
            {
                
            }

            /*��ѯֱ���ӻ������� 80us �ߵ�ƽ �����źŽ���*/
            while(AM2302_CH0_Data_IN()==GPIO_PIN_SET)
            {
            }

            /*��ʼ��������*/   
            AM2302_Data->humi_high8bit= AM2302_ReadByte(AM2302_CH0);
            AM2302_Data->humi_low8bit = AM2302_ReadByte(AM2302_CH0);
            AM2302_Data->temp_high8bit= AM2302_ReadByte(AM2302_CH0);
            AM2302_Data->temp_low8bit = AM2302_ReadByte(AM2302_CH0);
            AM2302_Data->check_sum    = AM2302_ReadByte(AM2302_CH0);

            /*��ȡ���������Ÿ�Ϊ���ģʽ*/
            AM2302_Mode_Out_PP(AM2302_CH0);
            /*��������*/
            AM2302_CH0_Dout_HIGH();
            
            /* �����ݽ��д��� */
            humi_temp=AM2302_Data->humi_high8bit*256+AM2302_Data->humi_low8bit;
            AM2302_Data->humidity = humi_temp;   
            humi_temp=AM2302_Data->temp_high8bit*256+AM2302_Data->temp_low8bit;
            AM2302_Data->temperature= humi_temp; 
            
            xTaskResumeAll(); //���¿�������
            
            /*����ȡ�������Ƿ���ȷ*/
            temp = AM2302_Data->humi_high8bit + AM2302_Data->humi_low8bit + 
                   AM2302_Data->temp_high8bit+ AM2302_Data->temp_low8bit;
            
            if(AM2302_Data->check_sum==temp)
            { 
              return SUCCESS;
            }
            else 
            {
              return ERROR;
            }
	}	
	else
        {
            xTaskResumeAll();
            return ERROR;
        }
}


/**
  * ��������: һ�����������ݴ���Ϊ40bit����λ�ȳ�
  * �������: AM2302_Data:AM2302��������
  * �� �� ֵ: ERROR��  ��ȡ����
  *           SUCCESS����ȡ�ɹ�
  * ˵    ����8bit ʪ������ + 8bit ʪ��С�� + 8bit �¶����� + 8bit �¶�С�� + 8bit У��� 
  */
uint8_t AM2302_CH1_Read_TempAndHumidity(AM2302_Data_TypeDef *AM2302_Data)
{  
    uint8_t temp;
    uint16_t humi_temp;
  
	/*���ģʽ*/
	AM2302_Mode_Out_PP(AM2302_CH1);
	/*��������*/
	AM2302_CH1_Dout_LOW();
	/*��ʱ18ms*/
	Delay_ms(18);

	/*�������� ������ʱ30us*/
	AM2302_CH1_Dout_HIGH(); 
    
        vTaskSuspendAll();  //�رյ���
    
	AM2302_Delay(30);   //��ʱ30us

	/*������Ϊ���� �жϴӻ���Ӧ�ź�*/ 
	AM2302_Mode_IPU(AM2302_CH1);
 
	/*�жϴӻ��Ƿ��е͵�ƽ��Ӧ�ź� �粻��Ӧ����������Ӧ����������*/   
	if(AM2302_CH1_Data_IN()==GPIO_PIN_RESET)     
	{
        /*��ѯֱ���ӻ����� ��80us �͵�ƽ ��Ӧ�źŽ���*/  
        while(AM2302_CH1_Data_IN()==GPIO_PIN_RESET);

        /*��ѯֱ���ӻ������� 80us �ߵ�ƽ �����źŽ���*/
        while(AM2302_CH1_Data_IN()==GPIO_PIN_SET);

        /*��ʼ��������*/   
        AM2302_Data->humi_high8bit= AM2302_ReadByte(AM2302_CH1);
        AM2302_Data->humi_low8bit = AM2302_ReadByte(AM2302_CH1);
        AM2302_Data->temp_high8bit= AM2302_ReadByte(AM2302_CH1);
        AM2302_Data->temp_low8bit = AM2302_ReadByte(AM2302_CH1);
        AM2302_Data->check_sum    = AM2302_ReadByte(AM2302_CH1);

        /*��ȡ���������Ÿ�Ϊ���ģʽ*/
        AM2302_Mode_Out_PP(AM2302_CH1);
        /*��������*/
        AM2302_CH1_Dout_HIGH();
        
        /* �����ݽ��д��� */
        humi_temp=AM2302_Data->humi_high8bit*256+AM2302_Data->humi_low8bit;
        AM2302_Data->humidity = humi_temp;   
        humi_temp=AM2302_Data->temp_high8bit*256+AM2302_Data->temp_low8bit;
        AM2302_Data->temperature= humi_temp;    
        
        /*����ȡ�������Ƿ���ȷ*/
        temp = AM2302_Data->humi_high8bit + AM2302_Data->humi_low8bit + 
               AM2302_Data->temp_high8bit+ AM2302_Data->temp_low8bit;
        
        xTaskResumeAll(); //���¿�������
          
        if(AM2302_Data->check_sum==temp)
        { 
          return SUCCESS;
        }
        else 
        {
          return ERROR;
        }
	}	
	else
    {
        xTaskResumeAll();
		return ERROR;
    }
}


/*******************************************************************************
* ������  : AM2302_Read_TempAndHumidity
* ��  ��  : ��ȡ��ʪ�ȴ�������һ�����������ݴ���Ϊ40bit����λ�ȳ�
*           8bitʪ������ + 8bitʪ��С�� + 8bit�¶����� + 8bit�¶�С�� + 8bitУ��� 
* ��  ��  : channel:ͨ���� 
*           AM2302_Data:AM2302_Data_TypeDef��������
* ��  ��  : ��
* �� �� ֵ: ERROR��  ��ȡ����
*           SUCCESS����ȡ�ɹ�
*******************************************************************************/
uint8_t AM2302_Read_TempAndHumidity(uint8_t channel, AM2302_Data_TypeDef * AM2302_Data)
{
    if (channel == AM2302_CH0)
    {
        return AM2302_CH0_Read_TempAndHumidity(AM2302_Data);
    }
    else if (channel == AM2302_CH1)
    {
        return AM2302_CH1_Read_TempAndHumidity(AM2302_Data);
    }
    else
    {
		return ERROR;
    }
}


/**
  * @brief  run am2302 thread
  * @param  thread not used
  * @retval None
  */
static void RunAm2302Thread(void const *argument)
{
  (void) argument;
  
  AM2302_Data_TypeDef AM2302_Data;
  
  AaSysLogPrintF(LOGLEVEL_INF, FeatureAm2303, "============AM2302_Init============\n\r");
  //��ʼ��ͨ��0
  AM2302_Init(AM2302_CH0);
  //��ʼ��ͨ��1
  AM2302_Init(AM2302_CH1);
  
  /* works after 10 seconds */
  osDelay(10000);
  
  for (;;)
  {
      /* ÿ2s�ɼ�һ�� */
      osDelay(2000);
      
       /* ͨ��0 */
      if(AM2302_Read_TempAndHumidity(AM2302_CH0, &AM2302_Data)==SUCCESS)
      {
        AaSysLogPrintF(LOGLEVEL_DBG, FeatureAm2303, "[CH0]read AM2302 successful!-->hum=%d.%d RH, temp=%d.%d C\n",
          AM2302_Data.humidity/10, AM2302_Data.humidity%10, AM2302_Data.temperature/10, AM2302_Data.temperature%10);
        
        StoreWetTempInfo(AM2302_Data.humidity, AM2302_Data.temperature, &g_tempWetIn);
      }
      else
      {
        AaSysLogPrintF(LOGLEVEL_INF, FeatureAm2303, "[CH0]read AM2302 failed!\n");      
      }
      
      /* ͨ��1 *//*
      if(AM2302_Read_TempAndHumidity(AM2302_CH1, &AM2302_Data)==SUCCESS)
      {
        AaSysLogPrintF(LOGLEVEL_INF, FeatureAm2303, "[CH1]read AM2302 successful!-->hum=%d.%d RH, temp=%d.%d C\n",
           AM2302_Data.humidity/10, AM2302_Data.humidity%10, AM2302_Data.temperature/10, AM2302_Data.temperature%10);
      }
      else
      {
        AaSysLogPrintF(LOGLEVEL_INF, FeatureAm2303, "[CH1]read AM2302 failed!\n");      
      }*/

      
  }
}


/**
  * @brief  start am2302 from system level
  * @param  none
  * @retval None
  */
void StartRunAm2302Task()
{
    //�����ȼ�
    osThreadDef(RunAm2302, RunAm2302Thread, osPriorityRealtime, 0, 2*configMINIMAL_STACK_SIZE);
    _runAm2302_id = AaThreadCreateStartup(osThread(RunAm2302), NULL);
}
