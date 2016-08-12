/*******************************************************************************
* osa_file.c
*
* Copyright (C) 2015-2017 xxx Technology CO.,LTD.
*
* Author : menki
* Version: V1.0.0  2016-7-18 Create
*
* Desc: 实现SD卡模块对外提供的文件操作接口
*
* Modification: 
*    Date    :  
*    Revision:
*    Author  :
*    Contents:
*******************************************************************************/

/* ========================================================================== */
/*                             头文件区                                       */
/* ========================================================================== */
#include "osa_file.h"
#include <stdio.h>
#include <stdlib.h>
#include "cmsis_os.h"
#include "AaInclude.h"
#include "main.h"

/* ========================================================================== */
/*                           宏和类型定义区                                   */
/* ========================================================================== */


/* ========================================================================== */
/*                          数据结构定义区                                    */
/* ========================================================================== */
/*定义文件对象结构体*/
typedef struct
{
    FIL                MyFile;   /* File object */
    unsigned int      nMgicNum; /*魔数,用于校验句柄有效性。*/
    BYTE               mode;     /*用户态文件操作模式*/

}OSA_FileObject;

OSA_FileObject  gFileObj;

/* ========================================================================== */
/*                          函数声明区                                        */
/* ========================================================================== */


/* ========================================================================== */
/*                          全局变量定义区                                    */
/* ========================================================================== */
FATFS MySDFatFs;  /* File system object for SD card logical drive */
char mySDPath[4]; /* SD card logical drive path */
osThreadId _tfcard_id;


extern FIL MyFile;     /* File object */
/* ========================================================================== */
/*                          函数定义区                                        */
/* ========================================================================== */
static void StartThread(void const *argument);
static void Error_Handler(void);


/** 
 * This is a brief description. 
 * This is a detail description. 
 * @param[in]   inArgName input argument description. 
 * @param[out]  outArgName output argument description.  
 * @retval  
 * @retval  
 * @par 
 *      
 * @par 
 *      
 * @par History
 *      2016-8-4 Huang Shengda
 */  
int StartTFCardTask()
{
    /*##-1- Start task #########################################################*/
    osThreadDef(uSDThread, StartThread, osPriorityNormal, 0, 8 * configMINIMAL_STACK_SIZE);
    _tfcard_id = AaThreadCreateStartup(osThread(uSDThread), NULL);

    return 0;
}

/**
  * @brief  Start task
  * @param  pvParameters not used
  * @retval None
  */
