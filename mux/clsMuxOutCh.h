
#include "EnumDef.h"


#ifndef _CLS_MUX_OUT_CHANNEL_H_
#define _CLS_MUX_OUT_CHANNEL_H_


ErrorTypeEm GetOutChnTSID(char *ip, int outChn,  unsigned int  *outTsId);
ErrorTypeEm SetOutChnTSID(char *ip, int outChn,  unsigned int  outTsId);
ErrorTypeEm GetOutChnNetID(char *ip, int outChn, unsigned int  *outNetId);
ErrorTypeEm SetOutChnNetID(char *ip, int outChn, unsigned int  outNetId);
ErrorTypeEm GetOutChnOrgNetID(char *ip, int outChn, unsigned int *outOrgNetId);
ErrorTypeEm SetOutChnOrgNetID(char *ip, int outChn, unsigned int outOrgNetId);
ErrorTypeEm GetOutChnTableVer(char *ip, int outChn, unsigned int *outTableVer);
ErrorTypeEm SetOutChnTableVer(char *ip, int outChn, unsigned int outTableVer);
ErrorTypeEm GetTableEnableFlag(char *ip, int outChn, unsigned int *outStatus);
ErrorTypeEm SetTableEnableFlag(char *ip, int outChn, unsigned int outStatus);
#endif


