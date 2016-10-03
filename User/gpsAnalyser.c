#include "gpsAnalyser.h"
#include "string.h"
#include "gps.h"
#include "common.h"
//��ֹ������ҵ��Ϊ

/********************************************************************************************
GPS���ݽ���
********************************************************************************************/
void GPS_Analysis(gps_process_data *gps_data,unsigned char *buf)
{
    if((strstr((const char *)buf,"$GPGSV")))//GPGSV����
    {
        GPGSV_Analysis(gps_data,buf);
        return;
    }

    if((strstr((const char *)buf,"$GPGGA")))//GPGGA����
    {
        GPGGA_Analysis(gps_data,buf);
        return;
    }
    
    if((strstr((const char *)buf,"$GPRMC")))//GPRMC����
    {
        GPRMC_Analysis(gps_data,buf);
        return;
    }
    
    if((strstr((const char *)buf,"$GPGSA")))//GPGSA����	
    {
        GPGSA_Analysis(gps_data,buf);
        return;
    }
}

/********************************************************************************************
����GPGSV����
********************************************************************************************/
void GPGSV_Analysis(gps_process_data *gps_data,unsigned char *buf)
{
    unsigned char *temp_buf,*address_buf,end_buf,decimal_places;  	 
    temp_buf=buf;
    address_buf=(unsigned char*)strstr((const char *)temp_buf,"$GPGSV");
    end_buf=Data_Removal(address_buf,3);									 										//����ȥ��
    if(end_buf!=0XFF)
        gps_data->svnum=Data_Extraction(address_buf+end_buf,&decimal_places);		//�õ���֪��������
}

/********************************************************************************************
����GPGGA����
********************************************************************************************/
void GPGGA_Analysis(gps_process_data *gps_data,unsigned char *buf)
{
    unsigned char *address_buf,end_buf,decimal_places;	
    address_buf =(unsigned char*)strstr((const char *)buf,"$GPGGA");		
          
    end_buf=Data_Removal(address_buf,6);																			//����ȥ��				
    if(end_buf!=0XFF)
        gps_data->gpssta=Data_Extraction(address_buf+end_buf,&decimal_places);	//�õ�GPS״̬
          
    end_buf=Data_Removal(address_buf,7);																			//����ȥ��		
    if(end_buf!=0XFF)
        gps_data->posslnum=Data_Extraction(address_buf+end_buf,&decimal_places);//�õ����ڶ�λ��������
}

/********************************************************************************************
����GPRMC����
********************************************************************************************/
void GPRMC_Analysis(gps_process_data *gps_data,unsigned char *buf)
{
    static unsigned long syncCnt = 0;    // 计数同步时间，一天同步一次 8640 次 做一次同步
    
    unsigned char *address_buf,end_buf,decimal_places;		 
    unsigned long temp;
    float rs;  
    unsigned char flag = 0;
    address_buf =(unsigned char*)strstr((const char *)buf,"GPRMC");

    /* ��ȡUTCʱ�� */
    end_buf = Data_Removal(address_buf, 1);
    if (end_buf < 6)
    {
        return;
    }
    
    syncCnt++;
    if (8640 <= syncCnt)
    {
        SetClockSynced(0);
        syncCnt = 0;
    }
    
    // 加次数控制，一定次数后，同步时间
    if (end_buf != 0xFF && address_buf[end_buf] != ',' && 2 != IsClockSynced())       /* ��Ҫȷ����Ч���ݵ����� */
    {
        unsigned char offset = end_buf;
        
        gps_data->utc.hour = (address_buf[offset] - '0') * 10 + address_buf[offset+1] - '0';
        gps_data->utc.strTime[8] = address_buf[offset];
        gps_data->utc.strTime[9] = address_buf[offset + 1];
        offset += 2;
        gps_data->utc.min = (address_buf[offset] - '0') * 10 + address_buf[offset+1] - '0';
        gps_data->utc.strTime[10] = address_buf[offset];
        gps_data->utc.strTime[11] = address_buf[offset + 1];
        offset += 2;
        gps_data->utc.sec = (address_buf[offset] - '0') * 10 + address_buf[offset+1] - '0';
        gps_data->utc.strTime[12] = address_buf[offset];
        gps_data->utc.strTime[13] = address_buf[offset + 1];
        flag = 1;
        
    }

    end_buf=Data_Removal(address_buf,3);																			//����ȥ��								
    if(end_buf!=0XFF)
    {
        temp=Data_Extraction(address_buf+end_buf,&decimal_places);		 	 
        gps_data->latitude=temp/MN_Process(10,decimal_places+2);
        rs=temp%MN_Process(10,decimal_places+2);
        gps_data->latitude=(unsigned long int)(gps_data->latitude*MN_Process(10,5)+(rs*MN_Process(10,5-decimal_places))/60);//�õ�γ��
    }


    end_buf=Data_Removal(address_buf,4);																			//����ȥ��								
    if(end_buf!=0XFF)
        gps_data->nshemi=*(address_buf+end_buf);																//��γ���Ǳ�γ 			


    end_buf=Data_Removal(address_buf,5);																			//����ȥ��									
    if(end_buf!=0XFF)
    {												  
        temp=Data_Extraction(address_buf+end_buf,&decimal_places);		 	 
        gps_data->longitude=temp/MN_Process(10,decimal_places+2);
        rs=temp%MN_Process(10,decimal_places+2); 
        gps_data->longitude=(unsigned long int)(gps_data->longitude*MN_Process(10,5)+(rs*MN_Process(10,5-decimal_places))/60);//�õ�����
    }

    end_buf=Data_Removal(address_buf,6);																			//����ȥ��
    if(end_buf!=0XFF)
        gps_data->ewhemi=*(address_buf+end_buf);																//������������		 
    
    end_buf=Data_Removal(address_buf, 9);
    if (end_buf < 6)
    {
        return;
    }
    if (end_buf != 0xFF && address_buf[end_buf] != ',' && 2 != IsClockSynced())       /* ��Ҫȷ����Ч���ݵ����� */
    {
        unsigned char offset = end_buf;
        
        gps_data->utc.date = (address_buf[offset] - '0') * 10 + address_buf[offset+1] - '0';
        gps_data->utc.strTime[6] = address_buf[offset];
        gps_data->utc.strTime[7] = address_buf[offset + 1];
        offset += 2;
        gps_data->utc.month = (address_buf[offset] - '0') * 10 + address_buf[offset+1] - '0';
        gps_data->utc.strTime[4] = address_buf[offset];
        gps_data->utc.strTime[5] = address_buf[offset + 1];
        offset += 2;
        gps_data->utc.year = (address_buf[offset] - '0') * 10 + address_buf[offset+1] - '0' + 2000;
        gps_data->utc.strTime[0] = '2';
        gps_data->utc.strTime[1] = '0';
        gps_data->utc.strTime[2] = address_buf[offset];
        gps_data->utc.strTime[3] = address_buf[offset + 1];
        gps_data->utc.strTime[14] = 0;
        
        if (flag == 1 &&
            (gps_data->utc.year  >= 2016 && 1 <= gps_data->utc.month && gps_data->utc.month <= 12 && 1 <= gps_data->utc.date && gps_data->utc.date <= 31 &&
             gps_data->utc.hour <= 23 && gps_data->utc.min <= 59 && gps_data->utc.sec <= 59))
        {
            if (0 == ConfigSetRTCTime(gps_data->utc.year, gps_data->utc.month, gps_data->utc.date, 
                             gps_data->utc.hour, gps_data->utc.min, gps_data->utc.sec))
            {
                SetClockSynced(2);
            }
        }

        GSM_LOG_P1("time synced with GPS : %s", gps_data->utc.strTime);
    }
}       

