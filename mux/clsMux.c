 
#include <stdio.h>


#include "datastructdef.h"
#include "list.h"
#include "EnumDef.h"
#include "clsMux.h"

#include "clsMakeTable.h"
#include "clsMuxOutCh.h"

extern ClsProgram_st clsProgram;
extern ClsParams_st *pdb;
unsigned int CreateTable(int outChnId)
{
	unsigned int rtn = 0;
	int prgNodesLen=0;
	int outChnIndex = outChnId - 1;
	ChannelProgramSt *outpst = NULL;

	printf("hello1\n");

	if (clsProgram.chnBypassEnable != 0 && clsProgram.chnBypassEnable[outChnId - 1] &&
		clsProgram.chnBypass2 != 0 && clsProgram.chnBypass2[outChnId - 1] != 0)
	{
		// if (clsProgram.outPrgList[outChnIndex].caNode.caIdenList != null)
		// 	clsProgram.outPrgList[outChnIndex].caNode.caIdenList.Clear();
		// if (clsProgram.outPrgList[outChnIndex].dtPidList != null)
		// 	clsProgram.outPrgList[outChnIndex].dtPidList.Clear();

		printf("hello2\n");
		list_get(&(clsProgram.outPrgList), outChnIndex, &outpst);
		prgNodesLen = list_len(&(outpst->prgNodes));
		if (prgNodesLen > 0){
			freePrograms(&outpst->prgNodes);			
		}
		// if (clsProgram.outPrgList[outChnIndex].userPrgNodes != null)
		// 	clsProgram.outPrgList[outChnIndex].userPrgNodes.Clear();

		// treeView_in[outChnIndex].Nodes[0].Checked = false;
	}
	pdb->pvalueTree->poutChnArray[outChnIndex].isManualMapMode = 1;	

	printf("hello3\n");
	rtn = MakeTable(outChnId);
	printf("hello4\n");
	//OutPsiTable_show(outChnIndex);			

	return rtn;
}


int sendOutPutMuxInfo(char *ip, int outChnId){
	//------ 设置输出通道信息 -----		
		int outChnIndexTmp = outChnId - 1;        

		ChnBypass_write(ip, outChnId); // 发送直传标志
		printf("(sendOutPutMuxInfo outChnId =%d)\n", outChnId);

		SendMux(ip, outChnId); // 发送表复用信息
		
		SetOutChnTSID(ip, outChnId, pdb->pvalueTree->poutChnArray[outChnIndexTmp].streamId);
		SetOutChnNetID(ip, outChnId, pdb->pvalueTree->poutChnArray[outChnIndexTmp].networkId);
		SetOutChnOrgNetID(ip, outChnId, pdb->pvalueTree->poutChnArray[outChnIndexTmp].oringal_networkid);
		SetOutChnTableVer(ip, outChnId, pdb->pvalueTree->poutChnArray[outChnIndexTmp].version);

		// if (dgltCasSend != null) // CAS发送
		// {
		// 	dgltCasSend(ip);
		// }


		// m_isLoadFileDb = 0;

		return 0;
}

int sendOutPutOption(char *ip, int outChnId)
{
	int outChnIndexTmp = outChnId - 1;	
	ErrorTypeEm errRslt = SetOutRate(outChnId, pdb->pvalueTree->poutChnArray[outChnIndexTmp].outputRate);	

	printf("fuck 6\n");
	unsigned int tableEnableFlag = 0;
	if (pdb->pvalueTree->poutChnArray[outChnIndexTmp].isNeedSend_pat)
		tableEnableFlag |= 0x1;
	if (pdb->pvalueTree->poutChnArray[outChnIndexTmp].isNeedSend_pmt)
		tableEnableFlag |= 0x2;
	if (pdb->pvalueTree->poutChnArray[outChnIndexTmp].isNeedSend_sdt)
		tableEnableFlag |= 0x4;
	if (pdb->pvalueTree->poutChnArray[outChnIndexTmp].isNeedSend_cat)
		tableEnableFlag |= 0x8;
	if (pdb->pvalueTree->poutChnArray[outChnIndexTmp].isNeedSend_nit)
		tableEnableFlag |= 0x10;

	printf("fuck 7\n");
	SetTableEnableFlag(ip, outChnIndexTmp, tableEnableFlag);

	printf("fuck 8\n");
}