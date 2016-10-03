#ifndef _COMMON_H
#define _COMMON_H

#ifdef __cplusplus
extern "C" {
#endif
	
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stm32f4xx_hal.h"
#include "AaInclude.h"
#include "config.h"
#include "ff.h"
#include "osa_file.h"
#include <stdbool.h>
#include "feature_name.h"
#include "rtc_dev.h"


#define LOCAL_BIG_ENDIAN 在编译器中定义
#ifdef LOCAL_BIG_ENDIAN
#define nhtons(s)  ((((s) & 0xff00)>>8) | (((s) & 0xff)<<8))
#define nhtonl(l)  (((l) & 0xff)<<24 | ((l) & 0xff000000)>>24 | ((l) & 0x00ff0000)>>8 | ((l) & 0x0000ff00)<<8)
#else
#define nhtons(s)  (s)
#define nhtonl(l)  (l)
#endif

#define GET_NEXT_POS(cur, total)    ((cur) == (total) - 1) ? 0 : (cur) + 1
#define MV_NEXT_POS(cur, total)     (cur) == (total) - 1 ? cur = 0 : (cur)++
	
#define OK		0
#define NOK_1	-1
#define NOK_2	-2
#define NOK_3 -3
  
#define LEN_REPORT_DATA         56
#define LEN_REPORT_DATA_WO_HEAD 46
#define LEN_REPORT_DATA_ONLY    38
#define LEN_HEAD                10
#define LEN_PARM_LEN            2
#define LEN_CRC                 2
#define LEN_ADDR                5
#define LEN_CMD                 3
#define LEN_ADDR_CMD            8

#define OFFSET_LEN              4
#define OFFSET_CRC              6
#define OFFSET_ADDR             10
#define OFFSET_CMD              15
#define OFFSET_DATA             18


/*
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned int u16;
typedef signed int s16;
typedef unsigned long u32;
typedef signed long s32;
*/

typedef struct MSG_HEAD_t
{
    u16 crc;
    u16 dataLen;              // 数据长度。 数据类型不同，长度不一，所以和operation个数没有直接关系

}MSG_HEAD_t;


#define VALUE_TEMP_LOW      -1000
#define VALUE_TEMP_HIGH     1000

#define VALUE_WET_LOW       0
#define VALUE_WET_HIGH      1000

#define VALUE_PM25_LOW      0
#define VALUE_PM25_HIGH     1000

#define VALUE_PM10_LOW      0
#define VALUE_PM10_HIGH     1000

#define VALUE_CO_LOW        0
#define VALUE_CO_HIGH       100

#define VALUE_SO2_LOW       0
#define VALUE_SO2_HIGH      100

#define VALUE_NO2_LOW       0
#define VALUE_NO2_HIGH      100

#define VALUE_O3_LOW        0
#define VALUE_O3_HIGH       100

#define IsValidTemp(a)      ((a) > VALUE_TEMP_LOW && (a) < VALUE_TEMP_HIGH)
#define IsValidWet(a)       ((a) > VALUE_WET_LOW && (a) < VALUE_WET_HIGH)
#define IsValidPM25(a)      ((a) > VALUE_PM25_LOW && (a) < VALUE_PM25_HIGH)
#define IsValidPM10(a)      ((a) > VALUE_PM10_LOW && (a) < VALUE_PM10_HIGH)
#define IsValidCO(a)        ((a) > VALUE_CO_LOW && (a) < VALUE_CO_HIGH)
#define IsValidSO2(a)       ((a) > VALUE_SO2_LOW && (a) < VALUE_SO2_HIGH)
#define IsValidNO2(a)       ((a) > VALUE_NO2_LOW && (a) < VALUE_NO2_HIGH)
#define IsValidO3(a)        ((a) > VALUE_O3_LOW && (a) < VALUE_O3_HIGH)


/* 是否达到采样个数 */
#define IsReachSamples(n, target)       ((n) >= (target) ? 1 : 0)

u32 stringToInt(u8 *buf, u16 len);

bool IsDirExit(u8 *path);

u32 myPow(u32 val, u32 times);

u16 getdayOfMon(u16 year, u16 month);
u16 getFebDays(u16 year);





#define DEBUG_DEBUG_GSM
//#define DEBUG_NO_DEBUG_GSM
//#define DEBUG_STD_GSM

#ifdef DEBUG_DEBUG_GSM
#define GSM_LOG_P0(fmt)                     AaSysLogPrintF(LOGLEVEL_INF, FeatureGsm, "%s %d:"fmt"\r\n",__FUNCTION__, __LINE__)
#define GSM_LOG_P1(fmt, p1)                 AaSysLogPrintF(LOGLEVEL_INF, FeatureGsm, "%s %d:"fmt"\r\n",__FUNCTION__, __LINE__, p1)
#define GSM_LOG_P2(fmt, p1, p2)             AaSysLogPrintF(LOGLEVEL_INF, FeatureGsm, "%s %d:"fmt"\r\n",__FUNCTION__, __LINE__, p1, p2)
#define GSM_LOG_P3(fmt, p1, p2, p3)         AaSysLogPrintF(LOGLEVEL_INF, FeatureGsm, "%s %d:"fmt"\r\n",__FUNCTION__, __LINE__, p1, p2, p3)
#define GSM_LOG_P4(fmt, p1, p2, p3, p4)     AaSysLogPrintF(LOGLEVEL_INF, FeatureGsm, "%s %d:"fmt"\r\n",__FUNCTION__, __LINE__, p1, p2, p3, p4)

#define GSM_LOG_E_P0(fmt)                     AaSysLogPrintF(LOGLEVEL_ERR, FeatureGsm, "%s %d:"fmt"\r\n",__FUNCTION__, __LINE__)
#define GSM_LOG_E_P1(fmt, p1)                 AaSysLogPrintF(LOGLEVEL_ERR, FeatureGsm, "%s %d:"fmt"\r\n",__FUNCTION__, __LINE__, p1)
#define GSM_LOG_E_P2(fmt, p1, p2)             AaSysLogPrintF(LOGLEVEL_ERR, FeatureGsm, "%s %d:"fmt"\r\n",__FUNCTION__, __LINE__, p1, p2)
#define GSM_LOG_E_P3(fmt, p1, p2, p3)         AaSysLogPrintF(LOGLEVEL_ERR, FeatureGsm, "%s %d:"fmt"\r\n",__FUNCTION__, __LINE__, p1, p2, p3)
#define GSM_LOG_E_P4(fmt, p1, p2, p3, p4)     AaSysLogPrintF(LOGLEVEL_ERR, FeatureGsm, "%s %d:"fmt"\r\n",__FUNCTION__, __LINE__, p1, p2, p3, p4)
#endif
  
#ifdef DEBUG_STD_GSM
#define GSM_LOG_P0(fmt)                     printf("%s %d:"fmt"\r\n",__FUNCTION__, __LINE__)
#define GSM_LOG_P1(fmt, p1)                 printf("%s %d:"fmt"\r\n",__FUNCTION__, __LINE__, p1)
#define GSM_LOG_P2(fmt, p1, p2)             printf("%s %d:"fmt"\r\n",__FUNCTION__, __LINE__, p1, p2)
#define GSM_LOG_P3(fmt, p1, p2, p3)         printf("%s %d:"fmt"\r\n",__FUNCTION__, __LINE__, p1, p2, p3)
#define GSM_LOG_P4(fmt, p1, p2, p3, p4)     printf("%s %d:"fmt"\r\n",__FUNCTION__, __LINE__, p1, p2, p3, p4)

#define GSM_LOG_E_P0(fmt)                     printf("%s %d:"fmt"\r\n",__FUNCTION__, __LINE__)
#define GSM_LOG_E_P1(fmt, p1)                 printf("%s %d:"fmt"\r\n",__FUNCTION__, __LINE__, p1)
#define GSM_LOG_E_P2(fmt, p1, p2)             printf("%s %d:"fmt"\r\n",__FUNCTION__, __LINE__, p1, p2)
#define GSM_LOG_E_P3(fmt, p1, p2, p3)         printf("%s %d:"fmt"\r\n",__FUNCTION__, __LINE__, p1, p2, p3)
#define GSM_LOG_E_P4(fmt, p1, p2, p3, p4)     printf("%s %d:"fmt"\r\n",__FUNCTION__, __LINE__, p1, p2, p3, p4)

#endif
 
#ifdef DEBUG_NO_DEBUG_GSM
#define GSM_LOG_P0(fmt)
#define GSM_LOG_P1(fmt, p1)
#define GSM_LOG_P2(fmt, p1, p2)
#define GSM_LOG_P3(fmt, p1, p2, p3)
#define GSM_LOG_P4(fmt, p1, p2, p3, p4)

#define GSM_LOG_E_P0(fmt)                     
#define GSM_LOG_E_P1(fmt, p1)        
#define GSM_LOG_E_P2(fmt, p1, p2)        
#define GSM_LOG_E_P3(fmt, p1, p2, p3)     
#define GSM_LOG_E_P4(fmt, p1, p2, p3, p4)
#endif


#define DEBUG_DEBUG_GPS
//#define DEBUG_NO_DEBUG_GPS
//#define DEBUG_STD_GPS

#ifdef DEBUG_DEBUG_GPS
#define GPS_LOG_P0(fmt)                     AaSysLogPrintF(LOGLEVEL_INF, FeatureGps, "%s %d:"fmt"\r\n",__FUNCTION__, __LINE__)
#define GPS_LOG_P1(fmt, p1)                 AaSysLogPrintF(LOGLEVEL_INF, FeatureGps, "%s %d:"fmt"\r\n",__FUNCTION__, __LINE__, p1)
#define GPS_LOG_P2(fmt, p1, p2)             AaSysLogPrintF(LOGLEVEL_INF, FeatureGps, "%s %d:"fmt"\r\n",__FUNCTION__, __LINE__, p1, p2)
#define GPS_LOG_P3(fmt, p1, p2, p3)         AaSysLogPrintF(LOGLEVEL_INF, FeatureGps, "%s %d:"fmt"\r\n",__FUNCTION__, __LINE__, p1, p2, p3)
#define GPS_LOG_P4(fmt, p1, p2, p3, p4)     AaSysLogPrintF(LOGLEVEL_INF, FeatureGps, "%s %d:"fmt"\r\n",__FUNCTION__, __LINE__, p1, p2, p3, p4)
#endif
  
#ifdef DEBUG_STD_GPS
#define GPS_LOG_P0(fmt)                     printf("%s %d:"fmt"\r\n",__FUNCTION__, __LINE__)
#define GPS_LOG_P1(fmt, p1)                 printf("%s %d:"fmt"\r\n",__FUNCTION__, __LINE__, p1)
#define GPS_LOG_P2(fmt, p1, p2)             printf("%s %d:"fmt"\r\n",__FUNCTION__, __LINE__, p1, p2)
#define GPS_LOG_P3(fmt, p1, p2, p3)         printf("%s %d:"fmt"\r\n",__FUNCTION__, __LINE__, p1, p2, p3)
#define GPS_LOG_P4(fmt, p1, p2, p3, p4)     printf("%s %d:"fmt"\r\n",__FUNCTION__, __LINE__, p1, p2, p3, p4)
#endif
 
#ifdef DEBUG_NO_DEBUG_GPS
#define GPS_LOG_P0(fmt)
#define GPS_LOG_P1(fmt, p1)
#define GPS_LOG_P2(fmt, p1, p2)
#define GPS_LOG_P3(fmt, p1, p2, p3)
#define GPS_LOG_P4(fmt, p1, p2, p3, p4)
#endif


#define DEBUG_DEBUG_CP15
//#define DEBUG_NO_DEBUG_CP15
//#define DEBUG_STD_CP15

#ifdef DEBUG_DEBUG_CP15
#define CP15_LOG_P0(fmt)                     AaSysLogPrintF(LOGLEVEL_INF, FeatureCP15, "%s %d:"fmt"\r\n",__FUNCTION__, __LINE__)
#define CP15_LOG_P1(fmt, p1)                 AaSysLogPrintF(LOGLEVEL_INF, FeatureCP15, "%s %d:"fmt"\r\n",__FUNCTION__, __LINE__, p1)
#define CP15_LOG_P2(fmt, p1, p2)             AaSysLogPrintF(LOGLEVEL_INF, FeatureCP15, "%s %d:"fmt"\r\n",__FUNCTION__, __LINE__, p1, p2)
#define CP15_LOG_P3(fmt, p1, p2, p3)         AaSysLogPrintF(LOGLEVEL_INF, FeatureCP15, "%s %d:"fmt"\r\n",__FUNCTION__, __LINE__, p1, p2, p3)
#define CP15_LOG_P4(fmt, p1, p2, p3, p4)     AaSysLogPrintF(LOGLEVEL_INF, FeatureCP15, "%s %d:"fmt"\r\n",__FUNCTION__, __LINE__, p1, p2, p3, p4)
#endif
  
#ifdef DEBUG_STD_CP15
#define CP15_LOG_P0(fmt)                     printf("%s %d:"fmt"\r\n",__FUNCTION__, __LINE__)
#define CP15_LOG_P1(fmt, p1)                 printf("%s %d:"fmt"\r\n",__FUNCTION__, __LINE__, p1)
#define CP15_LOG_P2(fmt, p1, p2)             printf("%s %d:"fmt"\r\n",__FUNCTION__, __LINE__, p1, p2)
#define CP15_LOG_P3(fmt, p1, p2, p3)         printf("%s %d:"fmt"\r\n",__FUNCTION__, __LINE__, p1, p2, p3)
#define CP15_LOG_P4(fmt, p1, p2, p3, p4)     printf("%s %d:"fmt"\r\n",__FUNCTION__, __LINE__, p1, p2, p3, p4)
#endif
 
#ifdef DEBUG_NO_DEBUG_CP15
#define CP15_LOG_P0(fmt)
#define CP15_LOG_P1(fmt, p1)
#define CP15_LOG_P2(fmt, p1, p2)
#define CP15_LOG_P3(fmt, p1, p2, p3)
#define CP15_LOG_P4(fmt, p1, p2, p3, p4)
#endif

#define DEBUG_DEBUG_AFS
//#define DEBUG_NO_DEBUG_AFS
//#define DEBUG_STD_AFS

#ifdef DEBUG_DEBUG_AFS
#define AFX_LOG_P0(fmt)                     AaSysLogPrintF(LOGLEVEL_INF, FeatureAlpha, "%s %d:"fmt"\r\n",__FUNCTION__, __LINE__)
#define AFX_LOG_P1(fmt, p1)                 AaSysLogPrintF(LOGLEVEL_INF, FeatureAlpha, "%s %d:"fmt"\r\n",__FUNCTION__, __LINE__, p1)
#define AFX_LOG_P2(fmt, p1, p2)             AaSysLogPrintF(LOGLEVEL_INF, FeatureAlpha, "%s %d:"fmt"\r\n",__FUNCTION__, __LINE__, p1, p2)
#define AFX_LOG_P3(fmt, p1, p2, p3)         AaSysLogPrintF(LOGLEVEL_INF, FeatureAlpha, "%s %d:"fmt"\r\n",__FUNCTION__, __LINE__, p1, p2, p3)
#define AFX_LOG_P4(fmt, p1, p2, p3, p4)     AaSysLogPrintF(LOGLEVEL_INF, FeatureAlpha, "%s %d:"fmt"\r\n",__FUNCTION__, __LINE__, p1, p2, p3, p4)
#endif
  
#ifdef DEBUG_STD_AFS
#define AFX_LOG_P0(fmt)                     printf("%s %d:"fmt"\r\n",__FUNCTION__, __LINE__)
#define AFX_LOG_P1(fmt, p1)                 printf("%s %d:"fmt"\r\n",__FUNCTION__, __LINE__, p1)
#define AFX_LOG_P2(fmt, p1, p2)             printf("%s %d:"fmt"\r\n",__FUNCTION__, __LINE__, p1, p2)
#define AFX_LOG_P3(fmt, p1, p2, p3)         printf("%s %d:"fmt"\r\n",__FUNCTION__, __LINE__, p1, p2, p3)
#define AFX_LOG_P4(fmt, p1, p2, p3, p4)     printf("%s %d:"fmt"\r\n",__FUNCTION__, __LINE__, p1, p2, p3, p4)
#endif
 
#ifdef DEBUG_NO_DEBUG_AFS
#define AFX_LOG_P0(fmt)
#define AFX_LOG_P1(fmt, p1)
#define AFX_LOG_P2(fmt, p1, p2)
#define AFX_LOG_P3(fmt, p1, p2, p3)
#define AFX_LOG_P4(fmt, p1, p2, p3, p4)
#endif

#ifdef __cplusplus
}
#endif

#endif



