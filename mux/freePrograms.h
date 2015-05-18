#include "datastructdef.h"
#include "list.h"



#ifndef _FREE_PROGRAMS_H_
#define _FREE_PROGRAMS_H_


int freePrograms(list_t *prginfolist);
int freeUserPrograms(list_t *prginfolist);
int freeProgramsMalloc(Dev_prgInfo_st * data);
int freeUserProgramsMalloc(User_prgInfo_t * data);
int freecanode(list_t *caIdenList);
#endif 