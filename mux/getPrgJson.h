#ifndef _GET_PROGRAMS_H_
#define _GET_PROGRAMS_H_

#include "datastructdef.h"
#include "list.h"

void getPrgsJson(char *ip, int inChn, char *outprg, char *lan);
void adduserprgjson(cJSON *basearry, ChannelProgramSt *pst, char *lan);
void getoutprgsJson(char *ip, int inChn, char *outprg, char *lan);
void getBackupJson(char *ip, char *outprg);
#endif 
