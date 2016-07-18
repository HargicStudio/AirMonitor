


/***

History:
2016-04-30: Ted: Create

*/

#ifndef _APIINTERNALMSG_H_
#define _APIINTERNALMSG_H_

#ifdef __cplusplus
 extern "C" {
#endif 


#include "AaPlatform.h"
#include <stdbool.h>


enum {
    API_MESSAGE_ID_NONE = 0x0000,
    // platform layer from 0x1000
    
    
    // application layer from 0x2000

    
    API_MESSAGE_ID_MAX = 0xFFFF,
};




#ifdef __cplusplus
}
#endif

#endif // _APIINTERNALMSG_H_

// end of file


