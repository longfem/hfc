
#include <stdio.h>
#include <stdlib.h>
#include "list.h"
#include "datastructdef.h"
#include "communicate.h"
#include "clsParams.h"

ClsParams_st *pdb = NULL;

void Init(int outChannelNumMax)
{
	int i = 0;
	if(pdb){
		printf("ClsParams_st had init already. don't init again.\n");
		return;
	}
	pdb->channelNumMax = 0;
	pdb->pvalueTree = malloc(sizeof(Database_st));

	pdb->pvalueTree->isAutoMakeMux = 1;
	pdb->pvalueTree->isAutoOutTreeListExpandAll = 1;
	pdb->pvalueTree->isAutoInTreeListExpandAll = 1;
	pdb->pvalueTree->isPrgInfoHexPid = 1;
	pdb->pvalueTree->isConnectPid = 0;
	
	pdb->pvalueTree->isLogEnable = 1;

	pdb->pvalueTree->startPid = 0x100;
	pdb->pvalueTree->poutChnArray = malloc( sizeof(DatabaseOutputChannel_st) * outChannelNumMax);
	for (i = 0; i < outChannelNumMax; i++)
	{						
		pdb->pvalueTree->poutChnArray[i].networkId = 0x10;
		pdb->pvalueTree->poutChnArray[i].oringal_networkid = i + 1;
		pdb->pvalueTree->poutChnArray[i].streamId = i + 1;
		pdb->pvalueTree->poutChnArray[i].outputRate = 0;
		pdb->pvalueTree->poutChnArray[i].isAutoRaiseVersion = 1;
		pdb->pvalueTree->poutChnArray[i].version = 1;
		pdb->pvalueTree->poutChnArray[i].isManualMapMode = 1;
		pdb->pvalueTree->poutChnArray[i].isAutoRankPAT = 0;
		
		pdb->pvalueTree->poutChnArray[i].isNeedSend_cat = 1;
		pdb->pvalueTree->poutChnArray[i].isNeedSend_nit = 1;
		pdb->pvalueTree->poutChnArray[i].isNeedSend_pat = 1;
		pdb->pvalueTree->poutChnArray[i].isNeedSend_pmt = 1;
		pdb->pvalueTree->poutChnArray[i].isNeedSend_sdt = 1;
	}
}
