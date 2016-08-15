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

/* 为避免占用太多栈空间，使用全局变量 */
static u8 s_cmd[52] = {0}; 

static u8 s_testBoot[] = "AT\r\n";
static u8 s_testReg[] = "AT+COPS?\r\n";
static u8 s_testCGCLASS[] = "AT+CGCLASS=\"B\"\r\n";
static u8 s_testCGDCONT[] = "AT+CGDCONT=1,\"IP\",\"CMNET\"\r\n";
static u8 s_testCGATT[] = "AT+CGATT=1\r\n";
static u8 s_testCIPCSGP[] = "AT+CIPCSGP=1,\"CMNET\"\r\n";
static u8 s_testCLPORT[] = "AT+CLPORT=\"TCP\",\"2222\"\r\n";
static u8 s_testCIPMODE[] = "AT+CIPMODE=1\r\n";
static u8 s_testCIPSTART[] = "AT+CIPSTART=\"TCP\",\"%s\",\"%d\"\r\n";

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
*
*/
u8 GetCmdDataLen(u16 cmd)
{
    switch (cmd)
    {
    case 1:  /* 服务器呼叫 */
    case 5:  /* 掉电模式 */
    case 19: /* 请求软件版本 */
    case 21: /* 请求当前数据 */
      return 0;
    case 3:  /* 校时 */
      return 12;
    case 7:  /* 修改站号 */
      return 5;
    case 11: /* 修改采集间隔 */
    case 13: /* 修改上报间隔 */
      return 2;
    case 81: /* 回调数据1 */
      return 10;
    case 83: /* 回调数据2 */
      return 8;
    case 85: /* 回调天的数据 */
      return 6;
    case 501: /* 校准信息 */
      return 32;
    case 502: /* 配置参数 */
      return 15;
    case 503: /* 请求经纬度 */
      return 0;
    default: /* Don't support command */
      return 0xff;
    }
}

bool IsCommandSuss()
{
    if (AT_SUSS == GetAtStatus())
    {
        SetAtStatus(AT_INVALID);
        return true;
    }
    else
    {
        SetAtStatus(AT_INVALID);
        return false;
    }
}
/*
*  执行AT指令
*  因为只有GSM thread会执行这个指令，所以无需加锁
*  等待回应，每50ms检查一次结果，waittime控制等待时间
*/
/*
bool AtCmdRun(u8 *cmd, u16 len, u16 waitTimes, u32 rspType)
{
    GsmSendToUSART(cmd, len);
    SetAtStatus(rspType);

    osDelay(waitTimes*50);
    
    if (IsCommandSuss())
    {
        GSM_LOG_P1("AT CMD Success 1: %s\r\n", cmd);
        return true;
    }
    
    GSM_LOG_P1("AT CMD Failed: %s\r\n", cmd);
    return false;
}*/

bool AtCmdRun(u8 *cmd, u16 len, u16 waitTimes, u32 rspType)
{
    u16 temp = waitTimes;
    
    GsmSendToUSART(cmd, len);
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
    if (true == AtCmdRun((u8 *)"AT+CIPSTATUS\r\n", 14, MAX_WAIT_TIMES, AT_WAIT_CONNECT_STU))
    {
        GSM_LOG_P0("TCP is connected!");
        GsmStatusSet(GSM_TCP_CONNECTED);
        return true;
    }
    
    GsmStatusSet(GSM_FREE);
    return false;
}

bool IsGsmRunning(void)
{
    if (true == AtCmdRun((u8 *)"AT\r\n", 3, 20, AT_WAIT_RSP))
    {
        return true;
    } 
    
    GsmStatusSet(GSM_FREE);
    return false;
}

void GsmPowerUpDownOpt(u8 type)
{
    int tiemLen = type == GSM_POWER_UP ? 2000 : 3000;
    GsmStatusSet(GSM_FREE);
    GsmPowerDown();
    osDelay(tiemLen);
    GsmPowerUp();
    
    osDelay(30000);
    
    GSM_LOG_P1("GSM POWER control: %d\r\n", type);
}

/**
  * @brief  startup
  * @param  none
  * @retval None
  */
bool GsmStartup(void)
{
    osDelay(100);
    if (true == AtCmdRun(s_testBoot, 4, 20, AT_WAIT_RSP))
    {
        return true;
    }
    
    GsmPowerUpDownOpt(GSM_POWER_UP);
    
    if (true == AtCmdRun(s_testBoot, 4, 20, AT_WAIT_RSP))
    {
        return true;
    }
    
    GsmPowerUpDownOpt(GSM_POWER_UP);
    
    if (true == AtCmdRun(s_testBoot, 4, 20, AT_WAIT_RSP))
    {
        return true;
    }
    
    return false;
}

