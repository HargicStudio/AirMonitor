
#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include "Cfg.h"
#include "osa_file.h"
#include "common.h"
#include "moduleStatus.h"

#define COUNT_KEY  19

static char * default_key[] ={

    "addr",
    "softVer",
    "hardVer",
    "serverip",
    "serverport",
    "simpleInt",
    "reportInt",
    "k25",
    "b25",
    "k10",
    "b10",
    "coVw",
    "coVa",
    "so2Vw",
    "so2Va",
    "o3Vw",
    "o3Va",
    "no2Vw",
    "no2Va",
    0,

};
static char * default_value[] ={
    "80001",
    "301",
    "201",
    "120.27.26.208",
    "21006",
    "5",
    "5",
    "1",
    "1",
    "1",
    "1",
    "1",
    "1",
    "1",
    "1",
    "1",
    "1",
    "1",
    "1",
    0,
};

/************************************************
*Input:
*Output:
*Des: 打开配置文件
************************************************/
OSA_FileHandle OpenCfgFile(u8 opera)
{
    OSA_FileHandle p = NULL;
    
    if (OSA_OK != OSA_fileOpen(CFG_FILE, opera, &p))
    {
        SetModuleStu(MDU_CARD, STU_ERROR);
        printf("++++++SD CARD in problem! +++++++\r\n");
        return NULL;
    }
    
    return p;
}

/************************************************
*Input:
*Output:
*Des: 关闭配置文件
************************************************/
void CloseCfgFile(OSA_FileHandle pf)
{
    if (NULL != pf)
        OSA_fileClose(pf);

    pf = NULL;
}

/************************************************
*Input:
*Output:
*Des: 初始化配置，文件不存在时，创建文件并写入默认配置
************************************************/
int Initcfg(char *temp)
{
    OSA_FileHandle pf;
      
    pf = OpenCfgFile(OSA_FILEMODE_RDONLY);
    
    if (pf == NULL || temp !=NULL) //no file
    {
        if (pf == NULL)
            printf("not exsit cfg file\n");
        else 
            CloseCfgFile(pf);
                
        int i;
        pf = OpenCfgFile(OSA_FILEMODE_RDWR_NEW);

        printf("cfg\n");
        if (pf == NULL)
        {
            printf("create cfg file error\n");
            return -1;
        }

        if (temp == NULL)
        {
            printf("1\n");
            for (i = 0; i < COUNT_KEY; i++)
            {
                printf("cfg default %s: %s\r\n", default_key[i], default_value[i]);
                f_printf(pf, "%s=%s\r\n", default_key[i], default_value[i]);
            }
        }
        else
        {
            f_printf(pf, "%s", temp);
        }


        OSA_fileSync(pf);
    }

    CloseCfgFile(pf);

    return 0;
}

/************************************************
*Input:
*Output:
*Des: 根据关键key值读取配置信息
************************************************/
int ReadValue(char *key,int keylen, char *value) 
{ 
    char buf[40] = { 0 };
    OSA_FileHandle pf;

    pf = OpenCfgFile(OSA_FILEMODE_RDONLY);
    if (pf == NULL)
    {
        printf("error\n");
        return -1;

    }
    OSA_fileSeek(pf, 0);

    while (f_gets(buf,sizeof(buf),pf) != NULL)
    {
       
        if (strncmp(buf,key,keylen) == 0)
        {
            memcpy(value, buf + keylen + 1, strlen(buf) - keylen - 1);

            CloseCfgFile(pf);
            return 0;
        }
    }

    CloseCfgFile(pf);
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
    char temp[256] = { 0 };
    int  len = 0;
    OSA_FileHandle pf;

    pf = OpenCfgFile(OSA_FILEMODE_RDONLY);
    if (pf == NULL)
    {
        return -1;
    }
    OSA_fileSeek(pf, 0);

    while (f_gets(buf,sizeof(buf),pf) != NULL)
    {
       
        if (strncmp(buf, key, keylen) == 0)
        {
            printf("cfg write %s: %s\n", key, buf);
            sprintf(buf, "%s=%s\n", key, value);
        }
        memcpy(temp + len, buf, strlen(buf)); //save
        len += strlen(buf);
       
    }
   
    CloseCfgFile(pf);

    return Initcfg(temp);
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
