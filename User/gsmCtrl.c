#include "gsm_power_dev.h"
#include "gsmCtrl.h"
#include "AaInclude.h"
#include "common.h"
#include "gsmCtrl.h"

/* AT command 状态*/
s32 _at_status;

/* GSM 的状态 */
s32 _gsm_status;

/* 发送后置1，开始准备关机 */
u8 _time_wait_close_flag;


void GsmWaitCloseFlagSet(void)
{
    _time_wait_close_flag = 1;
}

void GsmWaitCloseFlagClear(void)
{
    _time_wait_close_flag = 0;
}

bool IsGsmWaitCloseFlag(void)
{
    return (_time_wait_close_flag == 1);
}

void GsmStatusSet(u32 stu)
{
    _gsm_status = stu;
}

u32 GsmStatusGet(void)
{
    return _gsm_status;
}
/*
*  GSM 流程控制
*/
void AtValueInit()
{
    _at_status = AT_INVALID;
}

void SetAtStatus(s32 stu)
{
    _at_status = stu;
}

s32 GetAtStatus(void)
{
    return _at_status;
}
/*
*  执行AT指令
*  因为只有GSM thread会执行这个指令，所以无需加锁
*  等待回应，每50ms检查一次结果，waittime控制等待时间
*/
bool AtCmdRun(u8 *cmd, u16 len, u16 waitTimes, u32 rspType)
{
    u16 temp = waitTimes;
    /* lock */
    GsmDataSendByIT(cmd, len);
    SetAtStatus(rspType);
    while (rspType == GetAtStatus())
    {
        osDelay(50);
        
        if (0 == waitTimes)
        {
            GSM_LOG_P2("AT CMD fail: %s, Times: %d\r\n", cmd, temp - waitTimes);
            SetAtStatus(AT_INVALID);
            return false;
        }
        waitTimes--;
    }
    
    if (AT_ERROR == GetAtStatus())
    {
        GSM_LOG_P2("AT CMD ERROR: %s, Times: %d\r\n", cmd, temp - waitTimes);
        SetAtStatus(AT_INVALID);
        return false;
    }
    
    SetAtStatus(AT_INVALID);
    
    GSM_LOG_P2("AT CMD Success: %s, Times: %d\r\n", cmd, temp - waitTimes);
    /* unlock */
    
    return true;
}

void IsAtSuss(u8 *buf, u8 *key)
{
    if (strstr((const char *)buf, key))
    {
        SetAtStatus(AT_SUSS);
    }
    
    return;
}

/*
*  GSM 流程控制 end
*/

bool IsTcpConnected(void)
{
    int num = 0;
    
    while(num++ < 1)
    {
        if (true == AtCmdRun((u8 *)"AT+CIPSTATUS\r\n", 14, MAX_WAIT_TIMES, AT_WAIT_CONNECT_STU))
        {
            GSM_LOG_P0("TCP is connected!");
            GsmStatusSet(GSM_TCP_CONNECTED);
            return true;
        }
    }
    
    GsmStatusSet(GSM_FREE);
    return false;
}

bool IsGsmRunning(void)
{
    int num = 0;
    
    while (num++ < 1)
    {
        if (true == AtCmdRun((u8 *)"AT\r\n", 3, 30, AT_WAIT_RSP))
        {
            return true;
        }
    }
    
    GsmStatusSet(GSM_FREE);
    return false;
}

void GsmPowerUpDownOpt(u8 type)
{
    int tiemLen = type == GSM_POWER_UP ? 1500 : 3000;
    GsmStatusSet(GSM_FREE);
    GsmPowerDown();
    osDelay(tiemLen);
    GsmPowerUp();
}

/**
  * @brief  startup
  * @param  none
  * @retval None
  */
bool GsmStartup(void)
{
    u8 testNum = 0;
    
    while(!IsGsmRunning())
    {
        GsmPowerUpDownOpt(GSM_POWER_UP);
        
        testNum++;
        if (testNum == 10)
        {
            return false;
        }
        
        GSM_LOG_P1("GSM startup try again! %d", testNum);
    }
    
    return true;
}

bool GsmWaitForReg(void)
{
    /*  wait for 5S */
    return AtCmdRun((u8 *)"AT+COPS?\r\n", 10, 200, AT_WAIT_REG);
}

