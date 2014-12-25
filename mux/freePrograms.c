

int freeProgramsMalloc(program_info_t * data)
{
	int pmtDesListLen = program_info_t->pmtDesListLen;
	if(pmtDesListLen>0){
		free(program_info_t->pmtDesList);
		program_info_t->pmtDesList = NULL;
	}
	int pdataStreamListLen = program_info_t->pdataStreamListLen;
	if(pdataStreamListLen>0){
		free(program_info_t->pdataStreamList);
		program_info_t->pdataStreamList = NULL;
	}
	if(program_info_t->prgNameLen > 0){
		free(program_info_t->prgName);
		program_info_t->prgName = NULL;
	}
	if(program_info_t->providerNameLen > 0){
		free(program_info_t->providerName);
		program_info_t->providerName = NULL;
	}
	if(program_info_t->psdtDesListLen > 0){
		free(program_info_t->psdtDesList);
		program_info_t->psdtDesList = NULL;
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