/*******************************************************************************
* am2302.h
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
*******************************************************************************/


#ifndef __AM2302_H__
#define	__AM2302_H__

/* ========================================================================== */
/*                             ͷ�ļ���                                       */
/* ========================================================================== */
#include "cmsis_os.h"
#include "AaInclude.h"
#include "main.h"
/* ========================================================================== */
/*                           ������Ͷ�����                                   */
/* ========================================================================== */
//AM2302 CH0�������Ŷ��� 
#define AM2302_CH0_Dout_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOD_CLK_ENABLE()
#define AM2302_CH0_Dout_PORT                  GPIOD
#define AM2302_CH0_Dout_PIN                   GPIO_PIN_14

//AM2302 CH1�������Ŷ��� 
#define AM2302_CH1_Dout_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOD_CLK_ENABLE()
#define AM2302_CH1_Dout_PORT                  GPIOD
#define AM2302_CH1_Dout_PIN                   GPIO_PIN_15

//AM2302 CH0�����궨��
#define AM2302_CH0_Dout_LOW()     HAL_GPIO_WritePin(AM2302_CH0_Dout_PORT, AM2302_CH0_Dout_PIN, GPIO_PIN_RESET) 
#define AM2302_CH0_Dout_HIGH()    HAL_GPIO_WritePin(AM2302_CH0_Dout_PORT, AM2302_CH0_Dout_PIN, GPIO_PIN_SET)
#define AM2302_CH0_Data_IN()	  HAL_GPIO_ReadPin(AM2302_CH0_Dout_PORT, AM2302_CH0_Dout_PIN)

//AM2302 CH1�����궨��
#define AM2302_CH1_Dout_LOW()     HAL_GPIO_WritePin(AM2302_CH1_Dout_PORT, AM2302_CH1_Dout_PIN, GPIO_PIN_RESET) 
#define AM2302_CH1_Dout_HIGH()    HAL_GPIO_WritePin(AM2302_CH1_Dout_PORT, AM2302_CH1_Dout_PIN, GPIO_PIN_SET)
#define AM2302_CH1_Data_IN()	  HAL_GPIO_ReadPin(AM2302_CH1_Dout_PORT, AM2302_CH1_Dout_PIN)

#define  AM2302_CH0   0
#define  AM2302_CH1   1

/* ========================================================================== */
/*                          ���ݽṹ������                                    */
/* ========================================================================== */
typedef struct
{
	uint8_t  humi_high8bit;		//ԭʼ���ݣ�ʪ�ȸ�8λ
	uint8_t  humi_low8bit;	 	//ԭʼ���ݣ�ʪ�ȵ�8λ
	uint8_t  temp_high8bit;	 	//ԭʼ���ݣ��¶ȸ�8λ
	uint8_t  temp_low8bit;	 	//ԭʼ���ݣ��¶ȸ�8λ
	uint8_t  check_sum;	 	    //У���
    uint16_t humidity;          //ʵ��ʪ�ȳ���10��ֻ����С�����һλ
    uint16_t temperature;       //ʵ��ʪ�ȳ���10��ֻ����С�����һλ  
} AM2302_Data_TypeDef;

/* ========================================================================== */
/*                          ����������                                        */
/* ========================================================================== */

/*******************************************************************************
* ������  : AM2302_Init
* ��  ��  : �ú�����ʼ��AM2302
* ��  ��  : channel:ͨ���� 
* ��  ��  : ��
* ����ֵ  : ��
*******************************************************************************/
void AM2302_Init(uint8_t channel);

/*******************************************************************************
* ������  : AM2302_Read_TempAndHumidity
* ��  ��  : ��ȡ��ʪ�ȴ�������һ�����������ݴ���Ϊ40bit����λ�ȳ�
*           8bitʪ������ + 8bitʪ��С�� + 8bit�¶����� + 8bit�¶�С�� + 8bitУ��� 
* ��  ��  : channel:ͨ���� 
*           AM2302_Data:AM2302_Data_TypeDef��������
* ��  ��  : ��
* �� �� ֵ: ERROR��  ��ȡ����
*           SUCCESS����ȡ�ɹ�
*           BUGע�⣺ż�����ȡʧ�ܣ����Ա������������洢�����ʧ�����ϴ���һ�λ�ȡֵ
*******************************************************************************/
uint8_t AM2302_Read_TempAndHumidity(uint8_t channel, AM2302_Data_TypeDef * AM2302_Data);

//������ʪ�ȴ�����AM2302�ɼ��߳�
void StartRunAm2302Task();

#endif /* __AM2302_H__ */
