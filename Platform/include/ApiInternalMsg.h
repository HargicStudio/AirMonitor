


/***

History:
2016-04-30: Ted: Create

*/

#ifndef _APIINTERNALMSG_H_
#define _APIINTERNALMSG_H_

#ifdef __cplusplus
 extern "C" {
#endif 


#include <stdbool.h>


#define API_BASE_APPL       0x2000

enum {
    API_MESSAGE_ID_NONE = 0x0000,
    // platform layer from 0x1000
    
    
    // application layer from 0x2000
    API_MESSAGE_ID_LED_INDICATION = API_BASE_APPL + 1,
    API_MESSAGE_ID_AIR_SAMPLE_INDICATION = API_BASE_APPL + 2,
    
    API_MESSAGE_ID_MAX = 0x8FFF,
};



typedef struct 
{
    bool led_status;
} SLedIndication;


#ifdef __cplusplus
}
#endif

#endif // _APIINTERNALMSG_H_

// end of file


