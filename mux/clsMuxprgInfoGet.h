

#include "EnumDef.h"
#include "list.h"

#ifndef _CLSMUX_PRGINFO_GET_H_
#define _CLSMUX_PRGINFO_GET_H_

ErrorTypeEm SetOutRate(char *ip, int outChannel, int outputRate);
ErrorTypeEm GetPidOffset(char *ip, int outChn, unsigned int *outPid);
int GetChannelOutputMaxRate(char *ip, int outChannel, unsigned int *outRate);
ErrorTypeEm GetOutProgramMuxMap(char *ip, int outChannel, list_t *muxPrgInfoList); // MuxPrgInfo_st;
ErrorTypeEm GetOutPidMuxMap(char *ip, int outChannel, list_t *muxPidInfoList); // MuxPidInfo_st;
ErrorTypeEm SendOutPrgMuxMap(char *ip, int outChannel, list_t *pmuxPrgInfoList);
ErrorTypeEm SendOutPrgMuxMap_finish(char *ip, int outChannel);
ErrorTypeEm SendEnableMuxTable(char *ip, int outChannel);
ErrorTypeEm GetCatDesList(char *ip, int channelId, list_t *catDesList);
list_t * MaketPaketSection(unsigned char *table, int length);
ErrorTypeEm SendTable_pmt(char *ip, int outChannel, list_t *pmtList);
ErrorTypeEm SendTable_psi(char *ip, int outChannel, PsiTableType tableType, unsigned char *ptableBytes, int length);
ErrorTypeEm SendTable_psi_finish(char *ip, int outChannel);
ErrorTypeEm SendTable_PidMap(char *ip, int outChannel, list_t **pidMapListArray);
ErrorTypeEm DirectlyTransmit_sendMap(char *ip, int outChannel, list_t *pidMapList);
unsigned char SendOutputPrgInfo(char *ip, int outChn, unsigned char *muxInfoBytes, int length);
void LoadBitrateAndTableEnable(char *ip, int iChn);
int GetNewFreePrgIndex(int inChn, int outChnId);
int GetNewFreeUserPrgIndex(int inChn, int outChnId);
int FlagInputSignal(char *ip, int *inputStatus);
int CheckReturnBytes(char *cmdHeadBytes, int headLen, char *rtnBytes, int rtnBytesLen);
//SearchingStatus GetSearchingStatus(char *ip, int inChn);
//ErrorTypeEm Search(char *ip, int inChn);

#endif