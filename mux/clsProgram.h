


#ifndef _CLS_PROGRAM_H_
#define _CLS_PROGRAM_H_

unsigned char AutoMux_makeMuxInfoAndSend(char *ip, int outChannel, unsigned char isNeedSendDevMux);
unsigned char MakeOutputBytesAndSend(char *ip, int outChn);
unsigned char PrgMuxInfoGet(char *ip);
int NewProgramNum(int curNum, list_t prgNumList);
int NewPid(int inChn, int oldPid, int curPid, int pidOffset, list_t usedPidTree, unsigned char isUsedPid[] , int isUsedPidLenth, int  repeatIsOk);
int DesPidRefresh(int inChn, int prgIndex, int avIndex,Commdes_t *desList,int desListLen,int pidOffset, unsigned char isUsedPid[],list_t usedPidTree)
;
int AutoMakeNewPid(int outChannel);
int MakePidMapTable(int outChannel,list_t  prginfolist, list_t *stanPrgAVMuxList);

void printPrgAVMuxList();

#endif

