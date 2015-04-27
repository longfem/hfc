
#include <stdio.h>
#include "datastructdef.h"
#include "list.h"

extern ClsMux_st *pclsMux;
extern ClsParams_st *pdb;
extern ClsProgram_st clsProgram;
static int isTableInit=0;


//return 
//-1001:pid directly failed
///return
int MakeTable(int outChnId)
{
	ChannelProgramSt *outpst = NULL;
	list_get(&(clsProgram.outPrgList), outChnId-1, &outpst);	
	return buildTable(outChnId,pdb->pvalueTree->poutChnArray,outpst->prgNodes,outpst->caNode);
}




int buildTable(int outChnId, 	DatabaseOutputChannel_st *outChnArray,	list_t  prginfolist,Chn_ca_st *caNode)
{

	


	//Error_psiTable errFlag;
	int outChnIndex = outChnId - 1;
	int rstPat;
	int i;


	ClsMuxInit(2,2);
	//ClsMuxInit(2,2);


	//int streamId = 1;
	//int netWorkId = 16;
	//int version = 2;
	unsigned char patTable[188];
	unsigned char pmtTable[188];
	unsigned char sdtTable[8 * 188];
	unsigned char catTable[188];
	Dev_prgInfo_st *ptmpPrgInfo;

	

	//if (AutoMakeNewPid(outChnId)==0)
	//	return 0;
 	 //MakePidMapTable(outChnId,prginfolist,clsProgram.PrgAVMuxList);
  	MakePidMapTable(outChnId);
  	printf("------------->>>end\n");
	int selCnt = CountSelectedPrgCnt(outChnId);
	if (selCnt > 29)
	{
		printf("out channel  prg number out of range\n");
		return 0;
	}

	int streamId = outChnArray[outChnIndex].streamId;
	int netWorkId = outChnArray[outChnIndex].networkId;
	int oringinalNetworkId = outChnArray[outChnIndex].oringal_networkid;
	int version = outChnArray[outChnIndex].version;

	// PAT

	if (outChnArray[outChnIndex].isAutoRaiseVersion==1) 
	{
		version++;
		if (version > 31)
			version = 0;
		outChnArray[outChnIndex].version = version;
	}
	BufferUn_st  *pbuff;


	rstPat=CreatePat(prginfolist,patTable, streamId, netWorkId, version);	
	list_get(&pclsMux->table_pat,outChnIndex,&pbuff);
	memcpy(pbuff->pbuf, patTable, sizeof(patTable));
	pbuff->bufLen=sizeof(patTable);
	//ist_set(&pclsMux->table_pat,outChnIndex,pbuff);

	if (!rstPat)
	{
		CleanOutputTable(outChnId);
		//ShowCreatTableErrorInfo(errFlag, "[PAT]");
		return 0;
	}
	else
	{	
		printf("make pat SUCCESSFULL---%d\n",rstPat);
	}


#if 0
	pat_senction_st *p_pat = (pat_senction_st*)malloc(sizeof(pat_senction_st));	
	rstPat=ParsePat(pbuff->pbuf, 5, p_pat);
	if(rstPat)
	{
		printPAT(p_pat);
	}

#endif


	//PMT

	list_t *tablePmt;
	list_get(&pclsMux->table_pmtList, outChnIndex, &tablePmt);
	while(list_len(tablePmt))
	{
	    BufferUn_st  *outPMTBuffer;
		list_get(tablePmt,list_len(tablePmt)-1,&outPMTBuffer);
		free(outPMTBuffer->pbuf);
		free(outPMTBuffer);
		list_pop_tail(tablePmt);	
	}
	//for (i = 0; i < list_len(&prginfolist); i++)
	for (i = 0; i < list_len(&prginfolist); i++)
	{
		list_get(&prginfolist, i, &ptmpPrgInfo);	
		rstPat=CreatePmt(ptmpPrgInfo,pmtTable,version);
		if(rstPat)
		{
			printf("make pmt SUCCESSFULL---%d\n",rstPat);
			BufferUn_st *pmtbuff =(BufferUn_st*)malloc(sizeof(BufferUn_st));

			pmtbuff->pbuf=malloc(sizeof(pmtTable));
			memcpy(pmtbuff->pbuf, pmtTable, sizeof(pmtTable));
			pmtbuff->bufLen=sizeof(pmtTable);
			list_append(tablePmt,pmtbuff);	

		}
		else
		{
			CleanOutputTable(outChnId);
			return 0;

		}


#if 0

		BufferUn_st  *outPMTBuffer;
		list_get(&pclsMux->table_pmtList, outChnIndex, &tablePmt);
		list_get(tablePmt,i,&outPMTBuffer);
		pmt_senction_st *PMTS = (pmt_senction_st*)malloc(sizeof(pmt_senction_st));
		int rst=ParsePmt(outPMTBuffer->pbuf, 5, PMTS);
		printPMT(PMTS);
#endif

	}	


	//SDT
	rstPat=CreateSdt(prginfolist,sdtTable, streamId, oringinalNetworkId, version);		

	list_get(&pclsMux->table_sdt,outChnIndex,&pbuff);
	memcpy(pbuff->pbuf, sdtTable,rstPat);
	pbuff->bufLen=rstPat;

	if (!rstPat)
	{
		CleanOutputTable(outChnId);

		return 0;
	}
	else
	{	

		printf("make SDT SUCCESSFULL---%d\n",rstPat);

	}

#if 0

	sdt_senction_st *p_sdt = (sdt_senction_st*)malloc(sizeof(sdt_senction_st));	
	rstPat=ParseSdt(pbuff->pbuf, 5, p_sdt);	


	if(rstPat)
	{
		printf("PRASE sdt SUCCESSFULL---%d\n",rstPat);
		printSDT(p_sdt);
	}
#endif


	//CAT
#if 0
	if (list_len(&caNode->caIdenList)>0)
	{
		rstPat = CreateCat(caNode, catTable, version);
		list_get(&pclsMux->table_cat,outChnIndex,&pbuff);
		memcpy(pbuff->pbuf, catTable, sizeof(catTable));
		pbuff->bufLen=sizeof(catTable);

		if (!rstPat)
		{
			return 0;
		}
		else
		{	
			printf("make cat SUCCESSFULL---%d\n",rstPat);

		}

	}
	else
	{
		printf("no need to make cat\n");

	}

#endif



	if (DirectlyTransmit_repeatePid_verify(outChnId)==0)
		return -1001;

	return 1;
}

