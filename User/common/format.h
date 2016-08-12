#ifndef _FORMAT_H
#define _FORMAT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"
	
#define MAX_SEND_BUFF_LEN				100
#define MAX_SEND_BIG_BUFF_LEN                           1024

enum SEND_FLAG_e
{
    SEND_FLAG_NO,
    SEND_FLAG_YES,
};

/* 发送缓冲区 */
typedef struct SEND_BUF_t
{
    u16 sendFlag;         // 数据是否准备好的标识
    u16 respFlag;         // 这条消息是否需要回应
    u16 useLen;
    u8 buf[MAX_SEND_BUFF_LEN];
    
}SEND_BUF_t;

/* 用于发送缓冲 */
extern SEND_BUF_t g_sendBuf;

/* 用于接收数据回应的缓冲区，避免block接收 */
extern SEND_BUF_t g_sendResponse;


/* 用于发送大量数据的发送缓冲区，比如回调数据 */
typedef struct SEND_BIG_BUF_t
{
    u16 sendFlag;         // 数据是否准备好的标识
    u16 respFlag;         // 这条消息是否需要回应
    u16 useLen;
    u8 buf[MAX_SEND_BIG_BUFF_LEN];
    
}SEND_BIG_BUF_t;

/* 回调应答的数据缓冲 */
extern SEND_BIG_BUF_t g_sendRecallData;


/* 如果发送不满足要求，可能需要使用环形buffer */
#define SEND_RESPONSE_OFFSET(x)              (g_sendResponse.buf + (x))
#define SEND_RESPONSE_SET_BYTE(x, pos)       (g_sendResponse.buf[(pos)] = x)
#define SEND_RESPONSE_SET_LEN(x)             (g_sendResponse.useLen = x)
#define SEND_RESPONSE_GET_LEN()              (g_sendResponse.useLen)
#define SEND_RESPONSE_RESET()                (g_sendResponse.useLen = 0)
#define SEND_RESPONSE_FLAG_SET()             (g_sendResponse.sendFlag = SEND_FLAG_YES)
#define SEND_RESPONSE_FLAG_CLEAR()           (g_sendResponse.sendFlag = SEND_FLAG_NO)
#define SEND_RESPONSE_RESP_FALG_SET(x)       (g_sendResponse.respFlag = x)
#define SEND_RESPONSE_RESP_FALG_GET()        (g_sendResponse.respFlag)
#define IsSendResponseReady()                (g_sendResponse.sendFlag == SEND_FLAG_YES)


#define SEND_BUF_OFFSET(x)              (g_sendBuf.buf + (x))
#define SEND_BUF_SET_BYTE(x, pos)       (g_sendBuf.buf[(pos)] = x)
#define SEND_BUF_SET_LEN(x)             (g_sendBuf.useLen = x)
#define SEND_BUF_GET_LEN()              (g_sendBuf.useLen)
#define SEND_BUF_RESET()                (g_sendBuf.useLen = 0)
#define SEND_BUF_FLAG_SET()             (g_sendBuf.sendFlag = SEND_FLAG_YES)
#define SEND_BUF_FLAG_CLEAR()           (g_sendBuf.sendFlag = SEND_FLAG_NO)
#define SEND_BUF_RESP_FALG_SET(x)       (g_sendBuf.respFlag = x)
#define SEND_BUF_RESP_FALG_GET()        (g_sendBuf.respFlag)
#define IsSendBufReady()                (g_sendBuf.sendFlag == SEND_FLAG_YES)



void FormatHead(u16 crc, u16 len, u8 *buf);
u16 Format8(u8 val, u8 *buf);
u16 Format16(u16 val, u8 *buf);
u16 Format32(u32 val, u8 *buf);
u16 FormatTime(u8 *strTime, u8 *buf);



#ifdef __cplusplus
}
#endif

#endif

