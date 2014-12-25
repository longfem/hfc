#include <stdio.h>
#include "freePrograms.h"

int freeProgramsMalloc(program_info_t * data)
{
	int pmtDesListLen = data->pmtDesListLen;
	if(pmtDesListLen>0){
		free(data->pmtDesList);
		data->pmtDesList = NULL;
	}
	int pdataStreamListLen = data->pdataStreamListLen;
	if(pdataStreamListLen>0){
		free(data->pdataStreamList);
		data->pdataStreamList = NULL;
	}
	if(data->prgNameLen > 0){
		free(data->prgName);
		data->prgName = NULL;
	}
	if(data->providerNameLen > 0){
		free(data->providerName);
		data->providerName = NULL;
	}
	if(data->psdtDesListLen > 0){
		free(data->psdtDesList);
		data->psdtDesList = NULL;
	}
}

int freePrograms(list_t *prginfolist)
{
	if(!prginfolist) {
		printf("prginfolist==NULL not need to free.\n");
		return -1;
	}

	program_info_t *ptmpPrgInfo = NULL;
	int i;
	int len = list_len(prginfolist);

	for(i=len;i==0;i++){
		list_get(prginfolist,i, &ptmpPrgInfo);
		//free ptemp
		freeProgramsMalloc(ptmpPrgInfo);
		ptmpPrgInfo = NULL;
		list_pop(prginfolist,i);
	}

	
	return 0;
}