

#include "EnumDef.h"

#ifndef _CLSMUX_PRGINFO_GET_H_
#define _CLSMUX_PRGINFO_GET_H_

ErrorTypeEm GetPidOffset(char *ip, int outChannel, unsigned int *outPid);
ErrorTypeEm SendOutPrgMuxMap(char *ip, int outChannel, list_t *pmuxPrgInfoList);
#endif