bool GsmWaitForReg(void)
{
    bool ret;
    /*  wait for 5S */
    ret = AtCmdRun(s_testReg, 10, 100, AT_WAIT_REG);
    if (!ret)
    {
        ret = AtCmdRun(s_testReg, 10, 100, AT_WAIT_REG);
    }
    
    return ret;
}

bool GsmStartAndconect(void)
{
    /* 关闭链接 */
    /*
    if (false == AtCmdRun((u8 *)"AT+CIPCLOSE=1\r\n", 15, MAX_WAIT_TIMES, AT_WAIT_RSP))
    {
        //return false;
    }*/

    if (!GsmStartup())
    {
       AaSysLogPrintF(LOGLEVEL_INF, FeatureGsm, "GSM startup fail!");
       return false;
    }
    
    AaSysLogPrintF(LOGLEVEL_INF, FeatureGsm, "GSM boot sucess!");

    /* 关闭链接 */
    /*
    if (false == AtCmdRun((u8 *)"AT+CIPCLOSE=1\r\n", 15, MAX_WAIT_TIMES, AT_WAIT_RSP))
    {
        //return false;
    }*/
    
    if (!GsmWaitForReg())
    {
       AaSysLogPrintF(LOGLEVEL_INF, FeatureGsm, "GSM register fail!");
       return false;
    }
    
    AaSysLogPrintF(LOGLEVEL_INF, FeatureGsm, "GSM Reg success!");

    if (false == AtCmdRun(s_testCGCLASS, 16, MAX_WAIT_TIMES, AT_WAIT_RSP))
    {
        return false;
    }

    if (false == AtCmdRun(s_testCGDCONT, 27, MAX_WAIT_TIMES, AT_WAIT_RSP))
    {
        return false;
    }

    if (false == AtCmdRun(s_testCGATT, 12, MAX_WAIT_TIMES, AT_WAIT_RSP))
    {
        return false;
    }

    if (false == AtCmdRun(s_testCIPCSGP, 22, MAX_WAIT_TIMES, AT_WAIT_RSP))
    {
        //return false;
    }

    if (false == AtCmdRun(s_testCLPORT, 24, MAX_WAIT_TIMES, AT_WAIT_RSP))
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
    
    if (false == AtCmdRun(s_testCIPMODE, 14, MAX_WAIT_TIMES, AT_WAIT_RSP))
    {
        //return false;
    }
    
    memset(s_cmd, 0, sizeof(s_cmd));
    sprintf((char *)s_cmd, s_testCIPSTART, ConfigGetServerIp(), ConfigGetServerPort());
    
    GSM_LOG_P1("CONNECT TO %s END!", s_cmd);
    
    if (false == AtCmdRun(s_cmd, strlen((char const *)s_cmd), MAX_WAIT_TIMES + 100, AT_WAIT_CONNECT_RSP))
    {
        return AtCmdRun(s_cmd, strlen((char const *)s_cmd), MAX_WAIT_TIMES + 100, AT_WAIT_CONNECT_RSP);
    }
    GsmStatusSet(GSM_TCP_CONNECTED);
    AaSysLogPrintF(LOGLEVEL_INF, FeatureGsm, "GSM connected to the server!");
    
    /* DTU head, 为了兼容单组分 */
    memcpy(s_cmd + 11, ConfigGetStrAddr(), 5);
    GSM_LOG_P1("Send Head: %s\r\n", s_cmd);
    GsmSendToUSART(s_cmd, 52);
    osDelay(100);
    
    return true;   
}

bool GsmSendData(u8 *data, u16 len, u8 respFlag)
{
    /* 测试，直接发送 */ 
    GsmSendToUSART(data, len);
    /*
    if (!respFlag)
    {
        // 透传模式发送数据 /
        GsmSendToUSART(data, len);
    }
    else
    {
        // 需要服务器响应 
        if (false == AtCmdRun(data, len, MAX_WAIT_TIMES + 30, AT_WAIT_SEND_OK))
        {
            GSM_LOG_P0("----Send failed, power down GSM!----\r\n");
            GsmPowerUpDownOpt(GSM_POWER_DOWN);
            return false;
        }
    }
    */
    
    /* 透传模式下，等待服务器的响应, 出错则关闭GSM */
    /*
    if (false == AtCmdRun(data, len, MAX_WAIT_TIMES + 300, AT_WAIT_SEND_OK))
    {
        GsmPowerUpDownOpt(GSM_POWER_DOWN);
        return false;
    }*/
    
    
    
    /*
    //GsmSendToUSART(send, strlen(send));
    if (false == AtCmdRun(send, len, MAX_WAIT_TIMES+20, AT_WAIT_TO_SEND))
    {
        return false;
    }
    
    //GsmSendToUSART(data, len);
    
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






        
