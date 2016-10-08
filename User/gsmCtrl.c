#include "gsm_power_dev.h"
#include "gsmCtrl.h"
#include "AaInclude.h"
#include "common.h"
#include "gsmCtrl.h"

osMutexId _gsm_ctrl_mutex_id;

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

#define BASE_CHECK_TIME                     100           /* base is 100 ms */
#define AT_MAX_RESP_TIME                     20           /* 10 s */          
#define AT_COPS_QUERY_MAX_RESP_TIME         450           /* 45 s */
#define AT_CGCLASS_MAX_RESP_TIME            100           /* no define */     
#define AT_CGDCONT_MAX_RESP_TIME            100           /* no define */
#define AT_CGATT_MAX_RESP_TIME              100           /* 10 s */
#define AT_CIPCSGP_MAX_RESP_TIME            100           /* no define */
#define AT_CLPORT_MAX_RESP_TIME             100           /* no define */
#define AT_CIPMODE_MAX_RESP_TIME            100           /* no define */
#define AT_CIPSTART_MAX_RESP_TIME           100           /* 160 s if IP INTIAL */


#define RESET_CTRL_MAX_TIMES       5
/* 多次无法建立连接，重启 */
static u8 resetCtrl;

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
    osMutexWait(_gsm_ctrl_mutex_id, osWaitForever);
    _at_status = stu;
    osMutexRelease(_gsm_ctrl_mutex_id);
}

s32 GetAtStatus(void)
{
    s32 stu;
    osMutexWait(_gsm_ctrl_mutex_id, osWaitForever);
    stu = _at_status;
    osMutexRelease(_gsm_ctrl_mutex_id);
    return stu;
}

