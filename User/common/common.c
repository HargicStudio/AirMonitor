#include "common.h"

u32 stringToInt(u8 *buf, u16 len)
{
    u32 i = 0;
    u32 rst = 0;
    
    for (i=0; i<len; i++)
    {
        rst += (buf[len - 1 - i] - '0') * myPow(10, i); 
    }
    
    return rst;
}

bool IsDirExit(u8 *path)
{
    DIR fd;
    if (FR_OK == f_opendir(&fd, path))
    {
        f_closedir(&fd);
        return true;
    }
    
    return false;
}