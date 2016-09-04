
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __PingPangBuffer_H
#define __PingPangBuffer_H

#ifdef __cplusplus
extern "C" {
#endif



#include <stdbool.h>
#include "AaInclude.h"



#define PINGPANG_BUFFER_DEEP    100



typedef struct {
    u32     buf[PINGPANG_BUFFER_DEEP];
    u16     idx;
    bool    full;
} SPPChild;



typedef struct SPingpang_t {
    SPPChild    buf_a;
    SPPChild    buf_b;
    SPPChild*   storing;
    SPPChild*   handling;
} SPingpang;



void PingPangInit(SPingpang * _this);
i8 PingPangPut(SPingpang * _this, u32 val);
i8 PingPangIsFull(SPingpang * _this, bool* full);
u32* PingPangGet(SPingpang * _this, u16 *len);
i8 PingPangExchange(SPingpang * _this);
void PingPandPrint(SPingpang * _this);



#ifdef __cplusplus
}
#endif

#endif /* __PingPangBuffer_H */




