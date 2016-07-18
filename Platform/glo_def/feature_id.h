
/***

History:
[2016-05-21 Ted]: Create

*/

#ifndef _FEATURE_ID_H
#define _FEATURE_ID_H

#ifdef __cplusplus
 extern "C" {
#endif 


#include "cmsis_os.h"



/** it's better to keep string 6 character */  
// platform
#define SystemStartup   "System"
#define FeatureCCS      "CCSDmn"
#define FeatureThread   "CCSTrd"
#define FeatureTag      "CCSTag"
#define FeatureLog      "CCSLog"
#define FeatureMem      "CCSMem"
#define FeatureSysCom   "CCSCom"
#define FeatureAaShell  "CCSShl"

// application
#define FeatureGps      "AppGps"
#define FeatureGsm      "AppGsm"


#ifdef __cplusplus
}
#endif

#endif // _FEATURE_ID_H

// end of file


