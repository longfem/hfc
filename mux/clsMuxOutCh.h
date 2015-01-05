
#include "EnumDef.h"


#ifndef _CLS_MUX_OUT_CHANNEL_H_
#define _CLS_MUX_OUT_CHANNEL_H_


ErrorTypeEm SetOutChnTSID(char *ip, int outChn, unsigned short tsId);
ErrorTypeEm GetOutChnTSID(char *ip, int outChn,  unsigned short  *outTsId);
ErrorTypeEm GetOutChnNetID(char *ip, int outChn, unsigned short  *outNetId);

#endif


