
#include <stdio.h>
#include "EnumDef.h"
#include "datastructdef.h"
#include "communicate.h"
#include "clsMuxSend.h"

extern ClsProgram_st clsProgram;
extern ClsParams_st *pdb;
ClsMux_st *pclsMux;
int o_selectedPrgCntMax = 29;

unsigned char SendTable(int outChnId)
{
	unsigned char rslt = 1;
	int iChn = outChnId - 1;
	enum PsiTableType psiType;

	BufferUn_st  *pbuff = NULL;
	if(NULL == pclsMux || NULL == pdb){
		printf("pclsMux== null error.\n");
		return 0;
	}

	list_get(&pclsMux->table_pat, iChn, &pbuff); 
	if (pbuff != NULL && pdb->pvalueTree->poutChnArray[iChn].isNeedSend_pat)
	{
		psiType = pat;
		if (SendTable_psi(outChnId, psiType, pbuff->pbuf, pbuff->bufLen) != ok)
			rslt = 0;
	}
	else
	{
		psiType = pat;
		if (SendTable_psi(outChnId, psiType, NULL, 0) != ok)
			rslt = 0;
	}


	list_t *table_pmt=NULL;
	int table_pmtListLen = list_len(&pclsMux->table_pmtList);
	list_get(&pclsMux->table_pmtList, iChn, &table_pmt);

	
	int table_pmtLen = 0;
	if(NULL!= table_pmt){
		table_pmtLen = list_len(table_pmt);
	}

	if (pdb->pvalueTree->poutChnArray[iChn].isNeedSend_pmt && 
		table_pmt != NULL 
		&& table_pmtListLen > 0
		&& table_pmtLen != NULL)
	{
		if (SendTable_pmt(outChnId, table_pmt) != ok)
			rslt = 0;
	}
	else
	{
		if (SendTable_pmt(outChnId, NULL) != ok)
			rslt = 0;
	}

	pbuff = NULL;	
	list_get(&pclsMux->table_sdt, iChn, &pbuff); 
	psiType = sdt;
	if (pbuff != NULL && pdb->pvalueTree->poutChnArray[iChn].isNeedSend_sdt)
	{					
		if (SendTable_psi(outChnId, psiType, pbuff->pbuf, pbuff->bufLen) != ok)
			rslt = 0;
	}
	else
	{		
		if (SendTable_psi(outChnId, psiType, NULL, 0) != ok)
			rslt = 0;
	}
	pbuff = NULL;	
	list_get(&pclsMux->table_cat, iChn, &pbuff); 
	psiType = cat;
	if (pbuff != NULL && pdb->pvalueTree->poutChnArray[iChn].isNeedSend_cat)
	{		
		if (SendTable_psi(outChnId, psiType, pbuff->pbuf, pbuff->bufLen) != ok)
			rslt = 0;
	}
	else
	{
		if (SendTable_psi(outChnId, psiType, NULL, 0) != ok)
			rslt = 0;
	}

	pbuff = NULL;	
	list_get(&pclsMux->table_nit, iChn, &pbuff); 
	psiType = nit;
	if (pbuff != NULL && pdb->pvalueTree->poutChnArray[iChn].isNeedSend_nit)
	{
		
		if (SendTable_psi(outChnId, psiType, pbuff->pbuf, pbuff->bufLen) != ok)
			rslt = 0;
	}
	else
	{
		if (SendTable_psi(outChnId, psiType, NULL, 0) != ok)
			rslt = 0;
	}
	//	if (rslt)
	// {
	// 	if (SendTable_psi_finish(outChnId) != ok)
	// 		rslt = 0;
	// 	else
	// 		rslt = SendPidMap(outChnId);
	// }

	
	return rslt;
}

// unsigned char SendPidMap(int outChnId)
// {
// 	bool rslt = true;

// 	if (SendTable_PidMap(outChnId, clsProgram.PrgAVMuxList) != ok)
// 		rslt = false;
// 	if (DirectlyTransmit_sendMap(outChnId, clsProgram.outPrgList[outChnId - 1].dtPidList) != ErrorTypeEm.ok)
// 		rslt = false;
// 	if (SendEnableMuxTable(outChnId) != ErrorTypeEm.ok)
// 		rslt = false;

// 	return rslt;
// }

int CountSelectedPrgCnt(int outChnId)
{
	int iSelectedCnt = 0;
	int i=0;
	ChannelProgramSt *eachChn = NULL;

	int outPrgListLen = list_len(&clsProgram.outPrgList);

	if (outChnId <= outPrgListLen)
	{
		for (i = 0; i < outPrgListLen; i++ )
		{
			if (outChnId == 0 || i + 1 == outChnId)
			{
				
				list_get(&clsProgram.outPrgList, i, &eachChn);
				
				if(eachChn){
					iSelectedCnt += list_len(&eachChn->prgNodes);				
					iSelectedCnt += list_len(&eachChn->userPrgNodes);					
				}
				
			}
		}
	}
	
	return iSelectedCnt;
}


void SendMux(int outChnId)
{
	unsigned char isNeedDesInfoSend = 1;
	int selCnt = CountSelectedPrgCnt(outChnId);
	//MuxPrgNumMax
	if (selCnt > o_selectedPrgCntMax) 
	{		
		isNeedDesInfoSend = 0;
		return;
	}
    unsigned char sendRslt = AutoMux_makeMuxInfoAndSend(outChnId, !pdb->pvalueTree->poutChnArray[outChnId - 1].isManualMapMode);
    if (!sendRslt)
    {	    
	    isNeedDesInfoSend = 0;
	    return;
    }
	SendTable(outChnId);
	// if (isNeedDesInfoSend)
	// {
	// 	MakeOutputBytesAndSend(outChnId);
	// }

	// RecordInputChnUseStatus(outChnId);

	// if (_isEnableScrambleModule) // 如果存在加扰模块
	// {				
	// 	if(_isCaRuleEnable)
	// 	{
	// 		sendRslt = CaRule_write() && CaPrgV2_send();
	// 	}
	// 	else
	// 	{
	// 		sendRslt = Ac_send() && CaPrg_send();
	// 	}
	// 	sendRslt &= CaCycle_write();
	// 	if(!sendRslt)
	// 	{
	// 		Shawn.WL.Forms.ClsShowWin.ShowErrMsg(lang.Get("复用信息发送失败") + " (2)");
	// 		return;
	// 	}
	// }
}

