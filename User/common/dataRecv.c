#include "dataRecv.h"
#include "AaInclude.h"
#include "common.h"


void HandleGsmRecv(u8 *buf, u16 len)
{
    u16 parmLen = 0;
    u16 parmCrc = 0;
    u32 parmCmd = 0;
    u32 parmAddr = 0;
    u16 crc = 0;
    
    if (len < LEN_HEAD + 1 || !(buf[0] == 'C' && buf[1] == 'C'))
    {
        GSM_LOG_P2("Invild data: len: %d, %s", len, buf);
        return;
    }
    /* ��ȡ��ַ */
    parmAddr = stringToInt(buf + OFFSET_ADDR, LEN_ADDR);
    if (parmAddr != ConfigGetAddr())
    {
        GSM_LOG_P4("Invild Addr: len: %d, Addr: %d:%d, %s", len, parmAddr, ConfigGetAddr(), buf);
        return;
    }
    /* ��ȡ�������� */
    memcpy(&parmLen, (u8 *)buf + OFFSET_LEN, LEN_PARM_LEN);
    parmLen = nhtons(parmLen);
    
    if (len < parmLen + LEN_HEAD)
    {
        GSM_LOG_P3("Invild Len: len: %d, parmLen: %d:%d", len, parmLen, buf);
        return;
    }
    
    /* ��ȡCRC */
    memcpy(&parmCrc, (u8 *)buf + OFFSET_CRC, LEN_CRC);
    parmCrc = nhtons(parmCrc);
    crc = usMBCRC16(buf + LEN_HEAD, parmLen);
    if (parmCrc == crc)
    {
        GSM_LOG_P3("Invild Crc, parmCrc: %d:%d %s", parmCrc, crc, buf);
        return;
    }
    
    /* ��ȡָ�� */
    parmCmd = stringToInt(buf + OFFSET_CMD, LEN_CMD);
    
    GSM_LOG_P4("RECV: addr: %d, len:%d, crc: %d, cmd:%d", parmAddr, parmLen, parmCrc, parmCmd);
    //GsmWaitCloseFlagClear();
    ProcessRecvData(buf + LEN_HEAD + LEN_ADDR, parmCmd);
}

void ProcessRecvData(u8 *buf, int cmd)
{
    switch (cmd)
    {
      /* ��������� */
    case 1:
      ProcessServerCall(buf);
      break;
      /* ���ģʽ */
    case 5:
      ProcessSavePower(buf);
      break;
      /* �޸�վ��ַ */
    case 7:
      ProcessChangeAddr(buf);
      break;
      /* �޸Ĳɼ�ʱ����� �����������û�ã�ÿ������������һ�� */
    case 11:
      ProcessChangeSimpleInterval(buf);
      break;
      /* �޸��ϱ�ʱ���� */
    case 12:
      ProcessChangeReportInterval(buf);
      break;
      /* ��ȡ������״̬ */
    case 15:
      ProcessChangeGetModuleStatus(buf);
      break;
      /* ����汾�� */
    case 19:
      ProcessGetSoftVersion(buf);
      break;
      /* Ӳ���汾�� */
    case 33:
      ProcessGetHardVersion(buf);
      break;
      
        
    }
}

u32 stringToInt(u8 *buf, u16 len)
{
    u32 i = 0;
    u32 rst = 0;
    
    for (i=0; i<len; i++)
    {
        rst += (buf[len - 1 - i] - '0') * 10^i; 
    }
    
    return rst;
}

void ProcessServerCall(u8 *buf)
{
    GSM_LOG_P0("Recv Server Call!");
}

void ProcessSavePower(u8 *buf)
{
    GSM_LOG_P0("Recv Save Power!");
}

void ProcessChangeAddr(u8 *buf)
{
    GSM_LOG_P0("Recv Change ADDR!");
}

void ProcessChangeSimpleInterval(u8 *buf)
{
    GSM_LOG_P0("Recv change simple interval!");
}

void ProcessChangeReportInterval(u8 *buf)
{
    GSM_LOG_P0("Recv change report interval!");
}

void ProcessChangeGetModuleStatus(u8 *buf)
{
    GSM_LOG_P0("Recv Get Module Status!");
}

void ProcessGetSoftVersion(u8 *buf)
{
    GSM_LOG_P0("Recv Get soft version!");
}

void ProcessGetHardVersion(u8 *buf)
{
    GSM_LOG_P0("Recv Get Hard version!");
}

/*
*  Construct response.
*  For Type != 0xff, will be put into buffer 
*/ 
void ConstructResponse(u8 *cmd, u8 type)
{
    u16 offset = 0;
    u32 crc = 0;
    
    SEND_RESPONSE_FLAG_CLEAR();
    
    offset = LEN_HEAD;
    memcpy((s8 *)SEND_RESPONSE_OFFSET(offset), ConfigGetStrAddr(), MAX_ADDR_LEN);
    offset += MAX_ADDR_LEN;
    /* cmd */
    memcpy((s8 *)SEND_RESPONSE_OFFSET(offset), cmd, LEN_CMD);
    offset += LEN_CMD;
    if (type != 0xff)
    {
        SEND_REPORT_SET_BYTE(type, SEND_RESPONSE_OFFSET(offset));
        offset += 1;
    }
    
    /* HEAD ����� */
    /* ����CRC */
    crc = usMBCRC16( SEND_RESPONSE_OFFSET(LEN_HEAD) , offset - LEN_HEAD );
    
    FormatHead(crc, offset - LEN_HEAD, SEND_RESPONSE_OFFSET(0));
    
    SEND_RESPONSE_SET_BYTE('\r', offset);
    SEND_RESPONSE_SET_BYTE('\n', offset + 1);
    
    /* �����ܳ��� */
    SEND_RESPONSE_SET_LEN(offset+2);
    
    
    SEND_RESPONSE_FLAG_SET();
}

