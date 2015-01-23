
#include <stdio.h>
#include "datastructdef.h"
#include "communicate.h"
#include "clsMuxSend.h"

extern ClsProgram_st clsProgram;
int o_selectedPrgCntMax = 29;

// unsigned char SendTable(int outChnId)
// {
// 	unsigned char rslt = true;
// 	int iChn = outChnId - 1;

// 	if (table_pat[iChn] != null && db.valueTree.outChnArray[iChn].isNeedSend_pat)
// 	{
// 		byte[] sendTable = (byte[])table_pat[iChn];
// 		if (SendTable_psi(outChnId, PsiTableType.pat, sendTable, sendTable.Length) != ErrorTypeEm.ok)
// 			rslt = false;
// 	}
// 	else
// 	{
// 		if (SendTable_psi(outChnId, PsiTableType.pat, null, 0) != ErrorTypeEm.ok)
// 			rslt = false;
// 	}
// 	if (db.valueTree.outChnArray[iChn].isNeedSend_pmt && 
// 		table_pmtList[iChn] != null 
// 		&& table_pmtList[iChn].Count > 0
// 		&& table_pmtList[iChn][0] != null)
// 	{
// 		if (SendTable_pmt(outChnId, table_pmtList[iChn]) != ErrorTypeEm.ok)
// 			rslt = false;
// 	}
// 	else
// 	{
// 		if (SendTable_pmt(outChnId, null) != ErrorTypeEm.ok)
// 			rslt = false;
// 	}
// 	if (table_sdt[iChn] != null && db.valueTree.outChnArray[iChn].isNeedSend_sdt)
// 	{
// 		byte[] sendTable = (byte[])table_sdt[iChn];
// 		if (SendTable_psi(outChnId, PsiTableType.sdt, sendTable, sendTable.Length) != ErrorTypeEm.ok)
// 			rslt = false;
// 	}
// 	else
// 	{
// 		if (SendTable_psi(outChnId, PsiTableType.sdt, null, 0) != ErrorTypeEm.ok)
// 			rslt = false;
// 	}
// 	if (table_cat[iChn] != null && db.valueTree.outChnArray[iChn].isNeedSend_cat)
// 	{
// 		byte[] sendTable = (byte[])table_cat[iChn];
// 		if (SendTable_psi(outChnId, PsiTableType.cat, sendTable, sendTable.Length) != ErrorTypeEm.ok)
// 			rslt = false;
// 	}
// 	else
// 	{
// 		if (SendTable_psi(outChnId, PsiTableType.cat, null, 0) != ErrorTypeEm.ok)
// 			rslt = false;
// 	}
// 	if (table_nit[iChn] != null && db.valueTree.outChnArray[iChn].isNeedSend_nit)
// 	{
// 		byte[] sendTable = (byte[])table_nit[iChn];
// 		if (SendTable_psi(outChnId, PsiTableType.nit, sendTable, sendTable.Length) != ErrorTypeEm.ok)
// 			rslt = false;
// 	}
// 	else
// 	{
// 		if (SendTable_psi(outChnId, PsiTableType.nit, null, 0) != ErrorTypeEm.ok)
// 			rslt = false;
// 	}
// 	//	if (rslt)
// 	{
// 		if (SendTable_psi_finish(outChnId) != ErrorTypeEm.ok)
// 			rslt = false;
// 		else
// 			rslt = SendPidMap(outChnId);
// 	}

	
// 	return rslt;
// }

// unsigned char SendPidMap(int outChnId)
// {
// 	bool rslt = true;

// 	if (SendTable_PidMap(outChnId, clsProgram.PrgAVMuxList) != ErrorTypeEm.ok)
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
    // unsigned char sendRslt = AutoMux_makeMuxInfoAndSend(outChnId, !db.valueTree.outChnArray[outChnId - 1].isManualMapMode);
    // if (!sendRslt)
    // {	    
	   //  isNeedDesInfoSend = 0;
	   //  return;
    // }
	// SendTable(outChnId);
	// if (isNeedDesInfoSend)
	// {
	// 	//MakeOutputBytesAndSend(outChnId);
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

