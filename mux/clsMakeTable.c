
#include <stdio.h>
#include "EnumDef.h"
#include "datastructdef.h"
#include "list.h"

static  pat_senction_st *table_pat[4];

extern ClsProgram_st clsProgram;

int MakeTable(int outChnId, DatabaseOutputChannel_st outChnArray[],	list_t  prginfolist)
{
	int outChnIndex = outChnId - 1;
	int streamId = 1;
	int netWorkId = 16;
	int oringinalNetworkId = 2184;
	int version = 2;
	unsigned char tmpBytes[1024];
	Dev_prgInfo_st *ptmpPrgInfo;
/*
	int streamId = outChnArray[outChnIndex].streamId;
	int netWorkId = outChnArray[outChnIndex].networkId;
	int oringinalNetworkId = outChnArray[outChnIndex].oringal_networkid;
	int version = outChnArray[outChnIndex].version;
*/
	// PAT
	/*
	if (outChnArray[outChnIndex].isAutoRaiseVersion) // 自动增加版本号
	{
	version++;
	if (version > 31)
	version = 0;
	outChnArray[outChnIndex].version = version;
	//db.SettingSave();
	}

	*/

	//table_pat[outChnIndex] = CreatePatNoju(buildOoutPrgList(),tmpBytes, streamId, netWorkId, version);



	table_pat[outChnIndex] = CreatePatStan(prginfolist,tmpBytes, streamId, netWorkId, version);
	////printf("call getprograms	prgnum = [%0x]\n",	((program_info_t *)(prginfolist.head->data))->prgNum);
	pat_senction_st* PATS=table_pat[outChnIndex];
	//printPAT(PATS);

  //   pat_senction_st *p_pat = (pat_senction_st*)malloc(sizeof(pat_senction_st));
	 // int xx=ParsePat(tmpBytes, 5, p_pat);

	  // printPAT(p_pat);


	//pmt
	list_get(&prginfolist, 0, &ptmpPrgInfo);
	pmt_senction_st* PMTS=CreatePmtStan(ptmpPrgInfo,tmpBytes,version);

	//pmt_senction_st *p_pmt = (pmt_senction_st*)malloc(sizeof(pmt_senction_st));
//int 	xx=ParsePmt(tmpBytes, 5, p_pmt);
	printPMT(PMTS);


}

void printPAT(pat_senction_st* PATS)
{
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

	  patPrg_t* p_program = PATS->p_first_program;
	  while(p_program != NULL)
	  {
		  printf("patPrg_st pid----%d\n",p_program->pid);
		  printf("patPrg_st program_number ----%d\n",p_program->program_number);
		  printf("patPrg_st reserved----%d\n",p_program->reserved);
		  p_program=p_program->p_next;
	  }


}


void printPMT(pmt_senction_st* PMTS)
{int i;
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



	 Commdes_t* p_program = PMTS->pmtDesList;

	for (i = 0; i < PMTS->pmtDesListLen; i++)
			{
			
			printf("Commdes_st index----%d\n",p_program->index);
			printf("Commdes_st tag ----%d\n",p_program->tag);
			printf("Commdes_st userNew----%d\n",p_program->userNew);
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
		   p_program++;
	   }

	p_last_DataStream_t++;


	}



}

int AutoMakeNewPid(int outChannel)
{

}

int MakePidMapTable(int outChannel)
{

}

int CleanOutputTable(int outChannel)
{

}