bool GsmStartAndconect(void)
{
    u8 cmd[50] = {0};
    
    /* 关闭链接 */
    if (false == AtCmdRun((u8 *)"AT+CIPCLOSE=1\r\n", 15, MAX_WAIT_TIMES, AT_WAIT_RSP))
    {
        //return false;
    }

    if (!GsmStartup())
    {
       AaSysLogPrintF(LOGLEVEL_INF, FeatureGsm, "GSM startup fail!");
       return false;
    }
    
    AaSysLogPrintF(LOGLEVEL_INF, FeatureGsm, "GSM boot sucess!");

    /* 关闭链接 */
    if (false == AtCmdRun((u8 *)"AT+CIPCLOSE=1\r\n", 15, MAX_WAIT_TIMES, AT_WAIT_RSP))
    {
        //return false;
    }
    
    if (!GsmWaitForReg())
    {
       AaSysLogPrintF(LOGLEVEL_INF, FeatureGsm, "GSM register fail!");
       return false;
    }
    
    AaSysLogPrintF(LOGLEVEL_INF, FeatureGsm, "GSM Reg success!");

    if (false == AtCmdRun((u8 *)"AT+CGCLASS=\"B\"\r\n", 16, MAX_WAIT_TIMES, AT_WAIT_RSP))
    {
        return false;
    }

    if (false == AtCmdRun((u8 *)"AT+CGDCONT=1,\"IP\",\"CMNET\"\r\n", 27, MAX_WAIT_TIMES, AT_WAIT_RSP))
    {
        return false;
    }

    if (false == AtCmdRun((u8 *)"AT+CGATT=1\r\n", 12, MAX_WAIT_TIMES, AT_WAIT_RSP))
    {
        return false;
    }

    if (false == AtCmdRun((u8 *)"AT+CIPCSGP=1,\"CMNET\"\r\n", 22, MAX_WAIT_TIMES, AT_WAIT_RSP))
    {
        //return false;
    }

    if (false == AtCmdRun((u8 *)"AT+CLPORT=\"TCP\",\"2222\"\r\n", 24, MAX_WAIT_TIMES, AT_WAIT_RSP))
    {
        return false;
    }
    
    /* 设置回应 */
    /*
    if (false == AtCmdRun((u8 *)"AT+CIPSPRT=1\r\n", 14, MAX_WAIT_TIMES, AT_WAIT_RSP))
    {
        return false;
    }*/
    
    /* 设置透传模式 */
    
    if (false == AtCmdRun((u8 *)"AT+CIPMODE=1\r\n", 14, MAX_WAIT_TIMES, AT_WAIT_RSP))
    {
        //return false;
    }
    
    sprintf((char *)cmd, "AT+CIPSTART=\"TCP\",\"%s\",\"%d\"\n", ConfigGetServerIp(), ConfigGetServerPort());
    
    if (false == AtCmdRun(cmd, strlen((char const *)cmd), MAX_WAIT_TIMES + 100, AT_WAIT_CONNECT_RSP))
    {
        return false;
    }
    GsmStatusSet(GSM_TCP_CONNECTED);
    AaSysLogPrintF(LOGLEVEL_INF, FeatureGsm, "GSM connected to the server!");
    
    return true;   
}

bool GsmSendData(u8 *data, u16 len, u8 respFlag)
{
    /* 测试，直接发送 */ 
    GsmDataSendByIT(data, len);
    /*
    if (!respFlag)
    {
        // 透传模式发送数据 /
        GsmDataSendByIT(data, len);
    }
    else
    {
        // 需要服务器响应 
        if (false == AtCmdRun(data, len, MAX_WAIT_TIMES + 300, AT_WAIT_SEND_OK))
        {
            GsmPowerUpDownOpt(GSM_POWER_DOWN);
            return false;
        }
    }*/
    
    
    /* 透传模式下，等待服务器的响应, 出错则关闭GSM */
    /*
    if (false == AtCmdRun(data, len, MAX_WAIT_TIMES + 300, AT_WAIT_SEND_OK))
    {
        GsmPowerUpDownOpt(GSM_POWER_DOWN);
        return false;
    }*/
    
    
    
    /*
    //GsmDataSendByIT(send, strlen(send));
    if (false == AtCmdRun(send, len, MAX_WAIT_TIMES+20, AT_WAIT_TO_SEND))
    {
        return false;
    }
    
    //GsmDataSendByIT(data, len);
    
    if (false == AtCmdRun(data, len+2, MAX_WAIT_TIMES + 300, AT_WAIT_SEND_OK))
    {
        return false;
    }
    */
    return true;
}

bool SendData(u8 *str, u16 len, u8 respflag)
{
    if (GSM_TCP_CONNECTED == GsmStatusGet())
        return GsmSendData(str, len, respflag);
      
    if (!IsTcpConnected())
    {
        AaSysLogPrintF(LOGLEVEL_ERR, FeatureGsm, "%s %d: Send data, but error status! Try to Connect!",
                    __FUNCTION__, __LINE__);
        if (!GsmStartAndconect())
        {
            AaSysLogPrintF(LOGLEVEL_ERR, FeatureGsm, "%s %d: Send data in error stu, and startup fail!",
                    __FUNCTION__, __LINE__);
            return false;
        }
    }
    
    return GsmSendData(str, len, respflag);
}






        
