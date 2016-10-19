#include "dataHandler.h"
#include "moduleStatus.h"
#include "crc.h"
#include "gpsAnalyser.h"
#include "dataRecord.h"
#include "gps.h"
#include "config.h"
#include "common.h"

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
/* 夏普传感器获得的PM10 */
PM_t g_pm10sharp;
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

/* 用于记录数据到TF卡 */
SEND_BUF_t g_recordBuf;

/* 拷贝到缓冲区，供写入到文件中 */

/* 用于接收数据回应的缓冲区，避免block接收 */
SEND_BUF_t g_sendResponse;

/* 用于构造直接发送的数据 */
SEND_BUF_t g_sendDirt;

/* 回调应答的数据缓冲 */
SEND_BIG_BUF_t g_sendRecallData;

extern RECORD_INFO_t g_recordInfo;


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

double GetNcoValue(float temp)
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

double GetNoxValue(float temp)
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
*  return : -1 温度异常  正常单位  ppb 转换后的 ug/m3
*/
s32 CalGasVal(u32 Vw, u32 Va, EASType gasType)
{
    double n = 0;
    double rst;
    s16 Vw0;
    s16 Va0;
    s16 S;
    float temp = 0;
    s16 mWght = 0;   // 记录分子量
    
    if (Vw > 6000 || Va > 6000)
    {
        return -1;
    }
    
    temp = GetTempIn()/10.0;
    if (temp < -30 || temp > 50)
    {
        return -1;
    }
    
    switch (gasType)
    {
    case AS_TYPE_CO:
      GetCoZero(&Vw0, &Va0, &S);
      n = GetNcoValue(temp);
      mWght = 28;
      AFX_LOG_P3("CalGasVal: CO  W: %d, A:%d, N:%lf", Vw, Va, n);
      break;
    case AS_TYPE_SO2:
      GetSo2Zero(&Vw0, &Va0, &S);
      n = GetNso2Value();
      mWght = 64;
      AFX_LOG_P3("CalGasVal: SO2  W: %d, A:%d, N:%lf", Vw, Va, n);
      break;
    case AS_TYPE_NO2:
      GetNo2Zero(&Vw0, &Va0, &S);
      n = GetNno2Value();
      mWght = 46;
      AFX_LOG_P3("CalGasVal: NO2  W: %d, A:%d, N:%lf", Vw, Va, n);
      break;
    case AS_TYPE_O3:
      GetO3Zero(&Vw0, &Va0, &S);
      n = GetNoxValue(temp);
      mWght = 48;
      AFX_LOG_P3("CalGasVal: O3  W: %d, A:%d, N:%lf", Vw, Va, n);
      break;
    default:
      AFX_LOG_P1("Warning: Error gas type: %d", gasType);
      break;
    }
    
    rst = ((Vw - Vw0) - n * (Va - Va0))/ S;
    
    /* PPM to PPB */
    rst = rst * 1000.0;
    
    // 质量浓度mg/m3=M气体分子量/22.4*ppm数值*[273/(273+T气体温度)]*（Ba压力/101325）
    rst = (mWght/22.4) * rst * (273.0/(273+temp)) * 1; 
    
    AFX_LOG_P4("CalGasVal: Vw0: %d, Va0: %d, S: %d, rst: %lf", 
               Vw0, Va0, S, rst);
    
    if (rst < 0.0)
      rst = 0;
    
    return (s32)(rst+0.5);
}

