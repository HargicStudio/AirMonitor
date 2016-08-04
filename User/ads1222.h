/*******************************************************************************
* ads1222.h
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


#ifndef __ADS1222_H__
#define	__ADS1222_H__

/* ========================================================================== */
/*                             头文件区                                       */
/* ========================================================================== */
#include "cmsis_os.h"
#include "AaInclude.h"
#include "main.h"
/* ========================================================================== */
/*                           宏和类型定义区                                   */
/* ========================================================================== */
//ADS1222 SCLK连接引脚定义 
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
#define ADS1222_C_SCLK_PORT                  GPIOC
#define ADS1222_C_SCLK_PIN                   GPIO_PIN_12
#define ADS1222_C_SCLK_LOW()          HAL_GPIO_WritePin(ADS1222_C_SCLK_PORT, ADS1222_C_SCLK_PIN, GPIO_PIN_RESET) 
#define ADS1222_C_SCLK_HIGH()         HAL_GPIO_WritePin(ADS1222_C_SCLK_PORT, ADS1222_C_SCLK_PIN, GPIO_PIN_SET)


#define ADS1222_D_SCLK_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOB_CLK_ENABLE()
#define ADS1222_D_SCLK_PORT                  GPIOC
#define ADS1222_D_SCLK_PIN                   GPIO_PIN_14
#define ADS1222_D_SCLK_LOW()          HAL_GPIO_WritePin(ADS1222_D_SCLK_PORT, ADS1222_D_SCLK_PIN, GPIO_PIN_RESET) 
#define ADS1222_D_SCLK_HIGH()         HAL_GPIO_WritePin(ADS1222_D_SCLK_PORT, ADS1222_D_SCLK_PIN, GPIO_PIN_SET)


//ADS1222 DOUT连接引脚定义 
#define ADS1222_A_DOUT_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOC_CLK_ENABLE()
#define ADS1222_A_DOUT_PORT                  GPIOC
#define ADS1222_A_DOUT_PIN                   GPIO_PIN_1
#define ADS1222_A_DOUT_LOW()     HAL_GPIO_WritePin(ADS1222_A_DOUT_PORT, ADS1222_A_DOUT_PIN, GPIO_PIN_RESET) 
#define ADS1222_A_DOUT_HIGH()    HAL_GPIO_WritePin(ADS1222_A_DOUT_PORT, ADS1222_A_DOUT_PIN, GPIO_PIN_SET)
#define ADS1222_A_DOUT_IN()	     HAL_GPIO_ReadPin(ADS1222_A_DOUT_PORT, ADS1222_A_DOUT_PIN)


#define ADS1222_B_DOUT_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOC_CLK_ENABLE()
#define ADS1222_B_DOUT_PORT                  GPIOC
#define ADS1222_B_DOUT_PIN                   GPIO_PIN_3
#define ADS1222_B_DOUT_LOW()     HAL_GPIO_WritePin(ADS1222_B_DOUT_PORT, ADS1222_B_DOUT_PIN, GPIO_PIN_RESET) 
#define ADS1222_B_DOUT_HIGH()    HAL_GPIO_WritePin(ADS1222_B_DOUT_PORT, ADS1222_B_DOUT_PIN, GPIO_PIN_SET)
#define ADS1222_B_DOUT_IN()	     HAL_GPIO_ReadPin(ADS1222_B_DOUT_PORT, ADS1222_B_DOUT_PIN)


#define ADS1222_C_DOUT_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOB_CLK_ENABLE()
#define ADS1222_C_DOUT_PORT                  GPIOC
#define ADS1222_C_DOUT_PIN                   GPIO_PIN_13
#define ADS1222_C_DOUT_LOW()     HAL_GPIO_WritePin(ADS1222_C_DOUT_PORT, ADS1222_C_DOUT_PIN, GPIO_PIN_RESET) 
#define ADS1222_C_DOUT_HIGH()    HAL_GPIO_WritePin(ADS1222_C_DOUT_PORT, ADS1222_C_DOUT_PIN, GPIO_PIN_SET)
#define ADS1222_C_DOUT_IN()	     HAL_GPIO_ReadPin(ADS1222_C_DOUT_PORT, ADS1222_C_DOUT_PIN)


