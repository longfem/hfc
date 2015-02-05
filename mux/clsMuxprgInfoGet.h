

#include "EnumDef.h"
#include "list.h"

#ifndef _CLSMUX_PRGINFO_GET_H_
#define _CLSMUX_PRGINFO_GET_H_

ErrorTypeEm SetOutRate(char *ip, int outChannel, int outputRate);
ErrorTypeEm GetPidOffset(char *ip, int outChannel, unsigned int *outPid);
int GetChannelOutputMaxRate(char *ip, int outChannel, unsigned int *outRate);
ErrorTypeEm SendOutPrgMuxMap(char *ip, int outChannel, list_t *pmuxPrgInfoList);
ErrorTypeEm SendTable_pmt(char *ip, int outChannel, list_t *pmtList);
ErrorTypeEm SendTable_psi(char *ip, int outChannel, PsiTableType tableType, unsigned char *ptableBytes, int length);
ErrorTypeEm GetOutProgramMuxMap(char *ip, int outChannel, list_t *muxPrgInfoList);
#endif