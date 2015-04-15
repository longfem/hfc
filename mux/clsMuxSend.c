
#include <stdio.h>
#include "EnumDef.h"
#include "datastructdef.h"
#include "communicate.h"
#include "clsMuxSend.h"
#include "clsMuxprgInfoGet.h"

extern ClsProgram_st clsProgram;
extern ClsParams_st *pdb;
ClsMux_st *pclsMux;
int o_selectedPrgCntMax = 29;

int SendTable(char *ip, int outChnId)
{
	int rslt = 1;
	int i=0;
	int iChn = outChnId - 1;
	enum PsiTableType psiType;

	printf("SendTable fuck 0\n");

	BufferUn_st  *pbuff = NULL;
	if(NULL == pclsMux || NULL == pdb){
		printf("pclsMux== null error.\n");
		return 0;
	}

	printf("SendTable fuck 1\n");
	list_get(&pclsMux->table_pat, iChn, &pbuff); 

	if (pbuff != NULL && pdb->pvalueTree->poutChnArray[iChn].isNeedSend_pat)
	{	
		printf("SendTable fuck 2\n");
//	    for(i=0;i<pbuff->bufLen;i++){
//	       printf("send_pat[%x]=%x\n",i, pbuff->pbuf[i]);
//	    }
//	    printf("send_pat end\n");
		if (SendTable_psi(ip, outChnId, pat, pbuff->pbuf, pbuff->bufLen) != ok)
			rslt = 0;
	}
	else
	{		
		printf("SendTable fuck 3\n");
		psiType = pat;
		if (SendTable_psi(ip, outChnId, pat, NULL, 0) != ok)
			rslt = 0;
	}

	
	printf("SendTable fuck 4\n");
	list_t *table_pmt=NULL;
	int table_pmtListLen = list_len(&pclsMux->table_pmtList);
	list_get(&pclsMux->table_pmtList, iChn, &table_pmt);


	int table_pmtLen = 0;
	if(NULL!= table_pmt){
		table_pmtLen = list_len(table_pmt);
	}


	printf("SendTable fuck 5\n");
	if (pdb->pvalueTree->poutChnArray[iChn].isNeedSend_pmt &&
		table_pmt != NULL
		&& table_pmtLen > 0
		)
	{		
		if (SendTable_pmt(ip, outChnId, table_pmt) != ok)
           rslt = 0;

	}
	else
	{	
	    if (SendTable_pmt(ip, outChnId, NULL) != ok)
        		rslt = 0;
	}

	printf("SendTable fuck 6\n");
	pbuff = NULL;
	list_get(&pclsMux->table_sdt, iChn, &pbuff);

	psiType = sdt;
	if (pbuff != NULL && pdb->pvalueTree->poutChnArray[iChn].isNeedSend_sdt)
	{		
		printf("SendTable fuck sdt 611\n");
		if (SendTable_psi(ip, outChnId, sdt, pbuff->pbuf, pbuff->bufLen) != ok)
			rslt = 0;

	}
	else
	{	
		printf("SendTable fuck sdt null\n");
		if (SendTable_psi(ip, outChnId, sdt, NULL, 0) != ok)
			rslt = 0;
	}
	printf("SendTable fuck 7\n");
	///////////////////////////////////////////////////////////////////////////////////////
	// pbuff = NULL;	
	// list_get(&pclsMux->table_cat, iChn, &pbuff); 
	// psiType = cat;
	// if (pbuff != NULL && pdb->pvalueTree->poutChnArray[iChn].isNeedSend_cat)
	// {		
	// 	if (SendTable_psi(ip, outChnId, cat, pbuff->pbuf, pbuff->bufLen) != ok)
	// 		rslt = 0;
	// }
	// else
	// {
	// 	if (SendTable_psi(ip, outChnId, cat, NULL, 0) != ok)
	// 		rslt = 0;
	// }


	// pbuff = NULL;	
	// list_get(&pclsMux->table_nit, iChn, &pbuff); 
	// psiType = nit;
	// if (pbuff != NULL && pdb->pvalueTree->poutChnArray[iChn].isNeedSend_nit)
	// {
		
	// 	if (SendTable_psi(ip, outChnId, nit, pbuff->pbuf, pbuff->bufLen) != ok)
	// 		rslt = 0;
	// }
	// else
	// {
	// 	if (SendTable_psi(ip, outChnId, nit, NULL, 0) != ok)
	// 		rslt = 0;
	// }
	
	if (SendTable_psi_finish(ip, outChnId) != ok){
		rslt = 0;
		
	}
	else{
		rslt = SendPidMap(ip, outChnId);
	}

	return rslt;
}

int SendPidMap(char *ip, int outChnId)
{
	int rslt = 1;
	printf("call SendPidMap----------------\n");
	if (SendTable_PidMap(ip, outChnId, clsProgram.PrgAVMuxList) != ok)
		rslt = 0;
	// if (DirectlyTransmit_sendMap(ip, outChnId, clsProgram.outPrgList[outChnId - 1].dtPidList) != ok)
	// 	rslt = 0;

	printf("call SendPidMap 2 after send tabli pidmap ----------------\n");
	if (SendEnableMuxTable(ip, outChnId) != ok)
		rslt = 0;

	printf("call SendPidMap 3 after send SendEnableMuxTable----------------\n");
	return rslt;
}

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


void SendMux(char *ip, int outChnId)
{
	unsigned char isNeedDesInfoSend = 1;

	printf("sendMux 1 outChnId=%d\n", outChnId);

    int selCnt = CountSelectedPrgCnt(outChnId);
	//MuxPrgNumMax
	if (selCnt > o_selectedPrgCntMax) 
	{		
		isNeedDesInfoSend = 0;
		return;
	}
	
	printf("XXXXXX=======XXXX=== sendMux fuck 1\n");
    unsigned char sendRslt = AutoMux_makeMuxInfoAndSend(ip, outChnId, !pdb->pvalueTree->poutChnArray[outChnId - 1].isManualMapMode);
    if (!sendRslt)
    {	    
		printf("AutoMux_makeMuxInfoAndSend error rest=%d\n", sendRslt);
	    isNeedDesInfoSend = 0;
	    return;
    }
    
    printf("XXXXXX=======XXXX=== sendMux fuck 2\n");
	
	 SendTable(ip, outChnId);

	printf("XXXXXX=======XXXX=== sendMux fuck 3\n");

	//if (isNeedDesInfoSend)
	{
		//MakeOutputBytesAndSend(ip, outChnId);
	}

	printf("XXXXXX=======XXXX=== sendMux fuck 4\n");
		
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

