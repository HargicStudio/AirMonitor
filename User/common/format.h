#ifndef _FORMAT_H
#define _FORMAT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"
	
#define MAX_SEND_BUFF_LEN				1024

enum SEND_FLAG_e
{
    SEND_FLAG_NO,
    SEND_FLAG_YES,
};

/* 发送缓冲区 */
typedef struct SEND_BUF_t
{
    u16 sendFlag;         // 数据是否准备好的标识
    u16 useLen;
    u8 buf[MAX_SEND_BUFF_LEN];
    
}SEND_BUF_t;

/* 用于发送缓冲 */
extern SEND_BUF_t g_sendBuf;

/* 用于接收数据回应的缓冲区，避免block接收 */
extern SEND_BUF_t g_sendResponse;


#define SEND_BUF_OFFSET(x)              (g_sendBuf.buf + (x))
#define SEND_BUF_SET_BYTE(x, pos)       (g_sendBuf.buf[(pos)] = x)
#define SEND_BUF_SET_LEN(x)             (g_sendBuf.useLen = x)
#define SEND_BUF_GET_LEN()              (g_sendBuf.useLen)
#define SEND_BUF_RESET()                (g_sendBuf.useLen = 0)
#define SEND_BUF_FLAG_SET()             (g_sendBuf.sendFlag = SEND_FLAG_YES)
#define SEND_BUF_FLAG_CLEAR()           (g_sendBuf.sendFlag = SEND_FLAG_NO)
#define IsSendBufReady()                (g_sendBuf.sendFlag == SEND_FLAG_YES)


#define SEND_REPORT_OFFSET(x)              (g_sendResponse.buf + (x))
#define SEND_REPORT_SET_BYTE(x, pos)       (g_sendResponse.buf[(pos)] = x)
#define SEND_REPORT_SET_LEN(x)             (g_sendResponse.useLen = x)
#define SEND_REPORT_GET_LEN()              (g_sendResponse.useLen)
#define SEND_REPORT_RESET()                (g_sendResponse.useLen = 0)
#define SEND_REPORT_FLAG_SET()             (g_sendResponse.sendFlag = SEND_FLAG_YES)
#define SEND_REPORT_FLAG_CLEAR()           (g_sendResponse.sendFlag = SEND_FLAG_NO)
#define IsSendResponseReady()                (g_sendResponse.sendFlag == SEND_FLAG_YES)

void FormatHead(u16 crc, u16 len, u8 *buf);
u16 Format8(u8 val, u8 *buf);
u16 Format16(u16 val, u8 *buf);
u16 Format32(u32 val, u8 *buf);



#ifdef __cplusplus
}
#endif

#endif