#define ADS1222_D_DOUT_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOB_CLK_ENABLE()
#define ADS1222_D_DOUT_PORT                  GPIOC
#define ADS1222_D_DOUT_PIN                   GPIO_PIN_15
#define ADS1222_D_DOUT_LOW()     HAL_GPIO_WritePin(ADS1222_D_DOUT_PORT, ADS1222_D_DOUT_PIN, GPIO_PIN_RESET) 
#define ADS1222_D_DOUT_HIGH()    HAL_GPIO_WritePin(ADS1222_D_DOUT_PORT, ADS1222_D_DOUT_PIN, GPIO_PIN_SET)
#define ADS1222_D_DOUT_IN()	     HAL_GPIO_ReadPin(ADS1222_D_DOUT_PORT, ADS1222_D_DOUT_PIN)



//ADS1222 MUX引脚定义 
#define ADS1222_MUX_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOB_CLK_ENABLE()
#define ADS1222_MUX_PORT                  GPIOB
#define ADS1222_MUX_PIN                   GPIO_PIN_0

#define ADS1222_MUX_LOW()        HAL_GPIO_WritePin(ADS1222_MUX_PORT, ADS1222_MUX_PIN, GPIO_PIN_RESET) 
#define ADS1222_MUX_HIGH()       HAL_GPIO_WritePin(ADS1222_MUX_PORT, ADS1222_MUX_PIN, GPIO_PIN_SET)



//ADS1222 TEMPEN引脚定义 
#define ADS1222_TEMPEN_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOB_CLK_ENABLE()
#define ADS1222_TEMPEN_PORT                  GPIOB
#define ADS1222_TEMPEN_PIN                   GPIO_PIN_1

#define ADS1222_TEMPEN_LOW()     HAL_GPIO_WritePin(ADS1222_TEMPEN_PORT, ADS1222_TEMPEN_PIN, GPIO_PIN_RESET) 
#define ADS1222_TEMPEN_HIGH()    HAL_GPIO_WritePin(ADS1222_TEMPEN_PORT, ADS1222_TEMPEN_PIN, GPIO_PIN_SET)



//ADS1222 BUFEN引脚定义 
#define ADS1222_BUFEN_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOB_CLK_ENABLE()
#define ADS1222_BUFEN_PORT                  GPIOB
#define ADS1222_BUFEN_PIN                   GPIO_PIN_2

#define ADS1222_BUFEN_LOW()      HAL_GPIO_WritePin(ADS1222_BUFEN_PORT, ADS1222_BUFEN_PIN, GPIO_PIN_RESET) 
#define ADS1222_BUFEN_HIGH()     HAL_GPIO_WritePin(ADS1222_BUFEN_PORT, ADS1222_BUFEN_PIN, GPIO_PIN_SET)


//ADS1222芯片编号
#define  ADS1222_CHIP_A   0
#define  ADS1222_CHIP_B   1
#define  ADS1222_CHIP_C   2
#define  ADS1222_CHIP_D   3

//ADS1222芯片采集通道号
#define  ADS1222_CH0   0
#define  ADS1222_CH1   1

/* ========================================================================== */
/*                          数据结构定义区                                    */
/* ========================================================================== */

/* ========================================================================== */
/*                          函数声明区                                        */
/* ========================================================================== */

/*******************************************************************************
* 函数名  : ADS1222_Init
* 描  述  : 该函数初始化ADS1222
* 输  入  : 无
* 输  出  : 无
* 返回值  : 无
*******************************************************************************/
void ADS1222_Init(void);

/*******************************************************************************
* 函数名  : ADS1222_AdRead
* 描  述  : 获取ADS1222．一次完整的数据传输为24bit，高位先出
* 输  入  : chipId: ads1222芯片编号
*           channel: ads1222芯片通道号 
* 输  出  : 无
* 返 回 值: 读取24bit 原始AD值
*******************************************************************************/
unsigned long ADS1222_AdRead(uint8_t chipId, uint8_t channel);

#endif /* __ADS1222_H__ */