/* 存储气体浓度 */
s8 StoreGasInfo(u32 val, GAS_t *bag)
{
    /* 数据有效性在外部判断 */
    AFX_LOG_P1("Store Gas value: %d", val);
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

u16 GetPm10Sharp()
{
    return g_pm10sharp.curData;
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
    memcpy(SEND_BUF_OFFSET(offset), CMD_CLI_REPORT_DATA, 3);
    
    offset = LEN_HEAD + LEN_ADDR + LEN_CMD;
    
    /* date*/
    if (IsClockSynced())
    {
        RTC_GetCalendar((struct tm *)&(gps.time));
        sprintf(gps.utc.strTime + 2, "%02d%02d%02d%02d%02d%02d", 
                gps.time.tm_year - 2000, gps.time.tm_mon, gps.time.tm_mday, 
                gps.time.tm_hour, gps.time.tm_min, gps.time.tm_sec);
    }
    
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
        offset += Format16(GetPm10Sharp(), SEND_BUF_OFFSET(offset));
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
    offset += 2;
    
    /* 数据总长度 */
    SEND_BUF_SET_LEN(offset);

    GSM_LOG_P1("Date: %s\r\n", gps.utc.strTime);
    GSM_LOG_P4("Long:Lati %d : %d, TmpO:WetO %d:%d", 
                 GetCoordLong(), GetCoordLati(), GetTempOut(), GetWetOut());
    GSM_LOG_P4("TmpI:WetI %d:%d, pm25:%d, pm10Sharp:%d", 
                 GetTempIn(), GetWetIn(), GetPm25(), GetPm10Sharp());
    GSM_LOG_P1("pm10:%d", GetPm10());
    GSM_LOG_P4("CO:%d, SO2:%d, O3:%d, No2: %d", 
                 GetCo(), GetSo2(), GetO3(), GetNo2());
    
    SEND_BUF_FLAG_SET();
    
    SEND_BUF_RESP_FALG_SET(1);
    
    /* 准备记录到TF卡 */
    /* 需要判断时间是否同步 */
    if (g_recordBuf.sendFlag == 0)
    {
        /*
        memcpy(g_recordBuf.buf, SEND_BUF_OFFSET(LEN_HEAD), offset - LEN_HEAD);
        g_recordBuf.useLen = offset - LEN_HEAD;
        sprintf(g_recordInfo.newNameMon, "%02d%02d", g_sdate.Year, g_sdate.Month);
        sprintf(g_recordInfo.newNameDay, "%02d%02d\\%02d", g_sdate.Year, g_sdate.Month,
                g_sdate.Date);
        sprintf(g_recordInfo.newNameHour, "%02d%02d\\%02d\\%02d.txt",
                g_sdate.Year, g_sdate.Month, g_sdate.Date, g_stime.Hours);

        g_recordBuf.sendFlag == 1;
        
        GSM_LOG_P3("Record data ready! \r\n%s\r\n%s\r\n%s", 
                   g_recordInfo.newNameMon, 
                   g_recordInfo.newNameDay, 
                   g_recordInfo.newNameHour);
      */
        //  20160801161616
        memcpy(g_recordBuf.buf, SEND_BUF_OFFSET(LEN_HEAD + LEN_ADDR_CMD), offset - LEN_HEAD - LEN_ADDR_CMD);
        g_recordBuf.useLen = offset - LEN_HEAD - LEN_ADDR_CMD;
        
        // 2016
        memcpy(g_recordInfo.newNameYear, gps.utc.strTime, 4);
        
        // 2016/08
        memcpy(g_recordInfo.newNameMon, g_recordInfo.newNameYear, 4);
        g_recordInfo.newNameMon[4] = '\\';
        memcpy(g_recordInfo.newNameMon + 5, gps.utc.strTime+4, 2);
        
        // 2016/08/01
        memcpy(g_recordInfo.newNameDay, g_recordInfo.newNameMon, 7);
        g_recordInfo.newNameDay[7] = '\\';
        memcpy(g_recordInfo.newNameDay + 8, gps.utc.strTime+6, 2);
        
        // 2016/08/01/16.txt
        memcpy(g_recordInfo.newNameHour, g_recordInfo.newNameDay, 10);
        g_recordInfo.newNameHour[10] = '\\';
        memcpy(g_recordInfo.newNameHour + 11, gps.utc.strTime+8, 2);
        g_recordInfo.newNameHour[13] = '.';
        g_recordInfo.newNameHour[14] = 't';
        g_recordInfo.newNameHour[15] = 'x';
        g_recordInfo.newNameHour[16] = 't';

        g_recordBuf.sendFlag = 1;
        
        GSM_LOG_P3("Record data ready! \r\n%s\r\n%s\r\n%s", 
                   g_recordInfo.newNameMon, 
                   g_recordInfo.newNameDay, 
                   g_recordInfo.newNameHour);
    }

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
*  onoff: 1: 开始     2 : 结束
*/
bool ConstructRecordDataToSend(u8 *data, u8 onoff, u16 cnt)
{
    u16 offset = 0;
    u32 crc = 0;
    u8 *buf = NULL;
    
    g_sendRecallData.sendFlag = 0;
    
    if (onoff == 1)
    {
        g_sendRecallData.useLen = LEN_HEAD + LEN_ADDR + LEN_CMD + 2;
        return true;
    }
    
    /* 代表结束 */
    if (onoff == 2)
    {
        u8 num[3] = {0};
        
        buf = g_sendRecallData.buf;
        /* 写入地址 */
        offset = LEN_HEAD;
        memcpy(buf+offset, ConfigGetStrAddr(), MAX_ADDR_LEN);
        offset += MAX_ADDR_LEN;
        /* cmd */
        memcpy(buf+offset, CMD_CLI_RECALL_DATA, 3);
        offset += 3;
        
        sprintf(num, "%02d", cnt);
        memcpy(buf+offset, num, 2);
        
        /* 计算CRC */
        crc = usMBCRC16( buf + LEN_HEAD , g_sendRecallData.useLen - LEN_HEAD );
        
        /* 赋值头部 */
        FormatHead(crc, offset - LEN_HEAD, buf);
        
        return true;
    }
   
    if (MAX_SEND_BIG_BUFF_LEN - g_sendRecallData.useLen < LEN_REPORT_DATA_ONLY)
    {
       GSM_LOG_P1("Send buffer is full!", g_sendRecallData.useLen);
       return false;
    }
    
    buf = g_sendRecallData.buf + g_sendRecallData.useLen;
    
    memcpy(buf, data, LEN_REPORT_DATA_ONLY);
    g_sendRecallData.useLen += LEN_REPORT_DATA_ONLY;
    
    return true;

}

void InitSendRecallData()
{
    g_sendRecallData.sendFlag = 0;
    g_sendRecallData.useLen = 0;
}

/*
typedef struct SEND_BUF_t
{
    u16 sendFlag;         // 数据是否准备好的标识
    u16 respFlag;         // 这条消息是否需要回应
    u16 useLen;
    u8 buf[MAX_SEND_BUFF_LEN];
    
}SEND_BUF_t;
*/
/* 在发送线程中使用的函数，直接构造数据并发送 */
void ConstructDataAndSend(u8 *cmd, u8* addr, u8 *opt, u8 optLen)
{
    u16 offset = 0;
    u32 crc = 0;
    u8 *buf = g_sendDirt.buf;
    
    offset = LEN_HEAD;
    memcpy((s8 *)(buf + offset), addr, MAX_ADDR_LEN);
    offset += MAX_ADDR_LEN;
    /* cmd */
    memcpy((s8 *)(buf + offset), cmd, LEN_CMD);
    offset += LEN_CMD;
    
    /* opt */
    memcpy((s8 *)(s8 *)(buf + offset), opt, optLen);
    offset += optLen;
    
    /* HEAD */
    /* CRC */
    crc = usMBCRC16( (u8 *)(buf + LEN_HEAD) , offset - LEN_HEAD );
    
    FormatHead(crc, offset - LEN_HEAD, (u8 *)buf);
    
    g_sendDirt.useLen = offset;
}






