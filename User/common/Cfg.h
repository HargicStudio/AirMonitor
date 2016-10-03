#ifndef CFG_H_
#define CFG_H_

#include "ff.h"
#include "common.h"

#define CFG_FILE  "AirMonitor.txt"

#define COUNT_KEY  26

enum 
{
    C_ADDR,
    C_SOFTVER,
    C_HARDVER,
    C_SERVERIP,
    C_SERVERPORT,
    C_SIMPLEINT,
    C_REPORTINT,
    C_K25,
    C_B25,
    C_K10,
    C_B10,
    C_PM10BaseV,
    C_PM10BaseC,
    C_PM10N,
    C_COVw,
    C_COVa,
    C_coS,
    C_SO2Vw,
    C_SO2Va,
    C_so2S,
    C_O3Vw,
    C_O3Va,
    C_o3S,
    C_NO2Vw,
    C_NO2Va,
    C_no2S,
};

int Initcfg(char *temp);
int ReadCfg(int index,char *value);
int ReadCfgInt(int index, int *iVal);

int WriteCfg(int index,char *value);
int WriteCfgInt(int index, int iVal);

int ReadValue(char *key,int keylen, char *value);

int WriteValue(char *key, int keylen, char *value);


/************************************************
*Input:
*Output:
*Des: 关闭配置文件
************************************************/
void CloseCfgFile(FIL *pf);
/************************************************
*Input:
*Output:
*Des: 打开配置文件
************************************************/
s32 OpenCfgFile(u8 opera, FIL *fp);


extern char * default_key[COUNT_KEY];
extern char * default_value[COUNT_KEY];

#endif