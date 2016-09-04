#include "dataHandler.h"
#include "moduleStatus.h"
#include "crc.h"
#include "gpsAnalyser.h"

/* 站号 */
/* 出厂唯一编号 */
/* 箱内温度湿度 */
TEMP_WET_t g_tempWetIn;
/* 箱外温度湿度 */
TEMP_WET_t g_tempWetOut;
/* PM2.5 */
PM_t g_pm25;
/* PM10 */
PM_t g_pm10;
/* CO */
GAS_t g_co;
/* NO2 */
GAS_t g_no2;
/* SO2 */
GAS_t g_so2;
/* O3 */
GAS_t g_o3;
/* 坐标 */
COORD_t g_coord;
/* 风的信息 */
WIND_t g_wind;

/* 用于发送的头部 */
MSG_HEAD_t g_head;

/* 用于发送缓冲 */
SEND_BUF_t g_sendBuf;

/* 用于接收数据回应的缓冲区，避免block接收 */
SEND_BUF_t g_sendResponse;

/* 回调应答的数据缓冲 */
SEND_BIG_BUF_t g_sendRecallData;




/* 用于测试填充数据 */
void testFillData(void)
{
    /* 存储温度湿度 */
    StoreWetTempInfo(90, 90, &g_tempWetIn);
    StoreWetTempInfo(80, 80, &g_tempWetOut);
    /* 存储气体浓度 */
    StoreGasInfo(100, &g_co);
    StoreGasInfo(200, &g_no2);
    StoreGasInfo(300, &g_so2);
    StoreGasInfo(400, &g_o3);
    /* 存储颗粒物浓度 */
    StorePmInfo(100, &g_pm25);
    StorePmInfo(200, &g_pm10);
}


/* 存储温度湿度 */
s8 StoreWetTempInfo(u16 wet, s16 temp, TEMP_WET_t *bag)
{
    if (!IsValidWet(wet) || !IsValidTemp(temp))
    {
        return NOK_1;
    }

    /* 达到采样个数才开始计算 */
    if (!IsReachSamples(bag->validNum, SAMPLE_TEMP_WET_NUM))
    {
        bag->validNum++;
        bag->totalTemp += temp;
        bag->dataTemp[bag->curPos] = temp;
        bag->curTemp = temp;
			
        bag->totalWet += wet;
        bag->dataWet[bag->curPos] = wet;
        bag->curWet = wet;
			
        MV_NEXT_POS(bag->curPos, SAMPLE_TEMP_WET_NUM);

        return NOK_2;
    }

    /* 更新总数 */
    bag->totalTemp = bag->totalTemp - bag->dataTemp[bag->curPos] + temp;
    /* 计算平均值 */
    bag->curTemp = bag->totalTemp / bag->validNum;
    /* 存储采样值 */
    bag->dataTemp[bag->curPos] = temp;
		
    /* 更新总数 */
    bag->totalWet = bag->totalWet - bag->dataWet[bag->curPos] + wet;
    /* 计算平均值 */
    bag->curWet = bag->totalWet / bag->validNum;
    /* 存储采样值 */
    bag->dataWet[bag->curPos] = wet;
		
		
    /* 更新最老的数据值位置 */
    MV_NEXT_POS(bag->curPos, SAMPLE_TEMP_WET_NUM);

    return OK;

}

/* 计算气体浓度的接口 */
/*
*  获得系数的接口
*/
double GetNso2Value(void)
{
    return 1.00;
}

double GetNno2Value(void)
{
    return 0.7;
}

double GetNcoValue(double temp)
{
    double n = 0;
    double v1 = 0;
    double v2 = 0;
    double c = 0;
    
    if (-30 <= temp && temp <= 0)
    {
        n = 1.20;
        return n;
    }
    else if (0 <= temp && temp <= 10)
    {
        v1 = 1.20;
        v2 = 1.05;
        c = temp - 0;
    }
    else if (10 < temp && temp <= 20)
    {
        v1 = 1.05;
        v2 = -0.32;
        c = temp - 10;
    }
    else if (20 < temp && temp <= 30)
    {
        v1 = -0.32;
        v2 = -1.07;
        c = temp - 20;
    }
    else if (30 < temp && temp <= 40)
    {
        v1 = -1.07;
        v2 = -3.0;
        c = temp - 30;
    }
    else if (40 < temp && temp <= 50)
    {
        v1 = -3.0;
        v2 = -2.4;
        c = temp - 40;
    }
    else
    {
        return 0;   /* 温度超出范围 */
    }
    
    n = (v1 - v2) * c / 10 + v2;
    
    return n;
}

