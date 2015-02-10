#ifndef _GET_PROGRAM_INFO_H_
#define _GET_PROGRAM_INFO_H_


void getTableJson(int prgcount, char *outprg);
int OutPsiTable_inSdt_search_prgInfo(sdt_senction_st *p_sdt, int seriveid, char *prgName, int *isCrpto);

#endif