
#include "cJSON.h"

#ifndef _CLS_MUX_H_
#define _CLS_MUX_H_

unsigned int CreateTable(int outChnId);
int sendOutPutMuxInfo(char *ip, int outChnId);
int sendOutPutOption(char *ip, int outChnId);
void ShowNeedChnDataButNoInputWarning(int isValidInputStatus, int inputStatus, cJSON *result);
void InputMissShow(int inChnId, int validStatus, cJSON *chjson);
void RecordInputChnUseStatus(int outChnId);
#endif