double GetNoxValue(double temp)
{
    double n = 0;
    double v1 = 0;
    double v2 = 0;
    double c = 0;
    
    if (-30 <= temp && temp <= 10)
    {
        n = -0.9;
        return n;
    }
    else if (10 < temp && temp <= 20)
    {
        v1 = -0.9;
        v2 = -0.35;
        c = temp - 10;
    } 
    else if (20 < temp && temp <= 30)
    {
        v1 = -0.35;
        v2 = 0.5;
        c = temp - 20;
    }
    else if (30 < temp && temp <= 40)
    {
        v1 = 0.5;
        v2 = 1.15;
        c = temp - 30;
    }
    else if (40 < temp && temp <= 50)
    {
        v1 = 1.15;
        v2 = 1.80;
        c = temp - 40;
    }
    else
    {
        return 0;
    }
    
    n = (v1 - v2) * c / 10 + v2;
    
    return n;
}
/*
*  Vw : 工作电极     Va ：辅助电极
*  gasType : 0: CO   1: SO2   2: NO2    3: O3
*  return : -1 温度异常  正常单位  ppb
*/
s32 CalGasVal(u32 Vw, u32 Va, u8 gasType)
{
    double n = 0;
    double rst;
    s16 Vw0;
    s16 Va0;
    s16 S;
    double temp = 0;
    
    temp = GetTempIn()/10.0;
    if (temp < -30 || temp > 50)
    {
        return -1;
    }
    
    switch (gasType)
    {
    case 0:
      GetCoZero(&Vw0, &Va0, &S);
      n = GetNcoValue(temp);
      break;
    case 1:
      GetSo2Zero(&Vw0, &Va0, &S);
      n = GetNso2Value();
      break;
    case 2:
      GetNo2Zero(&Vw0, &Va0, &S);
      n = GetNno2Value();
      break;
    case 3:
      GetO3Zero(&Vw0, &Va0, &S);
      n = GetNoxValue(temp);
      break;
    }
    
    rst = ((Vw - Vw0) - n * (Va - Va0))/ S;
    
    /* PPM to PPB */
    rst = rst * 1000;
    
    return rst;
}

/* 存储气体浓度 */
s8 StoreGasInfo(u32 val, GAS_t *bag)
{
    /* 数据有效性在外部判断 */

    /* 达到采样个数才开始计算 */
    if (!IsReachSamples(bag->validNum, SAMPLE_GAS_NUM))
    {
        bag->validNum++;
        bag->total = bag->total + val;
        bag->data[bag->curPos] = val;
        bag->curData = val;
        MV_NEXT_POS(bag->curPos, SAMPLE_GAS_NUM);

        return NOK_2;
    }

    /* 更新总数 */
    bag->total = bag->total - bag->data[bag->curPos] + val;
    /* 计算平均值 */
    bag->curData = bag->total / bag->validNum;
    /* 存储采样值 */
    bag->data[bag->curPos] = val;
    /* 更新最老的数据值位置 */
    MV_NEXT_POS(bag->curPos, SAMPLE_GAS_NUM);

    return OK;

}

/* 存储颗粒物浓度 */
s8 StorePmInfo(u16 val, PM_t *bag)
{
    /* 数据有效性在外部判断 */

    /* 达到采样个数才开始计算 */
    if (!IsReachSamples(bag->validNum, SAMPLE_PM_NUM))
    {
        bag->validNum++;
        bag->total = bag->total + val;
        bag->data[bag->curPos] = val;
        bag->curData = val;
        MV_NEXT_POS(bag->curPos, SAMPLE_PM_NUM);

        return NOK_2;
    }

    /* 更新总数 */
    bag->total = bag->total - bag->data[bag->curPos] + val;
    /* 计算平均值 */
    bag->curData = bag->total / bag->validNum;
    /* 存储采样值 */
    bag->data[bag->curPos] = val;
    /* 更新最老的数据值位置 */
    MV_NEXT_POS(bag->curPos, SAMPLE_PM_NUM);

    return OK;

}