/*
*
*/
u8 GetCmdDataLen(u16 cmd)
{
    switch (cmd)
    {
    case CMD_SER_CHECK_ONLINE_V:  /* 服务器呼叫 */
    case CMD_SER_POWER_SAVE_MODE_V:  /* 掉电模式 */
    case CMD_SER_REPORT_DATA_RSP_V: /* 服务器回应上报数据 */
    case CMD_SER_REBOOT_V: /* 重启 */
    case CMD_SER_STATION_INFO_V:    /* 获取站点信息 */
    case 217: /* 通知停止回调 */
    case 33: /* 请求硬件版本号 */
    case 19: /* 请求软件版本 */
      return 0;
    case CMD_SER_CORRECT_TIME_V:  /* 校时 */
      return 12;
    case CMD_SER_CFG_STATION_V:     /* 配置站点消息 */
      return 4;
    case 7:  /* 修改站号 */
      return 5;
    case 11: /* 修改采集间隔 */
    case 13: /* 修改上报间隔 */
      return 2;
    case 215: /* 新版回调请求 */
      return 28;
    case 81: /* 回调数据1 */
      return 10;
    case 83: /* 回调数据2 */
      return 8;
    case 85: /* 回调天的数据 */
      return 6;
    case 501: /* 校准信息 */
      return 32;
    case 503: /* 配置参数 */
      return 15;
    case 505: /* 请求经纬度 */
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
        osDelay(BASE_CHECK_TIME);
    
        if (0 == waitTimes)
        {
            //GSM_LOG_P2("AT CMD fail: %s, Times: %d\r\n", cmd, temp - waitTimes);
            
            GSM_LOG_E_P4("%s %d: Cmd over try %s Fail! With %d",
                    __FUNCTION__, __LINE__, cmd, temp - waitTimes);
            SetAtStatus(AT_INVALID);
            return false;
        }
        waitTimes--;
    }
  
    if (AT_ERROR == GetAtStatus())
    {
        GSM_LOG_E_P3("%s %d: Cmd %s Fail!",
                    __FUNCTION__, __LINE__, cmd);
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
    if (true == AtCmdRun((u8 *)"AT\r\n", 3, AT_MAX_RESP_TIME, AT_WAIT_RSP))
    {
        return true;
    } 
    
    GsmStatusSet(GSM_FREE);
    return false;
}

void GsmPowerUpDownOpt(u8 type)
{
    int tiemLen = type == GSM_POWER_UP ? 2000 : 3000;
    
    //GSMSetIRQ(false);
    
    GsmStatusSet(GSM_FREE);
    GsmPowerDown();
    osDelay(tiemLen);
    GsmPowerUp();
    
    osDelay(20000);
    //GSMSetIRQ(true);
    //StartReceiveIRQ();
    
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
    if (true == AtCmdRun(s_testBoot, 4, AT_MAX_RESP_TIME, AT_WAIT_RSP))
    {
        return true;
    }
    
    GsmPowerUpDownOpt(GSM_POWER_UP);
    
    if (true == AtCmdRun(s_testBoot, 4, AT_MAX_RESP_TIME, AT_WAIT_RSP))
    {
        return true;
    }
    
    if (true == AtCmdRun(s_testBoot, 4, AT_MAX_RESP_TIME, AT_WAIT_RSP))
    {
        return true;
    }
    
    GsmPowerUpDownOpt(GSM_POWER_UP);
    
    if (true == AtCmdRun(s_testBoot, 4, AT_MAX_RESP_TIME, AT_WAIT_RSP))
    {
        return true;
    }
    
    if (true == AtCmdRun(s_testBoot, 4, AT_MAX_RESP_TIME, AT_WAIT_RSP))
    {
        return true;
    }
    
    return false;
}

bool GsmWaitForReg(void)
{
    bool ret;
    /*  wait for 5S */
    ret = AtCmdRun(s_testReg, 10, AT_COPS_QUERY_MAX_RESP_TIME, AT_WAIT_REG);
    if (!ret)
    {
        ret = AtCmdRun(s_testReg, 10, AT_COPS_QUERY_MAX_RESP_TIME, AT_WAIT_REG);
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
       GSM_LOG_P0("GSM startup fail!");
       return false;
    }
    
    GSM_LOG_P0("GSM boot sucess!");

    /* 关闭链接 */
    /*
    if (false == AtCmdRun((u8 *)"AT+CIPCLOSE=1\r\n", 15, MAX_WAIT_TIMES, AT_WAIT_RSP))
    {
        //return false;
    }*/
    
    if (!GsmWaitForReg())
    {
       GSM_LOG_P0("GSM register fail!");
       return false;
    }
    
    GSM_LOG_P0("GSM Reg success!");

    if (false == AtCmdRun(s_testCGCLASS, 16, AT_CGCLASS_MAX_RESP_TIME, AT_WAIT_RSP))
    {
        return false;
    }

    if (false == AtCmdRun(s_testCGDCONT, 27, AT_CGDCONT_MAX_RESP_TIME, AT_WAIT_RSP))
    {
        return false;
    }

    if (false == AtCmdRun(s_testCGATT, 12, AT_CGATT_MAX_RESP_TIME, AT_WAIT_RSP))
    {
        return false;
    }

    if (false == AtCmdRun(s_testCIPCSGP, 22, AT_CIPCSGP_MAX_RESP_TIME, AT_WAIT_RSP))
    {
        //return false;
    }

    if (false == AtCmdRun(s_testCLPORT, 24, AT_CLPORT_MAX_RESP_TIME, AT_WAIT_RSP))
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
    
    if (false == AtCmdRun(s_testCIPMODE, 14, AT_CIPMODE_MAX_RESP_TIME, AT_WAIT_RSP))
    {
        //return false;
    }
    
    memset(s_cmd, 0, sizeof(s_cmd));
    sprintf((char *)s_cmd, s_testCIPSTART, ConfigGetServerIp(), ConfigGetServerPort());
    
    GSM_LOG_P1("CONNECT TO %s END!", s_cmd);
    
    if (false == AtCmdRun(s_cmd, strlen((char const *)s_cmd), AT_CIPSTART_MAX_RESP_TIME, AT_WAIT_CONNECT_RSP))
    {
        SendDtuHeadData();
        return false;
        /*
        if (false == AtCmdRun(s_cmd, strlen((char const *)s_cmd), MAX_WAIT_TIMES + 100, AT_WAIT_CONNECT_RSP))
        {
            return false;
        }*/
    }
    GsmStatusSet(GSM_TCP_CONNECTED);
    GSM_LOG_E_P0("GSM connected to the server!");
    
    SendDtuHeadData();

    return true;   
}

bool SendDtuHeadData(void)
{
    /* DTU head, 为了兼容单组分 */
    memcpy(s_cmd + 11, ConfigGetStrAddr(), 5);
    GSM_LOG_P1("Send Head: %s\r\n", s_cmd);
    GsmSendToUSART(s_cmd, 52);
    osDelay(100);
}

bool GsmSendData(u8 *data, u16 len, u8 respFlag)
{
    /* 测试，直接发送 */ 
    //GsmSendToUSART(data, len);

    if (!respFlag)
    {
        // 透传模式发送数据 /
        GsmSendToUSART(data, len);
    }
    else
    {
        // 需要服务器响应 
        if (false == AtCmdRun(data, len, MAX_WAIT_TIME_FOR_SENDING, AT_WAIT_SEND_OK))
        {
            GSM_LOG_P0("----Send failed, power down GSM!----\r\n");
            GsmPowerUpDownOpt(GSM_POWER_DOWN);
            return false;
        }
    }
  
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
    /* 可以不判断是否连接，直接发送，如果收到回应就是连接上，否则重发 */
    if (GSM_TCP_CONNECTED == GsmStatusGet())
        return GsmSendData(str, len, respflag);
      
    if (!IsTcpConnected())
    {
        
        GSM_LOG_P2("%s %d: Send data, but error status! Try to Connect!",
                    __FUNCTION__, __LINE__);
        if (!GsmStartAndconect())
        {
            /* 连接后,发送数据,根据是否有响应判断是否真正连接上 */
            if (false == GsmSendData(str, len, respflag))
            {
                resetCtrl++;
                if (resetCtrl == RESET_CTRL_MAX_TIMES)
                {
                    resetCtrl = 0;
                    HAL_NVIC_SystemReset();
                }
                GSM_LOG_E_P2("%s %d: Send data in error stu, and startup fail!",
                        __FUNCTION__, __LINE__);
                return false;
            }
        }
        
        resetCtrl = 0;
    }
    
    return GsmSendData(str, len, respflag);
}






        
