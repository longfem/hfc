

#ifndef _CLS_MUX_SEND_H_
#define _CLS_MUX_SEND_H_

int SendTable(char *ip, int outChnId);
int SendPidMap(char *ip, int outChnId);
int CountSelectedPrgCnt(int outChnId);
void SendMux(char *ip, int outChnId);

#endif