void printPAT(pat_senction_st* PATS)
{
	int i;

	printf("crc32----%d\n",PATS->crc32);
	printf("current_next_indicator----%d\n",PATS->current_next_indicator);
	printf("flag_0----%d\n",PATS->flag_0);
	printf("last_section_number----%d\n",PATS->last_section_number);
	printf("reserved0----%d\n",PATS->reserved0);
	printf("reserved1----%d\n",PATS->reserved1);
	printf("section_length----%d\n",PATS->section_length);
	printf("section_number----%d\n",PATS->section_number);
	printf("section_syntax_indicator----%d\n",PATS->section_syntax_indicator);

	printf("table_id----%d\n",PATS->table_id);
	printf("transport_stream_id----%d\n",PATS->transport_stream_id);
	printf("version_number----%d\n",PATS->version_number);

	patPrg_t* p_program = PATS->patPrg_tList;
	for (i = 0; i < PATS->patPrg_tListLen; i++)
	{
		printf("patPrg_st pid----%d\n",p_program->pid);
		printf("patPrg_st program_number ----%d\n",p_program->program_number);
		printf("patPrg_st reserved----%d\n",p_program->reserved);
		p_program++;
	}


}


void printPMT(pmt_senction_st* PMTS)
{

	int i;
	int j;

	printf("crc32----%d\n",PMTS->crc32);
	printf("current_next_indicator----%d\n",PMTS->current_next_indicator);
	printf("flag_0----%d\n",PMTS->flag_0);
	printf("last_section_number----%d\n",PMTS->last_section_number);
	printf("pcrPid----%d\n",PMTS->pcrPid);
	printf("program_info_length----%d\n",PMTS->program_info_length);
	printf("program_number----%d\n",PMTS->program_number);

	printf("reserved0----%d\n",PMTS->reserved0);
	printf("reserved1----%d\n",PMTS->reserved1);
	printf("reserved2----%d\n",PMTS->reserved2);
	printf("reserved3----%d\n",PMTS->reserved3);
	printf("section_length----%d\n",PMTS->section_length);
	printf("section_number----%d\n",PMTS->section_number);
	printf("section_syntax_indicator----%d\n",PMTS->section_syntax_indicator);

	printf("table_id----%d\n",PMTS->table_id);
	printf("version_number----%d\n",PMTS->version_number);


	int k;

	Commdes_t* p_program = PMTS->pmtDesList;

	for (i = 0; i < PMTS->pmtDesListLen; i++)
	{

		printf("Commdes_st index----%d\n",p_program->index);
		printf("Commdes_st tag ----%d\n",p_program->tag);
		printf("Commdes_st userNew----%d\n",p_program->userNew);
		printf("Commdes_st dataLen %d \n",p_program->dataLen);



		unsigned char *ptmp=p_program->data;

		for (k = 0; k < p_program->dataLen; k++)
		{	  

			printf(" %d  ",*ptmp++);		  

		}




		p_program++;

	}


	DataStream_t* p_last_DataStream_t = PMTS->pdataStreamList;

	for (i = 0; i < PMTS->pdataStreamListLen; i++)
	{
		printf("DataStream_st inChn----%d\n",p_last_DataStream_t->inChn);
		printf("DataStream_st index ----%d\n",p_last_DataStream_t->index);
		printf("DataStream_st inPid----%d\n",p_last_DataStream_t->inPid);
		printf("DataStream_st outPid----%d\n",p_last_DataStream_t->outPid);
		printf("DataStream_st streamType----%d\n",p_last_DataStream_t->streamType);


		p_program = p_last_DataStream_t->desNode;
		for (j = 0; j< p_last_DataStream_t->desNodeLen; j++)
		{
			printf("Commdes_st index----%d\n",p_program->index);
			printf("Commdes_st tag ----%d\n",p_program->tag);
			printf("Commdes_st userNew----%d\n",p_program->userNew);
			printf("Commdes_st dataLen is :%d  vList: ",p_program->dataLen);


			unsigned char *ptmp=p_program->data;

			for (k = 0; k < p_program->dataLen; k++)
			{

				printf(" %d  ",*ptmp++);			

			}

			p_program++;
		}

		p_last_DataStream_t++;
	}
}





