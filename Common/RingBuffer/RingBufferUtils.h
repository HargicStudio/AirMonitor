/**
******************************************************************************
* @file    RingBufferUtils.h 
* @author  William Xu
* @version V1.0.0
* @date    05-May-2014
* @brief   This header contains function prototypes called by ring buffer 
*          operation
******************************************************************************
* @attention
*
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
* TIME. AS A RESULT, MXCHIP Inc. SHALL NOT BE HELD LIABLE FOR ANY
* DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
* FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
* CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*
* <h2><center>&copy; COPYRIGHT 2014 MXCHIP Inc.</center></h2>
******************************************************************************
*/ 

#ifndef __RingBufferUtils_h__
#define __RingBufferUtils_h__

#include "AaInclude.h"

typedef struct
{
  u32  size;
  u32  head;
  u32  tail;
  u8*  buffer;
} ring_buffer_t;

#ifndef MIN
#define MIN(x,y)  ((x) < (y) ? (x) : (y))
#endif /* ifndef MIN */

EStatus ring_buffer_init( ring_buffer_t* ring_buffer, u8* buffer, u32 size );

EStatus ring_buffer_deinit( ring_buffer_t* ring_buffer );

u32 ring_buffer_free_space( ring_buffer_t* ring_buffer );

u32 ring_buffer_used_space( ring_buffer_t* ring_buffer );

u8 ring_buffer_get_data( ring_buffer_t* ring_buffer, u8** data, u32* contiguous_bytes );

u8 ring_buffer_consume( ring_buffer_t* ring_buffer, u32 bytes_consumed );

u32 ring_buffer_write( ring_buffer_t* ring_buffer, const u8* data, u32 data_length );

/* 用于串口每次接收一个字节的情况 */
u32 ring_buffer_write_c(ring_buffer_t* ring_buffer, u8 data);

/* 取一段以换行符为结束标志的串 */
u8 ring_buffer_consume_enter( ring_buffer_t* ring_buffer,  u8 *buf, u16 *len);

/* 取一段以 ffffff 为结束标志的串 */
u8 ring_buffer_consume_str( ring_buffer_t* ring_buffer,  u8 *buf, u16 *len);

#endif // __RingBufferUtils_h__