/********************************************************************************************
����GPGSA����
********************************************************************************************/
void GPGSA_Analysis(gps_process_data *gps_data,unsigned char *buf)
{
    unsigned char *address_buf,end_buf,decimal_places;			   
    address_buf=(unsigned char*)strstr((const char *)buf,"$GPGSA");
    end_buf=Data_Removal(address_buf,2);

    if(end_buf!=0XFF)
        gps_data->fixmode=Data_Extraction(address_buf+end_buf,&decimal_places);	//�õ���λ����
}

/********************************************************************************************
����ȥ��
********************************************************************************************/
unsigned char Data_Removal(unsigned char *buf,unsigned char address_buf)
{	 		    
    unsigned char *temp_buf=buf;
    while(address_buf)
    {		 
        if(*buf=='*'||*buf<' '||*buf>'z')
            return 0XFF;
        if(*buf==',')address_buf--;
            buf++;
    }
    return buf-temp_buf;	 
}

/********************************************************************************************
m^n����
********************************************************************************************/
unsigned long int MN_Process(unsigned char m,unsigned char n)
{
    unsigned long int result=1;	 
    while(n--)
        result*=m;    
    return result;
}

/********************************************************************************************
��ȡ��Ҫ������
********************************************************************************************/
int Data_Extraction(unsigned char *buf,unsigned char*decimal_places)
{
    unsigned char *temp_buf=buf;
    unsigned long int intbuf=0,fres=0;
    unsigned char address_int=0,address_decimal_places=0,i,mask=0;
    int res;
    while(1)
    {
        if(*temp_buf=='-')
        {
            mask|=0X02;
            temp_buf++;
        }

        if(*temp_buf==','||(*temp_buf=='*'))
            break;

        if(*temp_buf=='.')
        {
            mask|=0X01;
            temp_buf++;
        }

        else if(*temp_buf>'9'||(*temp_buf<'0'))
        {	
            address_int=0;
            address_decimal_places=0;
            break;
        }

        if(mask&0X01)
            address_decimal_places++;
        else 
            address_int++;

        temp_buf++;
    }

    if(mask&0X02)
        buf++;
    
    for(i=0;i<address_int;i++)
    {  
        intbuf+=MN_Process(10,address_int-1-i)*(buf[i]-'0');
    }

    if(address_decimal_places>5)
        address_decimal_places=5;

    *decimal_places=address_decimal_places;	 		

    for(i=0;i<address_decimal_places;i++)
    {  
        fres+=MN_Process(10,address_decimal_places-1-i)*(buf[address_int+1+i]-'0');
    } 

    res=intbuf*MN_Process(10,address_decimal_places)+fres;

    if(mask&0X02)
        res=-res;		   

    return res;
}	 