/* 存储定位信息 */
s8 StoreCoordInfo(unsigned long int longVal, unsigned long int latVal, COORD_t *bag)
{
    /* 达到采样个数才开始计算 */
    if (!IsReachSamples(bag->validNum, SAPMLE_COORD_NUM))
    {
        bag->validNum++;
        bag->totalLong = bag->totalLong + longVal;
        bag->dataLong[bag->curPos] = longVal;
        bag->curLong = longVal;

        bag->totalLati = bag->totalLati + latVal;
        bag->dataLati[bag->curPos] = latVal;
        bag->curLati = latVal;

        MV_NEXT_POS(bag->curPos, SAPMLE_COORD_NUM);

        return NOK_2;
    }

    /* 更新总数 */
    bag->totalLong = bag->totalLong - bag->dataLong[bag->curPos] + longVal;
    /* 计算平均值 */
    bag->curLong = bag->totalLong / bag->validNum;
    /* 存储采样值 */
    bag->dataLong[bag->curPos] = longVal;

    /* 更新总数 */
    bag->totalLati = bag->totalLati - bag->dataLati[bag->curPos] + latVal;
    /* 计算平均值 */
    bag->curLati = bag->totalLati / bag->validNum;
    /* 存储采样值 */
    bag->dataLati[bag->curPos] = latVal;

    /* 更新最老的数据值位置 */
    MV_NEXT_POS(bag->curPos, SAPMLE_COORD_NUM);

    return OK;

}

/* 存储风信息 */
s8 StoreWindInfo(u8 speedVal, u8 dirVal)
{
    return 0;
}

/*
*
*  获取各采样值的接口
*
*/
/* 获取箱外温度 */
s16 GetTempOut()
{
    return g_tempWetOut.curTemp;
}

/* 获取箱外湿度 */
u16 GetWetOut()
{
    return g_tempWetOut.curWet;
}

/* 获取箱内温度 */
s16 GetTempIn()
{
    return g_tempWetIn.curTemp;
}

/* 获取箱内湿度 */
u16 GetWetIn()
{
    return g_tempWetIn.curWet;
}

/* PM2.5 */
u16 GetPm25()
{
    return g_pm25.curData;
}

/* PM10 */
u16 GetPm10()
{
    return g_pm10.curData;
}

/* CO */
u32 GetCo()
{
    return g_co.curData;
}

/* NO2 */
u32 GetNo2()
{
    return g_no2.curData;
}

/* SO2 */
u32 GetSo2()
{
    return g_so2.curData;
}

/* O3 */
u32 GetO3()
{
    return g_o3.curData;
}

/* 坐标经度 */
u32 GetCoordLong()
{
    return g_coord.curLong;
}

/* 坐标纬度 */
u32 GetCoordLati()
{
    return g_coord.curLati;
}

/* 风的信息 */
WIND_t g_wind;

/* 用于计算和保存信息 */
extern gps_process_data gps;

