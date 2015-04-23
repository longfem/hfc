

#include "EnumDef.h"

#ifndef _CLS_MUX_OUT_CHANNEL_H_
#define _CLS_MUX_OUT_CHANNEL_H_

ErrorTypeEm OutChn_validBitrateGet(char *ip, int outChnId, int *outValidBitrate);
ErrorTypeEm GetOutChannelStatus(char *ip, int outChn, unsigned int *outStatus);
ErrorTypeEm GetOutChnTSID(char *ip, int outChn,  unsigned int  *outTsId);
ErrorTypeEm SetOutChnTSID(char *ip, int outChn,  unsigned int  outTsId);
ErrorTypeEm GetOutChnNetID(char *ip, int outChn, unsigned int  *outNetId);
ErrorTypeEm SetOutChnNetID(char *ip, int outChn, unsigned int  outNetId);
ErrorTypeEm GetOutChnOrgNetID(char *ip, int outChn, unsigned int *outOrgNetId);
ErrorTypeEm SetOutChnOrgNetID(char *ip, int outChn, unsigned int outOrgNetId);
ErrorTypeEm GetOutChnTableVer(char *ip, int outChn, unsigned char *outTableVer);
ErrorTypeEm SetOutChnTableVer(char *ip, int outChn, unsigned char outTableVer);
ErrorTypeEm GetTableEnableFlag(char *ip, int outChn, unsigned int *outStatus);
ErrorTypeEm SetTableEnableFlag(char *ip, int outChn, unsigned int outStatus);

#endif


