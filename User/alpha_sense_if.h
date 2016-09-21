
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __Alpha_sense_IF_H
#define __Alpha_sense_IF_H

#ifdef __cplusplus
extern "C" {
#endif


#include "AaInclude.h"


typedef enum
{
    AS_TYPE_CO = 1,
    AS_TYPE_NO2,
    AS_TYPE_O3,
    AS_TYPE_SO2
} EASType;


typedef enum
{
    AS_ELECTRODE_WORKER = 1,
    AS_ELECTRODE_AUXILIARY
} EASElec;


typedef struct
{
    EASType chan;
    EASElec elec;
    u32     ad_samp;
    double  volt;
} SAirSamp;



#ifdef __cplusplus
}
#endif

#endif /* __Alpha_sense_IF_H */



