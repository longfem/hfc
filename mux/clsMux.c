 
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

	if (clsProgram.chnBypassEnable != 0 && clsProgram.chnBypassEnable[outChnId - 1] &&
		clsProgram.chnBypass2 != 0 && clsProgram.chnBypass2[outChnId - 1] != 0)
	{
		// if (clsProgram.outPrgList[outChnIndex].caNode.caIdenList != null)
		// 	clsProgram.outPrgList[outChnIndex].caNode.caIdenList.Clear();
		// if (clsProgram.outPrgList[outChnIndex].dtPidList != null)
		// 	clsProgram.outPrgList[outChnIndex].dtPidList.Clear();


		// list_get(&(clsProgram.outPrgList), outChnIndex, &outpst);
		// prgNodesLen = list_len(&(outpst->prgNodes));
		// if (prgNodesLen > 0){
		// 	freePrograms(&outpst->prgNodes);			
		// }

		
		// if (clsProgram.outPrgList[outChnIndex].userPrgNodes != null)
		// 	clsProgram.outPrgList[outChnIndex].userPrgNodes.Clear();

		// treeView_in[outChnIndex].Nodes[0].Checked = false;
	}
	pdb->pvalueTree->poutChnArray[outChnIndex].isManualMapMode = 1;	

	rtn = MakeTable(outChnId);
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
	printf("clsMux file SetOutRate=[%d] \n", pdb->pvalueTree->poutChnArray[outChnIndexTmp].outputRate);

//	ErrorTypeEm errRslt = SetOutRate(ip, outChnId, pdb->pvalueTree->poutChnArray[outChnIndexTmp].outputRate);

	
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

	
	SetTableEnableFlag(ip, outChnId, tableEnableFlag);
	
}


// 记录某个输入通道是否有数据要求
// void RecordInputChnUseStatus(int outChnId)
// {
// 	int outChannelIndex = outChnId - 1;
// 	for (int iInput = 0; iInput < _inMaxNum; iInput++)
// 	{
// 		clsProgram.needInputData[outChannelIndex, iInput] = false;
// 	}
// 	if (clsProgram.PrgAVMuxList != null && clsProgram.PrgAVMuxList[outChannelIndex] != null)
// 	{
// 		foreach (MuxPidInfo_st tmpPidInfo in clsProgram.PrgAVMuxList[outChannelIndex])
// 		{
// 			if (tmpPidInfo.inChannel > 0 && tmpPidInfo.inChannel <= clsProgram.needInputData.Length)
// 				clsProgram.needInputData[outChannelIndex, tmpPidInfo.inChannel - 1] = true;
// 		}
// 	}
// 	if (clsProgram.outPrgList[outChannelIndex].dtPidList != null)
// 	{
// 		foreach (MuxPidInfo_st tmpPidInfo in clsProgram.outPrgList[outChannelIndex].dtPidList)
// 		{
// 			if (tmpPidInfo.inChannel > 0 && tmpPidInfo.inChannel <= clsProgram.needInputData.Length)
// 				clsProgram.needInputData[outChannelIndex, tmpPidInfo.inChannel - 1] = true;
// 		}
// 	}
// }

void InputMissShow(int inChnId, int validStatus, cJSON *chjson)
{
    cJSON_AddNumberToObject(chjson, "ch", inChnId);
    if (validStatus == 2)   //error
    {
        cJSON_AddNumberToObject(chjson, "sts", 2);
//        if (!m_alarmDisconnect)
//        {
//            m_alarmDisconnect = true;
//            inputFlagPannel.BackgroundImage = il_warning.Images[0];
//            WarningNotice(DevNoticeCmdEm.disconnect, lang.Get("连接中断"));
//            StatusNotice();
//        }
    }
    else
    {
//        if (m_alarmDisconnect)
//        {
//            m_alarmDisconnect = false;
//            WarningNotice(DevNoticeCmdEm.reconnect, lang.Get("连接恢复"));
//            StatusNotice();
//        }
        if (validStatus == 1)//warning
        {
            cJSON_AddNumberToObject(chjson, "sts", 1);
//            if (!m_alarmInputLoss[inChnId - 1])
//            {
//                m_alarmInputLoss[inChnId - 1] = true;
//                WarningNotice(DevNoticeCmdEm.warning, inChnId.ToString() + lang.Get("通道输入丢失"));
//                inputFlagPannel.BackgroundImage = il_warning.Images[1];
//            }
        }
        else
        {
            cJSON_AddNumberToObject(chjson, "sts", 0);
//            if (m_alarmInputLoss[inChnId - 1])
//            {
//                m_alarmInputLoss[inChnId - 1] = false;
//                WarningNotice(DevNoticeCmdEm.good, inChnId.ToString() + lang.Get("通道输入正常"));
//            }
//            inputFlagPannel.BackgroundImage = il_warning.Images[2];
        }
    }
}

// 当选择某个通道，但通道码率丢失时报警，isValidInputStatus=指示有一个参数是有效的标示
void ShowNeedChnDataButNoInputWarning(int isValidInputStatus, int inputStatus, cJSON *result)
{
    int i = 0;
    cJSON *jsonarray, *chjson;
    cJSON_AddItemToObject(result, "children", jsonarray = cJSON_CreateArray());
    for (i = 0; i < clsProgram._intChannelCntMax; i++)
    {
        cJSON_AddItemToArray(jsonarray,chjson = cJSON_CreateObject());
        if (isValidInputStatus == 0)
        {
            if ((inputStatus & (1 << i)) == 0) // 报警
            {
                InputMissShow(i + 1, 1, chjson);
                continue;
            }
            InputMissShow(i + 1, 0, chjson);
        }
        else
        {
            InputMissShow(i + 1, 2, chjson);
        }
    }
}