
/***

History:
[2016-05-21 Ted]: Create
[2016-09-02 Ted]: separate from platform and user

*/

#ifndef _FEATURE_NAME_H
#define _FEATURE_NAME_H

#ifdef __cplusplus
 extern "C" {
#endif 


#include "cmsis_os.h"



/** it's better to keep string length less than 16 character */  
#define FeatureGps          "AppGps"
#define FeatureGsm          "AppGsm"
#define FeatureCP15         "AppCP15"
#define FeatureAlpha        "AlphaSense"
#define FeatureAm2303       "Am2303"
#define FeatureFatFS        "FatFS"
#define FeatureDn7c3        "Dn7c3"



#ifdef __cplusplus
}
#endif

#endif // _FEATURE_NAME_H

// end of file


