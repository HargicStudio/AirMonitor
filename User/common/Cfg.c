
#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include "Cfg.h"
#include "osa_file.h"
#include "common.h"
#include "moduleStatus.h"


char * default_key[COUNT_KEY] ={

    "addr",
    "softVer",
    "hardVer",
    "serverip",
    "serverport",
    "simpleInt",
    "reportInt",
    "k25",                               // PM2.5 K值（实际值/100）
    "b25",                               // PM2.5 B值（实际值/100）
    "k10",                               // PM10 K值（实际值/100）
    "b10",                               // PM10 B值（实际值/100）
    "pm10baseV",                         // PM10 基准电压  单位 mv      default : 967
    "pm10baseC",                         // PM10 基准温度  基准温度精确到十分位，所以是实际值 * 10 default : 250
    "pm10N",                             // PM10 系数。 实际值 * 100， 默认 0.6， 即 60       default: 60
    "coVw",
    "coVa",
    "coS",
    "so2Vw",
    "so2Va",
    "so2S",
    "o3Vw",
    "o3Va",
    "o3S",
    "no2Vw",
    "no2Va",
    "no2S",

};

char * default_value[] ={
    "80001",
    "301",
    "201",
    "120.92.44.87",
    "21007",
    "5",
    "5",
    "1",
    "1",
    "1",
    "1",
    "967",              // PM10 基准电压
    "250",              // PM10 基准温度
    "60",
    "520",
    "354",
    "320",
    "163",
    "360",
    "450",
    "28",
    "28",
    "1150",
    "23",
    "23",
    "430",
};

u8 temp_buf[256];

/************************************************
*Input:
*Output:
*Des: 打开配置文件
************************************************/
s32 OpenCfgFile(u8 opera, FIL *fp)
{
    s32 ret;
    
    ret = f_open(fp, CFG_FILE, opera);
    if (FR_OK != ret)
    {
        SetModuleStu(MDU_CARD, STU_ERROR);
        printf("++++++SD CARD in problem! +++++++\r\n");
    }
    
    return ret;
}

/************************************************
*Input:
*Output:
*Des: 关闭配置文件
************************************************/
void CloseCfgFile(FIL *pf)
{
    if (NULL != pf)
        f_close(pf);

    pf = NULL;
}

/************************************************
*Input:
*Output:
*Des: 初始化配置，文件不存在时，创建文件并写入默认配置
************************************************/
int Initcfg(char *temp)
{
    FIL pf;
    s32 ret;
      
    ret = OpenCfgFile(FA_READ, &pf);
    
    if (ret != FR_OK || temp !=NULL) //no file
    {
        if (ret != FR_OK)
            printf("not exsit cfg file\n");
        else 
            CloseCfgFile(&pf);
                
        int i;
        ret = OpenCfgFile(FA_READ | FA_WRITE | FA_CREATE_ALWAYS, &pf);

        if (ret != FR_OK)
        {
            printf("create cfg file error\n");
            return -1;
        }

        if (temp == NULL)
        {
            for (i = 0; i < COUNT_KEY; i++)
            {
                printf("cfg default %s: %s\r\n", default_key[i], default_value[i]);
                f_printf(&pf, "%s=%s\r\n", default_key[i], default_value[i]);
            }
        }
        else
        {
            f_printf(&pf, "%s", temp);
        }


        f_sync(&pf);
    }

    CloseCfgFile(&pf);

    return 0;
}

/************************************************
*Input:
*Output:
*Des: 根据关键key值读取配置信息
************************************************/
int ReadValue(char *key,int keylen, char *value) 
{ 
    FIL pf;
    s32 ret;
    
    memset(temp_buf, 0, 40);

    ret = OpenCfgFile(FA_READ, &pf);
    if (ret != FR_OK)
    {
        printf("error\n");
        return -1;

    }
    f_lseek(&pf, 0);

    while (f_gets(temp_buf,40,&pf) != NULL)
    {
       
        if (strncmp(temp_buf,key,keylen) == 0)
        {
            memcpy(value, temp_buf + keylen + 1, strlen(temp_buf) - keylen - 2);

            CloseCfgFile(&pf);
            return 0;
        }
    }

    CloseCfgFile(&pf);
    return -1; 
}

/************************************************
*Input:
*Output:
*Des: 根据关键key值写配置信息
************************************************/
int WriteValue(char *key, int keylen, char *value)
{
    char buf[40] = { 0 };
    int  len = 0;
    FIL pf;
 
    s32 ret;
    
    ret = OpenCfgFile(FA_READ, &pf);
    if (ret != FR_OK)
    {
        return -1;
    }
    
    f_lseek(&pf, 0);

    while (f_gets(buf,sizeof(buf),&pf) != NULL)
    {
       
        if (strncmp(buf, key, keylen) == 0)
        {
            printf("cfg write %s: %s\n", key, buf);
            sprintf(buf, "%s=%s\n", key, value);
        }
        memcpy(temp_buf + len, buf, strlen(buf)); //save
        len += strlen(buf);
       
    }
   
    CloseCfgFile(&pf);

    return Initcfg(temp_buf);
}

/************************************************
*Input:
*Output:
*Des: 根据配置项的index值读取配置项
************************************************/
int ReadCfg(int index,char *value)
{
    return ReadValue(default_key[index], strlen(default_key[index]), value);
}

/************************************************
*Input:
*Output:
*Des: 根据配置项的index，直接读取配置项的 int型值
************************************************/
int ReadCfgInt(int index, int *iVal)
{
    char buf[30] = {0};
    
    if (-1 == ReadCfg(index, buf))
    {
        return -1;
    }
    
    *iVal = atoi(buf);

    printf("************Read cfg: %s: %d  ************\r\n", default_key[index], *iVal);
    
    return 0;
}

/************************************************
*Input:
*Output:
*Des: 根据配置项的index值写配置项，传入字符串
************************************************/
int WriteCfg(int index,char *value)
{
    return WriteValue(default_key[index], strlen(default_key[index]), value);
}

/************************************************
*Input:
*Output:
*Des: 根据配置项的index，写配置项，传入int型值
************************************************/
int WriteCfgInt(int index, int iVal)
{
    char buf[10] = {0};

    sprintf(buf, "%d", iVal);


    return WriteCfg(index, buf);
}
