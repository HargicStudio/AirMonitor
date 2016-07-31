
/***

History:
[2016-05-21 Ted]: Create

*/

#ifndef _GLO_DEF_H
#define _GLO_DEF_H

#ifdef __cplusplus
 extern "C" {
#endif 


#include "cmsis_os.h"



typedef unsigned char u8;
typedef signed char i8;
typedef unsigned short u16;
typedef signed short i16;
typedef unsigned long u32;
typedef signed long i32;


typedef signed char s8;
typedef signed short s16;
typedef signed long s32;



typedef enum EStatus_t {
    ErrOk = 0,
    ErrInitialize,
    ErrParam,
    ErrMem,
    ErrRTOS,
} EStatus;


#ifdef __cplusplus
}
#endif

#endif // _GLO_DEF_H

// end of file

