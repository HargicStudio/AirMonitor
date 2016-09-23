
/***

History:
[2016-05-21 Ted]: Create
[2016-09-02 Ted]: separate from platform and user

*/

#ifndef _FEATURE_ID_H
#define _FEATURE_ID_H

#ifdef __cplusplus
 extern "C" {
#endif 


#include "cmsis_os.h"



/** it's better to keep string length less than 16 character */  
#define SystemStartup       "System"
#define FeatureCCS          "CCSDaemon"
#define FeatureAaThread     "CCSThread"
#define FeatureSysTag       "CCSSysTag"
#define FeatureSysLog       "CCSSysLog"
#define FeatureSysMem       "CCSSysMem"
#define FeatureSysCom       "CCSSysCom"
#define FeatureAaShell      "CCSShell"
#define FeaturePP           "PingPangBuf"


#ifdef __cplusplus
}
#endif

#endif // _FEATURE_ID_H

// end of file


