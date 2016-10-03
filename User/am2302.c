/*******************************************************************************
* am2302.c
*
* Copyright (C) 2015-2017 xxx Technology CO.,LTD.
*
* Author : menki
* Version: V1.0.0  2016-7-25 Create
*
* Desc: 实现AM2302温湿度传感器底层驱动程序
*
* Modification: 
*    Date    :  
*    Revision:
*    Author  :
*    Contents:
*
* 1．典型应用电路中建议连接线长度短于30米时用5.1K上拉电阻，大于30米时根据实际情况降低上拉电阻的阻值。 
* 2．使用3.3V电压供电时连接线长度不得大于30cm。否则线路压降会导致传感器供电不足，造成测量偏差。 
* 3．读取传感器最小间隔时间为2S；读取间隔时间小于2S，可能导致温湿度不准或通信不成功等情况。 
* 4．每次读出的温湿度数值是上一次测量的结果，欲获取实时数据，需连续读取两次， 建议连续多次读取传感器，
*    且每次读取传感器间隔大于2秒即可获得准确的数据。
*
*******************************************************************************/

/* ========================================================================== */
/*                             头文件区                                       */
/* ========================================================================== */
#include "am2302.h"
#include <stdio.h>
#include <stdlib.h>
#include "cmsis_os.h"
#include "AaInclude.h"
#include "main.h"
#include "dataHandler.h"

/* ========================================================================== */
/*                           宏和类型定义区                                   */
/* ========================================================================== */
#define Delay_ms(x)   HAL_Delay(x)
/* ========================================================================== */
/*                          数据结构定义区                                    */
/* ========================================================================== */

/* ========================================================================== */
/*                          函数声明区                                        */
/* ========================================================================== */
static void AM2302_Mode_IPU(uint8_t channel);
static void AM2302_Mode_Out_PP(uint8_t channel);
static uint8_t AM2302_ReadByte(uint8_t channel);
/* ========================================================================== */
/*                          全局变量定义区                                    */
/* ========================================================================== */
/** RunAm2302Thread handler id */  
osThreadId _runAm2302_id;


/* ========================================================================== */
/*                          函数定义区                                        */
/* ========================================================================== */
/*static void Delay_ms(uint16_t time)
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
  * 函数功能: AM2302 初始化函数
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
void AM2302_Init(uint8_t channel)
{
     if (channel == AM2302_CH0)
    {
        AM2302_CH0_Dout_GPIO_CLK_ENABLE();
        AM2302_Mode_Out_PP(channel);
        AM2302_CH0_Dout_HIGH();  // 拉高GPIO
    }
    else if (channel == AM2302_CH1)
    {
        AM2302_CH1_Dout_GPIO_CLK_ENABLE();
        AM2302_Mode_Out_PP(channel);
        AM2302_CH1_Dout_HIGH();  // 拉高GPIO    
    }

}


/**
  * 函数功能: 使AM2302-DATA引脚变为上拉输入模式
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
static void AM2302_Mode_IPU(uint8_t channel)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    /* GPIO配置 */
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
  * 函数功能: 使AM2302-DATA引脚变为推挽输出模式
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
static void AM2302_Mode_Out_PP(uint8_t channel)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    /* GPIO配置 */	 
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
  * 函数功能: 从AM2302读取一个字节，MSB先行
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
static uint8_t AM2302_ReadByte(uint8_t channel)
{
	uint8_t i, temp=0;
	
     if (channel == AM2302_CH0)
    {
        for(i=0; i<8; i++)    
        {	 
            /*每bit以50us低电平标置开始，轮询直到从机发出 的50us 低电平 结束*/  
            while(AM2302_CH0_Data_IN()==GPIO_PIN_RESET);

            /*AM2302 以26~28us的高电平表示“0”，以70us高电平表示“1”，
             *通过检测 x us后的电平即可区别这两个状 ，x 即下面的延时 
             */
            AM2302_Delay(40); //延时x us 这个延时需要大于数据0持续的时间即可	   	  

            if(AM2302_CH0_Data_IN()==GPIO_PIN_SET)/* x us后仍为高电平表示数据“1” */
            {
                /* 等待数据1的高电平结束 */
                while(AM2302_CH0_Data_IN()==GPIO_PIN_SET);

                temp|=(uint8_t)(0x01<<(7-i));  //把第7-i位置1，MSB先行 
            }
            else	 // x us后为低电平表示数据“0”
            {			   
                temp&=(uint8_t)~(0x01<<(7-i)); //把第7-i位置0，MSB先行
            }
        }
    }
    else if (channel == AM2302_CH1)
    {
        for(i=0; i<8; i++)    
        {	 
            /*每bit以50us低电平标置开始，轮询直到从机发出 的50us 低电平 结束*/  
            while(AM2302_CH1_Data_IN()==GPIO_PIN_RESET);

            /*AM2302 以26~28us的高电平表示“0”，以70us高电平表示“1”，
             *通过检测 x us后的电平即可区别这两个状 ，x 即下面的延时 
             */
            AM2302_Delay(40); //延时x us 这个延时需要大于数据0持续的时间即可	   	  

            if(AM2302_CH1_Data_IN()==GPIO_PIN_SET)/* x us后仍为高电平表示数据“1” */
            {
                /* 等待数据1的高电平结束 */
                while(AM2302_CH1_Data_IN()==GPIO_PIN_SET);

                temp|=(uint8_t)(0x01<<(7-i));  //把第7-i位置1，MSB先行 
            }
            else	 // x us后为低电平表示数据“0”
            {			   
                temp&=(uint8_t)~(0x01<<(7-i)); //把第7-i位置0，MSB先行
            }
        }
    }
    
	return temp;
    
}