void printSDT(sdt_senction_st* SDTS)
{

	int i;
	int j;

#if 1
	printf("crc32----%d\n",SDTS->crc32);
	printf("current_next_indicator----%d\n",SDTS->current_next_indicator);
	printf("last_section_number----%d\n",SDTS->last_section_number);
	printf("original_network_id----%d\n",SDTS->original_network_id);
	printf("reserved_future_use1----%d\n",SDTS->reserved_future_use1);
	printf("reserved_future_use1----%d\n",SDTS->reserved_future_use2);


	printf("reserved0----%d\n",SDTS->reserved0);
	printf("reserved1----%d\n",SDTS->reserved1);

	printf("section_length----%d\n",SDTS->section_length);
	printf("section_number----%d\n",SDTS->section_number);
	printf("section_syntax_indicator----%d\n",SDTS->section_syntax_indicator);

	printf("table_id----%d\n",SDTS->table_id);
	printf("transport_stream_id----%d\n",SDTS->transport_stream_id);
	printf("version_number----%d\n",SDTS->version_number);
#endif
	int k;


	sdtPrgName_st* p_last_sdtPrgName_t = SDTS->nameList;
	//printf("SDTS->nameListLen----%d\n",SDTS->nameListLen);

	for (i = 0; i < SDTS->nameListLen; i++)
	{	
#if 1
		printf("descriptors_loop_length ----%d\n",p_last_sdtPrgName_t->descriptors_loop_length);
		printf("EIT_present_following_flag----%d\n",p_last_sdtPrgName_t->EIT_present_following_flag);
		printf("EIT_schedule_flag----%d\n",p_last_sdtPrgName_t->EIT_schedule_flag);
		printf("free_CA_mode----%d\n",p_last_sdtPrgName_t->free_CA_mode);
		printf("DataStream_st streamType----%d\n",p_last_sdtPrgName_t->reserved_future_use);

		printf("runing_status ----%d\n",p_last_sdtPrgName_t->runing_status);	
#endif
		printf("service_id----%d\n",p_last_sdtPrgName_t->service_id);


		Commdes_t* p_command_st = p_last_sdtPrgName_t->desList;
		for (j = 0; j< p_last_sdtPrgName_t->desListLen; j++)
		{
			printf("Commdes_st index----%d\n",p_command_st->index);
			printf("Commdes_st tag ----%d\n",p_command_st->tag);
			printf("Commdes_st userNew----%d\n",p_command_st->userNew);
			printf("Commdes_st dataLen is :%d  vList: \n",p_command_st->dataLen);
			printf("Commdes_st dataLen is :%d  vList: ",p_command_st->dataLen);

			unsigned char *ptmp=p_command_st->data;
			for (k = 0; k < p_command_st->dataLen; k++)
			{
				printf(" %d  ",*ptmp++);			

			}

			p_command_st++;
		}

		p_last_sdtPrgName_t++;
	}
}






