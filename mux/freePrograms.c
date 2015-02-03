#include <stdio.h>
#include "freePrograms.h"


int freeProgramsMalloc(Dev_prgInfo_st * data)
{
	int i = 0, j=0;
	int pmtDesListLen = data->pmtDesListLen;
	for(i=0; i < pmtDesListLen; i++){		
			free(data->pmtDesList[i].data);
			data->pmtDesList[i].data = NULL;		
	}
	if(data->pmtDesList){
		free(data->pmtDesList);
		data->pmtDesList = NULL;
	}
	int pdataStreamListLen = data->pdataStreamListLen;
	if(pdataStreamListLen>0){
		for(i=0; i< pdataStreamListLen; i++){
			for(j=0; j < data->pdataStreamList[i].destlen; j++)
			{				
				if(data->pdataStreamList[i].desNode[j].data){
					free(data->pdataStreamList[i].desNode[j].data);
					data->pdataStreamList[i].desNode[j].data = NULL;
				}												
			}

			free(data->pdataStreamList[i].desNode);
			data->pdataStreamList[i].desNode = NULL;
		
		}
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


	Dev_prgInfo_st *ptmpPrgInfo = NULL;
	int i;
	int len = list_len(prginfolist);

	for(i= len -1 ;i<0;i--){
		list_get(prginfolist,i, &ptmpPrgInfo);
		//free ptemp
		if(ptmpPrgInfo != NULL){
			freeProgramsMalloc(ptmpPrgInfo);
			ptmpPrgInfo = NULL;
			list_pop(prginfolist,i);
		}		
		
	}

	prginfolist = NULL;

	
	return 0;
}