/**
  * 函数功能: 一次完整的数据传输为40bit，高位先出
  * 输入参数: AM2302_Data:AM2302数据类型
  * 返 回 值: ERROR：  读取出错
  *           SUCCESS：读取成功
  * 说    明：8bit 湿度整数 + 8bit 湿度小数 + 8bit 温度整数 + 8bit 温度小数 + 8bit 校验和 
  */
uint8_t AM2302_CH0_Read_TempAndHumidity(AM2302_Data_TypeDef *AM2302_Data)
{  
    uint8_t temp;
    uint16_t humi_temp;
    //u32 cnt = 0;
  
	/*输出模式*/
	AM2302_Mode_Out_PP(AM2302_CH0);
	/*主机拉低*/
	AM2302_CH0_Dout_LOW();
	/*延时18ms*/
	Delay_ms(18);
    
        vTaskSuspendAll();  //关闭调度
 
	/*总线拉高 主机延时30us*/
	AM2302_CH0_Dout_HIGH(); 

	AM2302_Delay(30);   //延时30us

	/*主机设为输入 判断从机响应信号*/ 
	AM2302_Mode_IPU(AM2302_CH0);

	/*判断从机是否有低电平响应信号 如不响应则跳出，响应则向下运行*/   
	if(AM2302_CH0_Data_IN()==GPIO_PIN_RESET)     
	{
            /*轮询直到从机发出 的80us 低电平 响应信号结束*/  
            while(AM2302_CH0_Data_IN()==GPIO_PIN_RESET)
            {
                
            }

            /*轮询直到从机发出的 80us 高电平 标置信号结束*/
            while(AM2302_CH0_Data_IN()==GPIO_PIN_SET)
            {
            }

            /*开始接收数据*/   
            AM2302_Data->humi_high8bit= AM2302_ReadByte(AM2302_CH0);
            AM2302_Data->humi_low8bit = AM2302_ReadByte(AM2302_CH0);
            AM2302_Data->temp_high8bit= AM2302_ReadByte(AM2302_CH0);
            AM2302_Data->temp_low8bit = AM2302_ReadByte(AM2302_CH0);
            AM2302_Data->check_sum    = AM2302_ReadByte(AM2302_CH0);

            /*读取结束，引脚改为输出模式*/
            AM2302_Mode_Out_PP(AM2302_CH0);
            /*主机拉高*/
            AM2302_CH0_Dout_HIGH();
            
            /* 对数据进行处理 */
            humi_temp=AM2302_Data->humi_high8bit*256+AM2302_Data->humi_low8bit;
            AM2302_Data->humidity = humi_temp;   
            humi_temp=AM2302_Data->temp_high8bit*256+AM2302_Data->temp_low8bit;
            AM2302_Data->temperature= humi_temp; 
            
            xTaskResumeAll(); //重新开启调度
            
            /*检查读取的数据是否正确*/
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
  * 函数功能: 一次完整的数据传输为40bit，高位先出
  * 输入参数: AM2302_Data:AM2302数据类型
  * 返 回 值: ERROR：  读取出错
  *           SUCCESS：读取成功
  * 说    明：8bit 湿度整数 + 8bit 湿度小数 + 8bit 温度整数 + 8bit 温度小数 + 8bit 校验和 
  */
uint8_t AM2302_CH1_Read_TempAndHumidity(AM2302_Data_TypeDef *AM2302_Data)
{  
    uint8_t temp;
    uint16_t humi_temp;
  
	/*输出模式*/
	AM2302_Mode_Out_PP(AM2302_CH1);
	/*主机拉低*/
	AM2302_CH1_Dout_LOW();
	/*延时18ms*/
	Delay_ms(18);

	/*总线拉高 主机延时30us*/
	AM2302_CH1_Dout_HIGH(); 
    
        vTaskSuspendAll();  //关闭调度
    
	AM2302_Delay(30);   //延时30us

	/*主机设为输入 判断从机响应信号*/ 
	AM2302_Mode_IPU(AM2302_CH1);
 
	/*判断从机是否有低电平响应信号 如不响应则跳出，响应则向下运行*/   
	if(AM2302_CH1_Data_IN()==GPIO_PIN_RESET)     
	{
        /*轮询直到从机发出 的80us 低电平 响应信号结束*/  
        while(AM2302_CH1_Data_IN()==GPIO_PIN_RESET);

        /*轮询直到从机发出的 80us 高电平 标置信号结束*/
        while(AM2302_CH1_Data_IN()==GPIO_PIN_SET);

        /*开始接收数据*/   
        AM2302_Data->humi_high8bit= AM2302_ReadByte(AM2302_CH1);
        AM2302_Data->humi_low8bit = AM2302_ReadByte(AM2302_CH1);
        AM2302_Data->temp_high8bit= AM2302_ReadByte(AM2302_CH1);
        AM2302_Data->temp_low8bit = AM2302_ReadByte(AM2302_CH1);
        AM2302_Data->check_sum    = AM2302_ReadByte(AM2302_CH1);

        /*读取结束，引脚改为输出模式*/
        AM2302_Mode_Out_PP(AM2302_CH1);
        /*主机拉高*/
        AM2302_CH1_Dout_HIGH();
        
        /* 对数据进行处理 */
        humi_temp=AM2302_Data->humi_high8bit*256+AM2302_Data->humi_low8bit;
        AM2302_Data->humidity = humi_temp;   
        humi_temp=AM2302_Data->temp_high8bit*256+AM2302_Data->temp_low8bit;
        AM2302_Data->temperature= humi_temp;    
        
        /*检查读取的数据是否正确*/
        temp = AM2302_Data->humi_high8bit + AM2302_Data->humi_low8bit + 
               AM2302_Data->temp_high8bit+ AM2302_Data->temp_low8bit;
        
        xTaskResumeAll(); //重新开启调度
          
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
* 函数名  : AM2302_Read_TempAndHumidity
* 描  述  : 获取温湿度传感器．一次完整的数据传输为40bit，高位先出
*           8bit湿度整数 + 8bit湿度小数 + 8bit温度整数 + 8bit温度小数 + 8bit校验和 
* 输  入  : channel:通道号 
*           AM2302_Data:AM2302_Data_TypeDef数据类型
* 输  出  : 无
* 返 回 值: ERROR：  读取出错
*           SUCCESS：读取成功
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
  //初始化通道0
  AM2302_Init(AM2302_CH0);
  //初始化通道1
  AM2302_Init(AM2302_CH1);
  
  /* works after 10 seconds */
  osDelay(10000);
  
  for (;;)
  {
      /* 每2s采集一次 */
      osDelay(2000);
      
       /* 通道0 */
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
      
      /* 通道1 *//*
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
    //高优先级
    osThreadDef(RunAm2302, RunAm2302Thread, osPriorityRealtime, 0, 2*configMINIMAL_STACK_SIZE);
    _runAm2302_id = AaThreadCreateStartup(osThread(RunAm2302), NULL);
}
