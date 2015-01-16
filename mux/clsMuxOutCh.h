
#include "EnumDef.h"


#ifndef _CLS_MUX_OUT_CHANNEL_H_
#define _CLS_MUX_OUT_CHANNEL_H_


ErrorTypeEm GetOutChnTSID(char *ip, int outChn,  unsigned int  *outTsId);
ErrorTypeEm GetOutChnNetID(char *ip, int outChn, unsigned int  *outNetId);
ErrorTypeEm GetOutChnOrgNetID(char *ip, int outChn, unsigned int *outOrgNetId);
ErrorTypeEm GetOutChnTableVer(char *ip, int outChn, unsigned int *outTableVer);
#endif


