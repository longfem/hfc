
#include "datastructdef.h"
#include "list.h"
#ifndef _CLS_MAKE_TABLE_H_
#define _CLS_MAKE_TABLE_H_

int MakeTable(int outChnId, DatabaseOutputChannel_st outChnArray[],	list_t  prginfolist);
void printPAT(pat_senction_st* PATS);
void printPMT(pmt_senction_st* PMTS);
int AutoMakeNewPid(int outChannel);
int MakePidMapTable(int outChannel);
int CleanOutputTable(int outChannel);

#endif