/* 构造发送的数据 */
void ContructDataUp()
{
    STATUS_e ret = STU_ERROR;
    u16 offset = 0;
    u32 crc = 0;
    
    SEND_BUF_FLAG_CLEAR();
    
    /* 写入地址 */
    offset = LEN_HEAD;
    memcpy(SEND_BUF_OFFSET(offset), ConfigGetStrAddr(), MAX_ADDR_LEN);
    offset += MAX_ADDR_LEN;
    /* cmd */
    memcpy(SEND_BUF_OFFSET(offset), "503", 3);
    
    offset = LEN_HEAD + LEN_ADDR + LEN_CMD;
    
    /* date*/
    offset += FormatTime(&gps.utc.strTime[2], SEND_BUF_OFFSET(offset));
    
    /* 写入模块状态 */ 
    offset += Format32(GetAllModuleStu(), SEND_BUF_OFFSET(offset));

    /* GPS 模块 */
    /*
    ret = GetModuleStu(MDU_GPS);
    if (STU_NORMAL == ret)
    {
        offset += Format32(GetCoordLong(), SEND_BUF_OFFSET(offset));
        offset += Format32(GetCoordLati(), SEND_BUF_OFFSET(offset));  
    }
    */
    
    
    ret = GetModuleStu(MDU_IN_TEMP_WET);
    if (STU_NORMAL == ret)
    {
        offset += Format16(GetTempIn(), SEND_BUF_OFFSET(offset));
        offset += Format16(GetWetIn(), SEND_BUF_OFFSET(offset));
    }
    else
    {
        offset += Format16(0, SEND_BUF_OFFSET(offset));
        offset += Format16(0, SEND_BUF_OFFSET(offset));
    }
    
    ret = GetModuleStu(MDU_OUT_TEMP_WET);
    if (STU_NORMAL == ret)
    {
        offset += Format16(GetTempOut(), SEND_BUF_OFFSET(offset));
        offset += Format16(GetWetOut(), SEND_BUF_OFFSET(offset));
    }
    else
    {
        offset += Format16(0, SEND_BUF_OFFSET(offset));
        offset += Format16(0, SEND_BUF_OFFSET(offset));
    }
    
    ret = GetModuleStu(MDU_PM25);
    if (STU_NORMAL == ret)
    {
        offset += Format16(GetPm25(), SEND_BUF_OFFSET(offset));
    }
    else
    {
        offset += Format16(0, SEND_BUF_OFFSET(offset));
    }
    
    ret = GetModuleStu(MDU_PM10_SHARP);
    if (STU_NORMAL == ret)
    {
        offset += Format16(GetPm10(), SEND_BUF_OFFSET(offset));
    }
    else
    {
        offset += Format16(0, SEND_BUF_OFFSET(offset));
    }
    
    ret = GetModuleStu(MDU_CO);
    if (STU_NORMAL == ret)
    {
        offset += Format16(GetCo(), SEND_BUF_OFFSET(offset));
    }
    else
    {
        offset += Format16(0, SEND_BUF_OFFSET(offset));
    }
    
    ret = GetModuleStu(MDU_SO2);
    if (STU_NORMAL == ret)
    {
        offset += Format16(GetSo2(), SEND_BUF_OFFSET(offset));
    }
    else
    {
        offset += Format16(0, SEND_BUF_OFFSET(offset));
    }
    
    ret = GetModuleStu(MDU_NO2);
    if (STU_NORMAL == ret)
    {
        offset += Format16(GetNo2(), SEND_BUF_OFFSET(offset));
    }
    else
    {
        offset += Format16(0, SEND_BUF_OFFSET(offset));
    }
    
    ret = GetModuleStu(MDU_O3);
    if (STU_NORMAL == ret)
    {
        offset += Format16(GetO3(), SEND_BUF_OFFSET(offset));
    }
    else
    {
        offset += Format16(0, SEND_BUF_OFFSET(offset));
    }
    
    /* 电压 */
    offset += Format16(120, SEND_BUF_OFFSET(offset));
    
    /* 计算CRC */
    crc = usMBCRC16( SEND_BUF_OFFSET(LEN_HEAD) , offset - LEN_HEAD );
    
    //GSM_LOG_P2("CRC####: %d : %d\r\n", crc, offset - LEN_HEAD);
    
    /* 赋值头部 */
    FormatHead(crc, offset - LEN_HEAD, SEND_BUF_OFFSET(0));
    
    SEND_BUF_SET_BYTE('\r', offset);
    SEND_BUF_SET_BYTE('\n', offset + 1);
    
    /* 数据总长度 */
    SEND_BUF_SET_LEN(offset + 2);

    GSM_LOG_P1("Date: %s\r\n", gps.utc.strTime);
    GSM_LOG_P3("YYMMSS: %04d%02d%02d\r\n", gps.utc.year, gps.utc.month, gps.utc.date);
    GSM_LOG_P3("HHMMSS: %02d%02d%02d\r\n", gps.utc.hour, gps.utc.min, gps.utc.sec);
    GSM_LOG_P4("Long:Lati %d : %d, TmpO:WetO %d:%d", 
                 GetCoordLong(), GetCoordLati(), GetTempOut(), GetWetOut());
    GSM_LOG_P4("TmpI:WetI %d:%d, pm25:%d, pm10:%d", 
                 GetTempIn(), GetWetIn(), GetPm25(), GetPm10());
    GSM_LOG_P4("CO:%d, SO2:%d, O3:%d, No2: %d", 
                 GetCo(), GetSo2(), GetO3(), GetNo2());
    
    SEND_BUF_FLAG_SET();
    
    SEND_BUF_RESP_FALG_SET(1);

}

/* 构造头部 */
void ContstructHead(u32 crc, u16 dataLen)
{
    g_head.crc = crc;
    g_head.dataLen = dataLen;
}

