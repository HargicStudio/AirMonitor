#include "format.h"

void FormatHead(u16 crc, u16 len, u8 *buf)
{
    u8 offset = 0;
    u16 ver = ConfigGetSoftVer();
    
    buf[0] = 'C';
    buf[1] = 'C';
    offset += 2;
    
    memcpy(buf + offset, (s8 *)&ver, 2);
    offset += 2;
    
    memcpy(buf + offset, &nhtons(len), 2);
    offset += 2;

    memcpy(buf + offset, &nhtons(crc), 2);
    offset += 2;
    
    buf[offset] = 'C';
    buf[offset + 1] = 'C';
}




u16 Format8(u8 val, u8 *buf)
{
    buf[1] = val;

    return 1;
}

u16 Format16(u16 val, u8 *buf)
{
    val = nhtons(val);
    memcpy(buf, &val, sizeof(val));
    
    return 2;
}

u16 Format32(u32 val, u8 *buf)
{
    val = nhtonl(val);

    memcpy(buf, &val, sizeof(val));
  
    return 4;
}