static void StartThread(void const *argument)
{
//以前为FATFS文件系统封装各个接口的测试代码
    
  uint32_t byteswritten, bytesread;                     /* File write/read counts */
  uint8_t wtext[] = "This is AirMonitor working with FatFs"; /* File write buffer */
  uint8_t rtext[36];                                   /* 注意读缓冲空间大小，大空间用malloc申请 */
  SD_sizeInfo sdSizeInfo;
  OSA_FileHandle hFile;

  /* 文件系统初始化　*/
  OSA_fileInit();
  
  /* 获取SD卡容量信息 */
  if (OSA_getSdSize(&sdSizeInfo) == OSA_OK)
  {
      AaSysLogPrintF(LOGLEVEL_INF, FeatureLog, "SD totalSize=%dM availableSize=%dM\n\r",
                     sdSizeInfo.totalSize, sdSizeInfo.availableSize);
  }
  
  if (OSA_fileOpen("AirMonitor.txt", OSA_FILEMODE_RDWR, &hFile) != OSA_OK)
  {
      Error_Handler();
  }
  
  byteswritten = OSA_fileWrite(hFile, wtext, sizeof(wtext));
  if (byteswritten <= 0)
  {
      Error_Handler();
  }
  else
  {
      OSA_fileSync(hFile);
      OSA_fileClose(hFile);
      
      if (OSA_fileOpen("AirMonitor.txt", OSA_FILEMODE_RDONLY, &hFile) != OSA_OK)
      {
          Error_Handler();
      }
      else
      {
          /* 跳转４个字节读写 */
          OSA_fileSeek (hFile, 4);

          bytesread = OSA_fileRead (hFile, rtext, sizeof(rtext));
          if (bytesread <= 0)
          {
              Error_Handler();
          }
          else
          {
              //printf("fatfs read: %s, len = %d\n\r",rtext, bytesread);
              AaSysLogPrintF(LOGLEVEL_DBG, FeatureLog, "fatfs read: %s, len = %d\n\r",rtext, bytesread);
          }

          OSA_fileClose(hFile);
      }
  }
  /* 删除文件 */
  OSA_removeFile("STM32.TXT");
  
  /* 文件系统反初始化　*/
  OSA_fileDeInit();

  /* Infinite Loop */
  for( ;; )
  {
  }
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
static void Error_Handler(void)
{
  /* Turn LED3 on */
  while(1)
  {
      printf("Error_Handler!!!\n\r");
      osDelay(1000);
  }
}

/*******************************************************************************
* 函数名  : OSA_fileInit
* 描  述  : 该函数初始化系统文件系统
* 输  入  : 无
* 输  出  : 无
* 返回值  : OSA_EFAIL: 失败
*           OSA_SOK:   成功
*******************************************************************************/
int OSA_fileInit (void)
{
    /*##-1- Link the micro SD disk I/O driver ##################################*/
    if(FATFS_LinkDriver(&SD_Driver, mySDPath) == 0)
    { 
      /*##-2- Register the file system object to the FatFs module ##############*/
      if(f_mount(&MySDFatFs, (TCHAR const*)mySDPath, 0) != FR_OK)
      {
          /* FatFs Initialization Error */
          printf("FatFs f_mount Error");
          return OSA_EFAIL;
      }
      else
      {
          return OSA_OK;
      }
    }
    else
    {
        return OSA_EFAIL;
    }
    
}

/*******************************************************************************
* 函数名  : OSA_fileDeInit
* 描  述  : 该函数反初始化系统文件系统
* 输  入  : 无
* 输  出  : 无
* 返回值  : OSA_EFAIL: 失败
*           OSA_SOK:   成功
*******************************************************************************/
int OSA_fileDeInit (void)
{
    int ret;
    

    /*##-11- Unlink the RAM disk I/O driver ####################################*/
    ret = FATFS_UnLinkDriver(mySDPath);
    if (ret)
    {
        printf("FATFS_UnLinkDriver failed\n");
        return OSA_EFAIL;
    }
    else
    {
        return OSA_OK;
    }
}


/*******************************************************************************
* 函数名  : OSA_getSdSize
* 描  述  : 文件空间占用情况
* 输  入  : 无
* 输  出  : 无
* 返回值  : OSA_EFAIL: 失败
*           OSA_SOK:   成功
*******************************************************************************/
int OSA_getSdSize(SD_sizeInfo *pSdSizeInfo)
{
    FATFS *fs;
    DWORD fre_clust;        
    FRESULT res;
    
    res = f_getfree(mySDPath, &fre_clust, &fs);  /* 必须是根目录，选择磁盘0 */
    if ( res==FR_OK ) 
    {
	  pSdSizeInfo->totalSize = ((fs->n_fatent - 2) * fs->csize ) / 2 /1024;
      pSdSizeInfo->availableSize = (fre_clust * fs->csize) / 2 /1024;
      
	  return OSA_OK;
	}
	else
	{ 
	  printf("\n\rGet total drive space faild!\n\r");
	  return OSA_EFAIL;   
	}
}

/*******************************************************************************
* 函数名  : OSA_fileOpen
* 描  述  : 该函数负责打开一个文件,支持四种操作模式: 只读、只写、读写创建、追加写
*           若文件打开成功,将返回一个文件句柄,供调用其他文件操作接口时传入
*
* 输  入  : - fileName: 包含文件路径信息的完整文件名
*           - mode:     文件操作模式,参见OSA_FileMode定义
*
* 输  出  : - phFile:   文件句柄指针,当打开成功时输出文件句柄
* 返回值  :  OSA_SOK:   打开成功
*            OSA_EFAIL: 打开失败
*******************************************************************************/
int OSA_fileOpen(const char *fileName, OSA_FileMode mode, 
                    OSA_FileHandle *phFile)
{
    OSA_FileObject *pFileObj = NULL;
    BYTE modeFlag;
    FRESULT result;
    
    if(NULL == fileName)
    {
        printf("NULL fileName\n");
        return OSA_EFAIL;
    }

    if(NULL == phFile)
    {
        printf("NULL phFile\n");
        return OSA_EFAIL;
    }
    
    switch(mode)
    {
        case OSA_FILEMODE_RDONLY:
            modeFlag = FA_READ;
            break;
        case OSA_FILEMODE_WRONLY:
            modeFlag = FA_WRITE;
            break;
        case OSA_FILEMODE_RDWR_NEW:
            modeFlag = FA_CREATE_ALWAYS | FA_WRITE | FA_READ;
            break;
        case OSA_FILEMODE_RDWR:
            modeFlag = FA_WRITE | FA_READ;
            break;
        case OSA_FILEMODE_WRITEN:
            modeFlag = FA__WRITTEN;
      
        default:
            printf("Invalid mode:%d\n", mode);
            return OSA_EFAIL;
    }

    /*分配内存*/
   /* pFileObj = (OSA_FileObject *)malloc(sizeof(OSA_FileObject));
    if(NULL == pFileObj)
    {
        printf("alloc pFileObj failed\n");
        return OSA_EFAIL;
    }*/
    pFileObj = &gFileObj;
    
    result = f_open(&pFileObj->MyFile, fileName, modeFlag);
    if(result != FR_OK)
    {
        printf("Open failed\r\n");
        goto failed;
    }


    
    pFileObj->mode = modeFlag;     
    pFileObj->nMgicNum = OSA_MAGIC_NUM;
    *phFile = (OSA_FileHandle)pFileObj;
    return OSA_OK;

failed:
   /* if(NULL != pFileObj)
    {
        free(pFileObj);
    }*/
    
    return OSA_EFAIL;
    
}


/*******************************************************************************
* 函数名  : OSA_fileClose
* 描  述  : 该函数负责关闭一个文件,将销毁文件句柄指向的对象,
*           因此调用该接口后原有的文件句柄不能再使用。
*
* 输  入  : - hFile: 文件句柄
* 输  出  : 无
* 返回值  : OSA_SOK:   关闭成功
*           OSA_EFAIL: 关闭失败
*******************************************************************************/
int OSA_fileClose(OSA_FileHandle hFile)
{
    OSA_FileObject *pFileObj = (OSA_FileObject *)hFile;
    
    if(NULL == pFileObj)
    {
        printf("NULL pFileObj\n");
        return OSA_EFAIL;
    }

    f_close(&pFileObj->MyFile);

    /*魔数设为-1，避免其他人在释放后继续用*/
    pFileObj->nMgicNum = (int)-1;
    ///free(pFileObj);
    return OSA_OK;
}

/*******************************************************************************
* 函数名  : OSA_removeFile
* 描  述  : 删除指定文件
* 输  入  : 无
* 输  出  : 无
* 返回值  : OSA_SOK:   删除成功
*           OSA_EFAIL: 删除失败
*******************************************************************************/
int OSA_removeFile(const char *pName)
{
    FRESULT ret;
    
    if (pName == NULL)
    {
         printf("param error!\n\r");
        return OSA_EFAIL;
    }
    
    ret =  f_unlink (pName);	
    if(ret != FR_OK)
    {
      printf("remove file failed !, %d\n\r",ret);
      return OSA_EFAIL;
    }
    return OSA_OK;
}

/*******************************************************************************
* 函数名  : OSA_fileRead
* 描  述  : 该函数负责实现文件的读操作,读缓冲必须由调用者申请,且调用者必须保证
*           buffer的长度要大于等于size
*
* 输  入  : - hFile:   文件句柄
*           - buffer:  读缓冲区
*           - size:    要读取的长度
* 输  出  : 无
* 返回值  : OSA_EFAIL: 读取失败
*           >= 0:      真正读取的字节数
*******************************************************************************/
int OSA_fileRead (OSA_FileHandle hFile, unsigned char *buffer, unsigned int size)
{
    OSA_FileObject *pFileObj = (OSA_FileObject *)hFile;
    int ret = 0;
    unsigned int bytesread;  
    
    if(NULL == pFileObj)
    {
        printf("NULL pFileObj\n");
        return OSA_EFAIL;
    }

    if(NULL == buffer)
    {
        printf("NULL buffer\n");
        return OSA_EFAIL;
    }

    ret = f_read(&pFileObj->MyFile, buffer, size, (UINT*)&bytesread);
    
    if(ret != FR_OK)
    {
        return OSA_EFAIL;
    }

    return bytesread;
}


/*******************************************************************************
* 函数名  : OSA_fileWrite
* 描  述  : 该函数负责实现文件的写操作,缓冲区buffer必须由调用者申请,且调用者必须保证
*           buffer的长度要大于等于size
*
* 输  入  : - hFile:   文件句柄
*           - buffer:  写缓冲区
*           - size:    要写入的长度
* 输  出  : 无
* 返回值  : OSA_EFAIL: 写入失败
*           >= 0:      真正写入的字节数
*******************************************************************************/
int OSA_fileWrite(OSA_FileHandle hFile, unsigned char *buffer, unsigned int size)
{
    OSA_FileObject *pFileObj = (OSA_FileObject *)hFile;
    int res = 0;
    unsigned int byteswritten;  
    
    if(NULL == pFileObj)
    {
        printf("NULL pFileObj\n");
        return OSA_EFAIL;
    }
    
    if(NULL == buffer)
    {
        printf("NULL buffer\n");
        return OSA_EFAIL;
    }

    res = f_write(&pFileObj->MyFile, buffer, size, (void *)&byteswritten);
    
    if((byteswritten == 0) || (res != FR_OK))
    {
       return OSA_EFAIL;
    }
    
    return byteswritten;
}

/*******************************************************************************
* 函数名  : OSA_fileSeek
* 描  述  : 该函数负责实现文件的Seek操作
*
* 输  入  : - hFile:   文件句柄
*           - offset:  seek的从文件头偏移字节数
* 输  出  : 无
* 返回值  : OSA_EFAIL: seek失败
*           OSA_SOK:   seek成功
*******************************************************************************/
int OSA_fileSeek (OSA_FileHandle hFile,unsigned int offset)
{
    OSA_FileObject *pFileObj = (OSA_FileObject *)hFile;
    int res = 0;

    if(NULL == pFileObj)
    {
        printf("NULL pFileObj\n");
        return OSA_EFAIL;
    }
    
    res = f_lseek(&pFileObj->MyFile, offset);
    if(res != FR_OK)
    {
       return OSA_EFAIL;
    }

    return OSA_OK;
}


/*******************************************************************************
* 函数名  : OSA_fileSync
* 描  述  : 该函数负责将文件缓冲里的数据刷到硬件
*
* 输  入  : - hFile: 文件句柄
* 输  出  : 无
* 返回值  : OSA_SOK:   成功
*           OSA_EFAIL: 失败
*******************************************************************************/
int OSA_fileSync(OSA_FileHandle hFile)
{
    OSA_FileObject *pFileObj = (OSA_FileObject *)hFile;
    int res;
    
    if(NULL == pFileObj)
    {
        printf("NULL pFileObj\n");
        return OSA_EFAIL;
    }
   
    res = f_sync(&pFileObj->MyFile);
    if(res != FR_OK)
    {
       return OSA_EFAIL;
    }
    
    return OSA_OK;
}
