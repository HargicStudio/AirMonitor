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

u32 myPow(u32 val, u32 times)
{
    u32 rst = 0;
    if (times == 0)
      return 1;
    
    rst = val;
    while(--times)
    {
        rst *= val;  
    }
    
    return rst;
}

u16 getFebDays(u16 year)
{
    if ((year%4 == 0 && year % 100 != 0) || year % 400 == 0)
    {
        return 29;
    }
    
    return 28;
}

u16 getdayOfMon(u16 year, u16 month)
{
    switch (month)
    {
    case 2:
      return getFebDays(year);
    case 1:
    case 3:
    case 5:
    case 7:
    case 8:
    case 10:
    case 12:
      return 31;
    case 4:
    case 6:
    case 9:
      return 30;
    default:
      GSM_LOG_P1("ERROR Month: %02d", month);
      return 30;   
    }
}