/*
*  构造记录到文件中的数据
*/
u8 ConstructRecordData(u8 *data)
{
    STATUS_e ret = STU_ERROR;
    u16 offset = 0;
    
    SEND_BUF_FLAG_CLEAR();
    
    /* date*/
    offset += FormatTime(&gps.utc.strTime[2], data+offset);
    
    /* 写入模块状态 */ 
    offset += Format32(GetAllModuleStu(), data+offset);
    
    ret = GetModuleStu(MDU_IN_TEMP_WET);
    if (STU_NORMAL == ret)
    {
        offset += Format16(GetTempIn(), data+offset);
        offset += Format16(GetWetIn(), data+offset);
    }
    else
    {
        offset += Format16(0, data+offset);
        offset += Format16(0, data+offset);
    }
    
    ret = GetModuleStu(MDU_OUT_TEMP_WET);
    if (STU_NORMAL == ret)
    {
        offset += Format16(GetTempOut(), data+offset);
        offset += Format16(GetWetOut(), data+offset);
    }
    else
    {
        offset += Format16(0, data+offset);
        offset += Format16(0, data+offset);
    }
    
    ret = GetModuleStu(MDU_PM25);
    if (STU_NORMAL == ret)
    {
        offset += Format16(GetPm25(), data+offset);
    }
    else
    {
        offset += Format16(0, data+offset);
    }
    
    ret = GetModuleStu(MDU_PM10_SHARP);
    if (STU_NORMAL == ret)
    {
        offset += Format16(GetPm10(), data+offset);
    }
    else
    {
        offset += Format16(0, data+offset);
    }
    
    ret = GetModuleStu(MDU_CO);
    if (STU_NORMAL == ret)
    {
        offset += Format16(GetCo(), data+offset);
    }
    else
    {
        offset += Format16(0, data+offset);
    }
    
    ret = GetModuleStu(MDU_SO2);
    if (STU_NORMAL == ret)
    {
        offset += Format16(GetSo2(), data+offset);
    }
    else
    {
        offset += Format16(0, data+offset);
    }
    
    ret = GetModuleStu(MDU_NO2);
    if (STU_NORMAL == ret)
    {
        offset += Format16(GetNo2(), data+offset);
    }
    else
    {
        offset += Format16(0, data+offset);
    }
    
    ret = GetModuleStu(MDU_O3);
    if (STU_NORMAL == ret)
    {
        offset += Format16(GetO3(), data+offset);
    }
    else
    {
        offset += Format16(0, data+offset);
    }
    
    /* 电压 */
    offset += Format16(120, data+offset);
    
    data[offset] = '\r';
    data[offset + 1] = '\n';
/*
    GSM_LOG_P3("YYMMSS: %04d%02d%02d\r\n", gps.utc.year, gps.utc.month, gps.utc.date);
    GSM_LOG_P3("HHMMSS: %02d%02d%02d\r\n", gps.utc.hour, gps.utc.min, gps.utc.sec);
    GSM_LOG_P4("TmpI:WetI %d:%d, pm25:%d, pm10:%d",
                 GetTempIn(), GetWetIn(), GetPm25(), GetPm10());
    GSM_LOG_P4("CO:%d, SO2:%d, O3:%d, No2: %d", 
                 GetCo(), GetSo2(), GetO3(), GetNo2());
    */
    return (offset + 2);
}




/*
*  构造回调数据
*/
u8 ConstructRecordDataToSend(u8 *data, u8 *cmd)
{
    STATUS_e ret = STU_ERROR;
    u16 offset = 0;
    u32 crc = 0;
    u8 *buf = g_sendRecallData.buf + g_sendRecallData.useLen;
    
    g_sendRecallData.sendFlag = 0;
   
    if (MAX_SEND_BIG_BUFF_LEN - g_sendRecallData.useLen < LEN_REPORT_DATA + 2)
    {
       GSM_LOG_P1("Send buffer is full!", g_sendRecallData.useLen);
       return 0;
    }
    
    /* 写入地址 */
    offset = LEN_HEAD;
    memcpy(buf+offset, ConfigGetStrAddr(), MAX_ADDR_LEN);
    offset += MAX_ADDR_LEN;
    /* cmd */
    memcpy(buf+offset, cmd, 3);
    
    offset = LEN_HEAD + LEN_ADDR + LEN_CMD;
    
    memcpy(buf+offset, data, LEN_REPORT_DATA_WO_HEAD);
    offset = LEN_REPORT_DATA;
    
    
    /* 计算CRC */
    crc = usMBCRC16( buf + LEN_HEAD , offset - LEN_HEAD );
    
    /* 赋值头部 */
    FormatHead(crc, offset - LEN_HEAD, buf);
    
    buf[offset] = '\r';
    buf[offset + 1] = '\n';
    
    g_sendRecallData.useLen += offset + 2;
    
    return offset + 2;

}

void InitSendRecallData()
{
    g_sendRecallData.sendFlag = 0;
    g_sendRecallData.useLen = 0;
}





