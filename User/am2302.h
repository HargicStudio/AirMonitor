/*******************************************************************************
* am2302.h
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
*******************************************************************************/


#ifndef __AM2302_H__
#define	__AM2302_H__

/* ========================================================================== */
/*                             头文件区                                       */
/* ========================================================================== */
#include "cmsis_os.h"
#include "AaInclude.h"
#include "main.h"
/* ========================================================================== */
/*                           宏和类型定义区                                   */
/* ========================================================================== */
//AM2302 CH0连接引脚定义 
#define AM2302_CH0_Dout_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOD_CLK_ENABLE()
#define AM2302_CH0_Dout_PORT                  GPIOD
#define AM2302_CH0_Dout_PIN                   GPIO_PIN_14

//AM2302 CH1连接引脚定义 
#define AM2302_CH1_Dout_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOD_CLK_ENABLE()
#define AM2302_CH1_Dout_PORT                  GPIOD
#define AM2302_CH1_Dout_PIN                   GPIO_PIN_15

//AM2302 CH0函数宏定义
#define AM2302_CH0_Dout_LOW()     HAL_GPIO_WritePin(AM2302_CH0_Dout_PORT, AM2302_CH0_Dout_PIN, GPIO_PIN_RESET) 
#define AM2302_CH0_Dout_HIGH()    HAL_GPIO_WritePin(AM2302_CH0_Dout_PORT, AM2302_CH0_Dout_PIN, GPIO_PIN_SET)
#define AM2302_CH0_Data_IN()	  HAL_GPIO_ReadPin(AM2302_CH0_Dout_PORT, AM2302_CH0_Dout_PIN)

//AM2302 CH1函数宏定义
#define AM2302_CH1_Dout_LOW()     HAL_GPIO_WritePin(AM2302_CH1_Dout_PORT, AM2302_CH1_Dout_PIN, GPIO_PIN_RESET) 
#define AM2302_CH1_Dout_HIGH()    HAL_GPIO_WritePin(AM2302_CH1_Dout_PORT, AM2302_CH1_Dout_PIN, GPIO_PIN_SET)
#define AM2302_CH1_Data_IN()	  HAL_GPIO_ReadPin(AM2302_CH1_Dout_PORT, AM2302_CH1_Dout_PIN)

#define  AM2302_CH0   0
#define  AM2302_CH1   1

/* ========================================================================== */
/*                          数据结构定义区                                    */
/* ========================================================================== */
typedef struct
{
	uint8_t  humi_high8bit;		//原始数据：湿度高8位
	uint8_t  humi_low8bit;	 	//原始数据：湿度低8位
	uint8_t  temp_high8bit;	 	//原始数据：温度高8位
	uint8_t  temp_low8bit;	 	//原始数据：温度高8位
	uint8_t  check_sum;	 	    //校验和
    uint16_t humidity;          //实际湿度乘以10，只保留小数点后一位
    uint16_t temperature;       //实际湿度乘以10，只保留小数点后一位  
} AM2302_Data_TypeDef;

/* ========================================================================== */
/*                          函数声明区                                        */
/* ========================================================================== */

/*******************************************************************************
* 函数名  : AM2302_Init
* 描  述  : 该函数初始化AM2302
* 输  入  : channel:通道号 
* 输  出  : 无
* 返回值  : 无
*******************************************************************************/
void AM2302_Init(uint8_t channel);

/*******************************************************************************
* 函数名  : AM2302_Read_TempAndHumidity
* 描  述  : 获取温湿度传感器．一次完整的数据传输为40bit，高位先出
*           8bit湿度整数 + 8bit湿度小数 + 8bit温度整数 + 8bit温度小数 + 8bit校验和 
* 输  入  : channel:通道号 
*           AM2302_Data:AM2302_Data_TypeDef数据类型
* 输  出  : 无
* 返 回 值: ERROR：  读取出错
*           SUCCESS：读取成功
*           BUG注意：偶尔会获取失败，所以本地申明变量存储，如果失败则上传上一次获取值
*******************************************************************************/
uint8_t AM2302_Read_TempAndHumidity(uint8_t channel, AM2302_Data_TypeDef * AM2302_Data);

//启动温湿度传感器AM2302采集线程
void StartRunAm2302Task();

#endif /* __AM2302_H__ */
