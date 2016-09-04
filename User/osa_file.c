/*******************************************************************************
* osa_file.c
*
* Copyright (C) 2015-2017 xxx Technology CO.,LTD.
*
* Author : menki
* Version: V1.0.0  2016-7-18 Create
*
* Desc: ʵ��SD��ģ������ṩ���ļ������ӿ�
*
* Modification: 
*    Date    :  
*    Revision:
*    Author  :
*    Contents:
*******************************************************************************/

/* ========================================================================== */
/*                             ͷ�ļ���                                       */
/* ========================================================================== */
#include "osa_file.h"
#include <stdio.h>
#include <stdlib.h>
#include "cmsis_os.h"
#include "AaInclude.h"
#include "main.h"

/* ========================================================================== */
/*                           ������Ͷ�����                                   */
/* ========================================================================== */


/* ========================================================================== */
/*                          ���ݽṹ������                                    */
/* ========================================================================== */
/*�����ļ�����ṹ��*/
typedef struct
{
    FIL                MyFile;   /* File object */
    unsigned int      nMgicNum; /*ħ��,����У������Ч�ԡ�*/
    BYTE               mode;     /*�û�̬�ļ�����ģʽ*/

}OSA_FileObject;

OSA_FileObject  gFileObj;

/* ========================================================================== */
/*                          ����������                                        */
/* ========================================================================== */


/* ========================================================================== */
/*                          ȫ�ֱ���������                                    */
/* ========================================================================== */
FATFS MySDFatFs;  /* File system object for SD card logical drive */
char mySDPath[4]; /* SD card logical drive path */
extern FIL MyFile;     /* File object */
/* ========================================================================== */
/*                          ����������                                        */
/* ========================================================================== */

/*******************************************************************************
* ������  : OSA_fileInit
* ��  ��  : �ú�����ʼ��ϵͳ�ļ�ϵͳ
* ��  ��  : ��
* ��  ��  : ��
* ����ֵ  : OSA_EFAIL: ʧ��
*           OSA_SOK:   �ɹ�
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
* ������  : OSA_fileDeInit
* ��  ��  : �ú�������ʼ��ϵͳ�ļ�ϵͳ
* ��  ��  : ��
* ��  ��  : ��
* ����ֵ  : OSA_EFAIL: ʧ��
*           OSA_SOK:   �ɹ�
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
* ������  : OSA_getSdSize
* ��  ��  : �ļ��ռ�ռ�����
* ��  ��  : ��
* ��  ��  : ��
* ����ֵ  : OSA_EFAIL: ʧ��
*           OSA_SOK:   �ɹ�
*******************************************************************************/
int OSA_getSdSize(SD_sizeInfo *pSdSizeInfo)
{
    FATFS *fs;
    DWORD fre_clust;        
    FRESULT res;
    
    res = f_getfree(mySDPath, &fre_clust, &fs);  /* �����Ǹ�Ŀ¼��ѡ�����0 */
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
* ������  : OSA_fileOpen
* ��  ��  : �ú��������һ���ļ�,֧�����ֲ���ģʽ: ֻ����ֻд����д������׷��д
*           ���ļ��򿪳ɹ�,������һ���ļ����,�����������ļ������ӿ�ʱ����
*
* ��  ��  : - fileName: �����ļ�·����Ϣ�������ļ���
*           - mode:     �ļ�����ģʽ,�μ�OSA_FileMode����
*
* ��  ��  : - phFile:   �ļ����ָ��,���򿪳ɹ�ʱ����ļ����
* ����ֵ  :  OSA_SOK:   �򿪳ɹ�
*            OSA_EFAIL: ��ʧ��
*******************************************************************************/
int OSA_fileOpen(const char *fileName, OSA_FileMode mode, 
                    OSA_FileHandle *phFile)
{
    OSA_FileObject *pFileObj = NULL;
    BYTE modeFlag;
    
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
        case OSA_FILEMODE_RDWR:
            modeFlag = FA_CREATE_ALWAYS | FA_WRITE;
            break;
        default:
            printf("Invalid mode:%d\n", mode);
            return OSA_EFAIL;
    }

    /*�����ڴ�*/
   /* pFileObj = (OSA_FileObject *)malloc(sizeof(OSA_FileObject));
    if(NULL == pFileObj)
    {
        printf("alloc pFileObj failed\n");
        return OSA_EFAIL;
    }*/
    pFileObj = &gFileObj;
    
    if(f_open(&pFileObj->MyFile, fileName, modeFlag) != FR_OK)
    {
        printf("Open failed\n");
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
* ������  : OSA_fileClose
* ��  ��  : �ú�������ر�һ���ļ�,�������ļ����ָ��Ķ���,
*           ��˵��øýӿں�ԭ�е��ļ����������ʹ�á�
*
* ��  ��  : - hFile: �ļ����
* ��  ��  : ��
* ����ֵ  : OSA_SOK:   �رճɹ�
*           OSA_EFAIL: �ر�ʧ��
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

    /*ħ����Ϊ-1���������������ͷź������*/
    pFileObj->nMgicNum = (int)-1;
    ///free(pFileObj);
    return OSA_OK;
}

/*******************************************************************************
* ������  : OSA_removeFile
* ��  ��  : ɾ��ָ���ļ�
* ��  ��  : ��
* ��  ��  : ��
* ����ֵ  : OSA_SOK:   ɾ���ɹ�
*           OSA_EFAIL: ɾ��ʧ��
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
* ������  : OSA_fileRead
* ��  ��  : �ú�������ʵ���ļ��Ķ�����,����������ɵ���������,�ҵ����߱��뱣֤
*           buffer�ĳ���Ҫ���ڵ���size
*
* ��  ��  : - hFile:   �ļ����
*           - buffer:  ��������
*           - size:    Ҫ��ȡ�ĳ���
* ��  ��  : ��
* ����ֵ  : OSA_EFAIL: ��ȡʧ��
*           >= 0:      ������ȡ���ֽ���
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
    
    if((bytesread == 0) || (ret != FR_OK))
    {
        return OSA_EFAIL;
    }

    return bytesread;
}


/*******************************************************************************
* ������  : OSA_fileWrite
* ��  ��  : �ú�������ʵ���ļ���д����,������buffer�����ɵ���������,�ҵ����߱��뱣֤
*           buffer�ĳ���Ҫ���ڵ���size
*
* ��  ��  : - hFile:   �ļ����
*           - buffer:  д������
*           - size:    Ҫд��ĳ���
* ��  ��  : ��
* ����ֵ  : OSA_EFAIL: д��ʧ��
*           >= 0:      ����д����ֽ���
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
* ������  : OSA_fileSeek
* ��  ��  : �ú�������ʵ���ļ���Seek����
*
* ��  ��  : - hFile:   �ļ����
*           - offset:  seek�Ĵ��ļ�ͷƫ���ֽ���
* ��  ��  : ��
* ����ֵ  : OSA_EFAIL: seekʧ��
*           OSA_SOK:   seek�ɹ�
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
* ������  : OSA_fileSync
* ��  ��  : �ú��������ļ������������ˢ��Ӳ��
*
* ��  ��  : - hFile: �ļ����
* ��  ��  : ��
* ����ֵ  : OSA_SOK:   �ɹ�
*           OSA_EFAIL: ʧ��
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