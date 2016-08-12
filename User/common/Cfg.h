#ifndef CFG_H_
#define CFG_H_

#define CFG_FILE  "AirMonitor.txt"
enum 
{
    C_ADDR,
    C_SOFTVER,
    C_HARDVER,
    C_SERVERIP,
    C_SERVERPORT,
    C_SIMPLEINT,
    C_REPORTINT,
    C_K25,
    C_B25,
    C_K10,
    C_B10,
    C_COVw,
    C_COVa,
    C_SO2Vw,
    C_SO2Va,
    C_O3Vw,
    C_O3Va,
    C_NO2Vw,
    C_NO2Va,
};

int Initcfg(char *temp);
int ReadCfg(int index,char *value);
int ReadCfgInt(int index, int *iVal);

int WriteCfg(int index,char *value);
int WriteCfgInt(int index, int iVal);

int ReadValue(char *key,int keylen, char *value);

int WriteValue(char *key, int keylen, char *value);


#endif