#ifndef _DATA_HANDLER_H
#define _DATA_HANDLER_H


#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"
#include "format.h"
	
#define SAMPLE_TEMP_WET_NUM     20
#define SAMPLE_GAS_NUM          20
#define SAMPLE_PM_NUM           20
#define SAPMLE_COORD_NUM        1                  // 坐标信息只采一次
#define SAMPLE_WIND_NUM         20


/* 温度湿度信息 */
typedef struct TEMP_WET_t
{
    u8      validNum;                       /* 当前保存了几个有效的数据 */
    u8      curPos;                         /* 当前已更新的数据位置 */
    s16     curTemp;                         /* 最近的温度平均值结果 */
    u16     curWet;													/* 最近的湿度平均值结果 */
    s16     totalTemp;                      /* 温度采样数据之和 */
    u16     totalWet;                       /* 湿度采样数据之和 */
    s16     dataTemp[SAMPLE_TEMP_WET_NUM];      /* 采样数据 */
    u16     dataWet[SAMPLE_TEMP_WET_NUM];      /* 采样数据 */

}TEMP_WET_t;

/* 气体浓度信息 */
typedef struct GAS_t
{
    u8      validNum;
    u8      curPos;
    u16     curData;
    u32     total;
    u32     data[SAMPLE_GAS_NUM];

}GAS_t;

/* 颗粒物浓度信息 */
typedef struct PM_t
{
    u8      validNum;
    u8      curPos;
    u16     curData;
    u16     total;
    u16     data[SAMPLE_PM_NUM];

}PM_t;

/* d定位信息。GPS坐标，需要转换成目标地图的坐标 */
/* 经纬度信息还原时要除以 100000 */
typedef struct COORD_t
{
    u8                  validNum;
    u8                  curPos;
    unsigned long int   curLong;
    unsigned long int   curLati;
    unsigned long int   totalLong;
    unsigned long int   totalLati;
    unsigned long int   dataLong[SAPMLE_COORD_NUM];       /* 经度 */
    unsigned long int   dataLati[SAPMLE_COORD_NUM];        /* 纬度 */

}COORD_t;

/* 风向风力信息。需要根
   据选择的是一体风向风
   速传感器还是分体传感
   器决定是否使用同一个结构体 */
typedef struct WIND_t
{
    u8      validNum;
    u8      curPos;
    u8      curSpeed;
    u8      curDirection;
    u8      speed[SAMPLE_WIND_NUM];
    u8      direction[SAMPLE_WIND_NUM];

}WIND_t;

/* 存储温度湿度 */
s8 StoreWetTempInfo(u16 wet, s16 temp, TEMP_WET_t *bag);
/* 存储气体浓度 */
s8 StoreGasInfo(u32 val, GAS_t *bag);
/* 存储颗粒物浓度 */
s8 StorePmInfo(u16 val, PM_t *bag);
/* 存储定位信息 */
s8 StoreCoordInfo(unsigned long int longVal, unsigned long int latVal, COORD_t *bag);
/* 存储风信息 */
s8 StoreWindInfo(u8 speedVal, u8 dirVal);

void testFillData(void);

void ContstructHead(u32 crc, u16 dataLen);
/*
void ContstructByte4();

void ConstructByte2();

void ConstuctByte();

void ContructString();
*/

/* 站号 */
/* 出厂唯一编号 */
/* 箱内温度 */
/* 箱内温度湿度 */
extern TEMP_WET_t g_tempWetIn;
/* 箱外温度湿度 */
extern TEMP_WET_t g_tempWetOut;
/* PM2.5 */
extern PM_t g_pm25;
/* PM10 */
extern PM_t g_pm10;
/* CO */
extern GAS_t g_co;
/* NO2 */
extern GAS_t g_no2;
/* SO2 */
extern GAS_t g_so2;
/* O3 */
extern GAS_t g_o3;
/* 坐标 */
extern COORD_t g_coord;
/* 风的信息 */
extern WIND_t g_wind;
/* 用于发送的头部 */
extern MSG_HEAD_t g_head;

#ifdef __cplusplus
}
#endif 

#endif
