/**
******************************************************************************
* @file    RingBufferUtils.c 
* @author  William Xu
* @version V1.0.0
* @date    05-May-2014
* @brief   This file contains function called by ring buffer operation
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

#include "RingBufferUtils.h"
#include <string.h>


EStatus ring_buffer_init( ring_buffer_t* ring_buffer, u8* buffer, u32 size )
{
    ring_buffer->buffer     = (u8*)buffer;
    ring_buffer->size       = size;
    ring_buffer->head       = 0;
    ring_buffer->tail       = 0;
    return ErrOk;
}

EStatus ring_buffer_deinit( ring_buffer_t* ring_buffer )
{
//    UNUSED_PARAMETER(ring_buffer);
    return ErrOk;
}

u32 ring_buffer_free_space( ring_buffer_t* ring_buffer )
{
  u32 tail_to_end = ring_buffer->size - ring_buffer->tail;
  return ((tail_to_end + ring_buffer->head) % ring_buffer->size);
}

u32 ring_buffer_used_space( ring_buffer_t* ring_buffer )
{
  u32 head_to_end = ring_buffer->size - ring_buffer->head;
  return ((head_to_end + ring_buffer->tail) % ring_buffer->size);
}

u8 ring_buffer_get_data( ring_buffer_t* ring_buffer, u8** data, u32* contiguous_bytes )
{
  u32 head_to_end = ring_buffer->size - ring_buffer->head;
  
  *data = &(ring_buffer->buffer[ring_buffer->head]);
  
  *contiguous_bytes = MIN(head_to_end, (head_to_end + ring_buffer->tail) % ring_buffer->size);
  return 0;
}

u8 ring_buffer_consume( ring_buffer_t* ring_buffer, u32 bytes_consumed )
{
  ring_buffer->head = (ring_buffer->head + bytes_consumed) % ring_buffer->size;
  return 0;
}

u32 ring_buffer_write( ring_buffer_t* ring_buffer, const u8* data, u32 data_length )
{
  u32 tail_to_end = ring_buffer->size - ring_buffer->tail;
  
  /* Calculate the maximum amount we can copy */
  u32 amount_to_copy = MIN(data_length, (ring_buffer->tail == ring_buffer->head) ? ring_buffer->size : (tail_to_end + ring_buffer->head) % ring_buffer->size);
  
  /* Copy as much as we can until we fall off the end of the buffer */
  memcpy(&ring_buffer->buffer[ring_buffer->tail], data, MIN(amount_to_copy, tail_to_end));
  
  /* Check if we have more to copy to the front of the buffer */
  if (tail_to_end < amount_to_copy)
  {
    memcpy(ring_buffer->buffer, data + tail_to_end, amount_to_copy - tail_to_end);
  }
  
  /* Update the tail */
  ring_buffer->tail = (ring_buffer->tail + amount_to_copy) % ring_buffer->size;
  
  return amount_to_copy;
}
