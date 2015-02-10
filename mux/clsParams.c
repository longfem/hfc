
#include <stdio.h>
#include <stdlib.h>
#include "list.h"
#include "datastructdef.h"
#include "communicate.h"
#include "clsParams.h"

ClsParams_st *pdb = NULL;
extern ClsProgram_st clsProgram;
void Init(int outChannelNumMax)
{
	int i = 0;
	if(pdb){
		printf("ClsParams_st had init already. don't init again.\n");
		return;
	}
	pdb = malloc(sizeof(ClsParams_st));
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

	clsProgram.pidMap_eachTransmit_numberMax = 200; // 每次PID映射表传输最大个数

	//add by stan for getmux info init global var
	//MuxPrgInfoGet_st list array	
	
	clsProgram.PrgPmtMuxList = malloc(sizeof(list_t) * clsProgram._outChannelCntMax);	
	list_t *PrgPmtMux = NULL;
	for(i=0; i< clsProgram._outChannelCntMax; i++){
		PrgPmtMux = malloc(sizeof(list_t));
		list_init(PrgPmtMux);
        list_append(clsProgram.PrgPmtMuxList, PrgPmtMux);
	}
	//MuxPidInfo_st list array
	clsProgram.PrgAVMuxList = malloc(sizeof(list_t) * clsProgram._outChannelCntMax);	
	list_t   *prgAVMux = NULL;
	for(i=0; i< clsProgram._outChannelCntMax; i++){
		prgAVMux = malloc(sizeof(list_t));
		list_init(prgAVMux);
        list_append(clsProgram.PrgAVMuxList, prgAVMux);		
	} 
	//////////////////////////////////////////////////////////////////////////////
	// clsProgram.PrgPmtMuxList = malloc(sizeof(list_t) * clsProgram._outChannelCntMax);	
	// MuxPrgInfoGet_st *muxPrgInfo = NULL;
	// for(i=0; i< clsProgram._outChannelCntMax; i++){
	// 	muxPrgInfo = malloc(sizeof(MuxPrgInfoGet_st));
	// 	muxPrgInfo->inChannel = 0x00;
 //        muxPrgInfo->prgIndex =  0x00;
 //        muxPrgInfo->prgNum   =  0x00;        
 //        muxPrgInfo->prgPid = 0x00;
 //        list_init(&clsProgram.PrgPmtMuxList[i]);
 //        list_append(&clsProgram.PrgPmtMuxList[i], muxPrgInfo);
	// }
	// //MuxPidInfo_st list array
	// clsProgram.PrgAVMuxList = malloc(sizeof(list_t) * clsProgram._outChannelCntMax);	
	// MuxPidInfo_st   *prgAVMux = NULL;
	// for(i=0; i< clsProgram._outChannelCntMax; i++){
	// 	prgAVMux = malloc(sizeof(MuxPidInfo_st));
	// 	prgAVMux->inChannel = 0x00;
	// 	prgAVMux->oldPid = 0x00;
	// 	prgAVMux->newPid = 0x00;
	// 	list_init(&clsProgram.PrgAVMuxList[i]);
 //        list_append(&clsProgram.PrgAVMuxList[i], prgAVMux);		
	// } 
}