void printCAT(cat_senction_st *SDTS)
{

	int i;
	int j;

#if 0
	printf("crc32----%d\n",SDTS->crc32);
	printf("current_next_indicator----%d\n",SDTS->current_next_indicator);
	printf("last_section_number----%d\n",SDTS->last_section_number);
	printf("original_network_id----%d\n",SDTS->original_network_id);
	printf("reserved_future_use1----%d\n",SDTS->reserved_future_use1);
	printf("reserved_future_use1----%d\n",SDTS->reserved_future_use2);


	printf("reserved0----%d\n",SDTS->reserved0);
	printf("reserved1----%d\n",SDTS->reserved1);

	printf("section_length----%d\n",SDTS->section_length);
	printf("section_number----%d\n",SDTS->section_number);
	printf("section_syntax_indicator----%d\n",SDTS->section_syntax_indicator);

	printf("table_id----%d\n",SDTS->table_id);
	printf("transport_stream_id----%d\n",SDTS->transport_stream_id);
	printf("version_number----%d\n",SDTS->version_number);

	int k;


	sdtPrgName_st* p_last_sdtPrgName_t = SDTS->nameList;
	//printf("SDTS->nameListLen----%d\n",SDTS->nameListLen);

	for (i = 0; i < SDTS->nameListLen; i++)
	{	

		printf("descriptors_loop_length ----%d\n",p_last_sdtPrgName_t->descriptors_loop_length);
		printf("EIT_present_following_flag----%d\n",p_last_sdtPrgName_t->EIT_present_following_flag);
		printf("EIT_schedule_flag----%d\n",p_last_sdtPrgName_t->EIT_schedule_flag);
		printf("free_CA_mode----%d\n",p_last_sdtPrgName_t->free_CA_mode);
		printf("DataStream_st streamType----%d\n",p_last_sdtPrgName_t->reserved_future_use);

		printf("runing_status ----%d\n",p_last_sdtPrgName_t->runing_status);	

		printf("service_id----%d\n",p_last_sdtPrgName_t->service_id);


		Commdes_t* p_command_st = p_last_sdtPrgName_t->desList;
		for (j = 0; j< p_last_sdtPrgName_t->desListLen; j++)
		{
			printf("Commdes_st index----%d\n",p_command_st->index);
			printf("Commdes_st tag ----%d\n",p_command_st->tag);
			printf("Commdes_st userNew----%d\n",p_command_st->userNew);
			printf("Commdes_st dataLen is :%d  vList: \n",p_command_st->dataLen);
			printf("Commdes_st dataLen is :%d  vList: ",p_command_st->dataLen);

			unsigned char *ptmp=p_command_st->data;
			for (k = 0; k < p_command_st->dataLen; k++)
			{
				printf(" %d  ",*ptmp++);			

			}

			p_command_st++;
		}

		p_last_sdtPrgName_t++;
	}
	#endif
}


int CleanOutputTable(int outChannel)
{
	
	BufferUn_st  *pbuff;
	int outChnIndex=outChannel-1;


	//PAT
	list_get(&pclsMux->table_pat,outChnIndex,&pbuff);
	pbuff->bufLen=9999;


	//PMT
	list_t *tablePmt;
	list_get(&pclsMux->table_pmtList, outChnIndex, &tablePmt);
	while(list_len(tablePmt))
	{
	    BufferUn_st  *outPMTBuffer;
		list_get(tablePmt,list_len(tablePmt)-1,&outPMTBuffer);
		free(outPMTBuffer->pbuf);
		free(outPMTBuffer);
		list_pop_tail(tablePmt);	
	}

	//SDT
	list_get(&pclsMux->table_sdt,outChnIndex,&pbuff);
	pbuff->bufLen=9999;

	
	

}



void ClsMuxInit(int _outMaxNum,int treeView_inLength)
{
	int i;
	if(isTableInit)
	{

		printf("pclsMuxb is already not null****\n");
	}
	else
	{	
		printf("go to init pclsMuxb****\n");

		if (_outMaxNum > 0)
		{
			pclsMux=(ClsMux_st*)malloc(sizeof(ClsMux_st));
			list_init(&pclsMux->table_pat);
			list_init(&pclsMux->table_pmtList);
			list_init(&pclsMux->table_sdt);
		}
		for (i = 0; i < treeView_inLength; i++)
		{
			if (_outMaxNum > 0)
			{
				BufferUn_st *pbuff =(BufferUn_st*)malloc(sizeof(BufferUn_st));

				pbuff->pbuf=malloc(188);
				pbuff->bufLen=9999;
				list_append(&pclsMux->table_pat,pbuff);


				pbuff =(BufferUn_st*)malloc(sizeof(BufferUn_st));

				pbuff->pbuf=malloc(8 * 188);
				pbuff->bufLen=9999;
				list_append(&pclsMux->table_sdt,pbuff);


				list_t *table_pmt=(list_t*)malloc(sizeof(list_t));
				list_init(table_pmt);
				list_append(&pclsMux->table_pmtList,table_pmt);

			}
		}	
		isTableInit=1;

	}

}




