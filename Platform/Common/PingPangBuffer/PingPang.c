

/***

History:
[2016-08-11 Ted]: Create

*/


#include <stdio.h>
#include <string.h>
#include "PingPang.h"



void PingPangInit(SPingpang * _this)
{
    memset(_this, 0, sizeof(SPingpang));
    _this->storing = &_this->buf_a;
}


i8 PingPangPut(SPingpang * _this, u32 val)
{
    if(_this->storing == NULL) {
        return -1;
    }

    SPPChild* pp_storing = _this->storing;

    if(pp_storing->idx >= PINGPANG_BUFFER_DEEP) {
        return -2;
    }

    pp_storing->buf[pp_storing->idx] = val;
    pp_storing->idx++;
    if(pp_storing->idx >= PINGPANG_BUFFER_DEEP) {
        pp_storing->full = true;
        pp_storing->idx = 0;
    }

    return 0;
}


i8 PingPangIsFull(SPingpang * _this, bool* full)
{
    if(_this->storing == NULL) {
        *full = false;
        return -1;
    }

    SPPChild* pp_storing = _this->storing;

    *full = pp_storing->full;

    return 0;
}


u32* PingPangGet(SPingpang * _this, u16 *len)
{
    if(_this->handling == NULL) {
        *len = 0;
        return NULL;
    }

    SPPChild* pp_handling = _this->handling;

    if(pp_handling->full == true) {
        *len = PINGPANG_BUFFER_DEEP;
    } else {
        *len = pp_handling->idx;
    }

    // re-initialize
    pp_handling->full = false;
    pp_handling->idx = 0;
    _this->handling = NULL;

    return pp_handling->buf;
}


i8 PingPangExchange(SPingpang * _this)
{
    if(_this->storing == NULL) {
        // there is no buffer in storing
        return -1;
    }

    if(_this->handling != NULL) {
        // handling buffer had not been taken away
        return -2;
    }

    _this->handling = _this->storing;
    if(_this->storing == &_this->buf_a) {
        _this->storing = &_this->buf_b;
    } else {
        _this->storing = &_this->buf_a;
    }

    return 0;
}


void PingPandPrint(SPingpang * _this)
{
    if(_this == NULL) {
        return ;
    }

    AaSysLogPrintF(LOGLEVEL_DBG, FeaturePP, "******************************");
    AaSysLogPrintF(LOGLEVEL_DBG, FeaturePP, "buf_a addr %p", &_this->buf_a);
    AaSysLogPrintF(LOGLEVEL_DBG, FeaturePP, "buf_b addr %p", &_this->buf_b);
    AaSysLogPrintF(LOGLEVEL_DBG, FeaturePP, "storing %p", _this->storing);
    AaSysLogPrintF(LOGLEVEL_DBG, FeaturePP, "handling %p", _this->handling);

    SPPChild* pp_buf = &_this->buf_a;
    AaSysLogPrintF(LOGLEVEL_DBG, FeaturePP, "buf_a idx %d", pp_buf->idx);
    AaSysLogPrintF(LOGLEVEL_DBG, FeaturePP, "buf_a full %s", pp_buf->full ? "true" : "false");
    
    pp_buf = &_this->buf_b;
    AaSysLogPrintF(LOGLEVEL_DBG, FeaturePP, "buf_b idx %d", pp_buf->idx);
    AaSysLogPrintF(LOGLEVEL_DBG, FeaturePP, "buf_b full %s", pp_buf->full ? "true" : "false");
    AaSysLogPrintF(LOGLEVEL_DBG, FeaturePP, "******************************");
}


// end of file


