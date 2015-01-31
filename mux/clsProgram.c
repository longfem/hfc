
#include <stdio.h>
#include <stdlib.h>
#include "list.h"
#include "datastructdef.h"
#include "communicate.h"
#include "clsProgram.h"

extern ClsProgram_st clsProgram;


initClsProgram(ClsMux muxer, int inChannelNum, int outChannelNum)//, Shawn.WL.WanLongPannel.language.ILanguage lang)
{	
	_intChannelCntMax = inChannelNum;
	_outChannelCntMax = outChannelNum;

	clsProgram.inPrgList = malloc(sizeof(ChannelProgramSt) *_intChannelCntMax);
	for (int i = 0; i < inPrgList.Length; i++)
	{
		inPrgList[i] = new ChannelProgramSt();
		inPrgList[i].channelId = i + 1;
	}

	outPrgList = new ChannelProgramSt[_outChannelCntMax];
	for (int i = 0; i < outPrgList.Length; i++)
	{
		outPrgList[i] = new ChannelProgramSt();
		outPrgList[i].channelId = i + 1;

		outPrgList[i].prgNodes = new ArrayList();
		outPrgList[i].userPrgNodes = new ArrayList();
		outPrgList[i].dtPidList = new ArrayList();
		outPrgList[i].caNode = new Chn_ca_st();
		outPrgList[i].caNode.caIdenList = new ArrayList();
	}
	m_autoMuxStartPid = new int[_outChannelCntMax];
	for (int i = 0; i < m_autoMuxStartPid.Length; i++)
	{
		m_autoMuxStartPid[i] = 0x100;
	}

	nitSection = new ArrayList[_outChannelCntMax];
	for (int i = 0; i < nitSection.Length; i++)
	{
		nitSection[i] = new ArrayList();
	}

	needInputData = new bool[_outChannelCntMax, _intChannelCntMax];

	scramblePrgList = new List<ScramblePrgSt>();
	scramblePrgListV2 = new List<ScramblePrgSt_v2>();
}


unsigned char AutoMux_makeMuxInfoAndSend(int outChannel, unsigned char isNeedSendDevMux)
{
	list_t sendList;
	MuxPrgInfoGet_st *pmtPrg;
	ChannelProgramSt *eachChn = NULL;
	Dev_prgInfo_st *prgInfo = NULL;

	int outPrgListLen = list_len(&clsProgram.outPrgList);
	int prgNodesLen = 0;
	int i=0, j=0;

	if (outPrgListLen > 0 )
	{
		for (i = 0; i < outPrgListLen; i++)
		{
			//sendList.Clear();

			//removeall list

			if (outChannel != 0 && outChannel != i + 1)
				continue;

			
			list_get(&clsProgram.outPrgList, i, &eachChn);

			if(eachChn !=NULL){
				prgNodesLen = list_len(&eachChn->prgNodes);

				for (j = 0; j < prgNodesLen; j++)
				{
					list_get(&eachChn->prgNodes, i, &prgInfo);

					if(prgInfo){
						pmtPrg = malloc(sizeof(MuxPrgInfoGet_st));

						pmtPrg->inChannel = prgInfo->chnId;
						pmtPrg->prgIndex = prgInfo->index;						
						list_append(&sendList, pmtPrg);
					}
											
				}

				// ErrorTypeEm rslt = muxer.SendOutPrgMuxMap(i + 1, &sendList);
				// if (rslt != ErrorTypeEm.ok)
				// 	return false;
			}
			

		}
	}
	return 1;


}


// int MakeOutPutBytes(int outChn, unsigned char *outBytes, unsigned int outlen)
// {
// 	MemoryStream memStream = NULL;
// 	outBytes = NULL;
// 	try
// 	{
// 		memStream = new MemoryStream();
// 		IFormatter formatter = new BinaryFormatter();
// 		formatter.Serialize(memStream, outPrgList[outChn - 1]);
// 		outBytes = memStream.GetBuffer();
// 		memStream.Close();

// 	}
// 	catch 
// 	{
// 		if(memStream != NULL)
// 			memStream.Close();
// 		MessageBox.Show(lang.Get("保存通道信息出错") + "!", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
// 	}
// 	return outBytes.Length;
// 	return 0;
// }

// unsigned char MakeOutputBytesAndSend(int outChn)
// {
// 	unsigned char *tmpBytes;

// 	int sendLen = MakeOutPutBytes(outChn, out tmpBytes);
// 	if (sendLen > 0)
// 	{
// 		if (muxer.SendOutputPrgInfo(outChn, tmpBytes, sendLen))
// 			return true;
// 	}
// 	return false;
// }


unsigned char PrgMuxInfoGet()
{
	//MuxPrgInfoGet_st
	PrgPmtMuxList = malloc(sizeof(list_t) *_outChannelCntMax);
	PrgAVMuxList = malloc(sizeof(list_t) * _outChannelCntMax);

    //PrgPmtMuxListLen
	for (int i = 0; i < _outChannelCntMax; i++)
	{
		if (GetOutProgramMuxMap(i + 1, out PrgPmtMuxList[i]) != ok)
			return false;
		if (GetOutPidMuxMap(i + 1, out PrgAVMuxList[i]) != tok)
			return false;
	}

	if (dglt_showPidMap != null)
	{
		for (int i = 0; i < PrgPmtMuxList.Length; i++)
		{
			if (PrgPmtMuxList[i] != null)
			{
				foreach (MuxPrgInfoGet_st pmtItem in PrgPmtMuxList[i])
				{
					if (inPrgList[pmtItem.inChannel].prgNodes == null)
						continue;
					if (inPrgList[pmtItem.inChannel].prgNodes.Count < pmtItem.prgIndex)
						continue;
					Dev_prgInfo_st tmpPrgInfo = ((Dev_prgInfo_st)(inPrgList[pmtItem.inChannel].prgNodes[pmtItem.prgIndex - 1]));
					int oldPid = tmpPrgInfo.pmtPid;

					dglt_showPidMap(pmtItem.inChannel, i + 1, "0x" + oldPid.ToString("x"),
							"0x" + pmtItem.prgPid.ToString("x"), tmpPrgInfo.prgNum.ToString(),
							pmtItem.prgNum.ToString(), "PMT [" + tmpPrgInfo.prgName + "]");
				}
			}
		}

		for (int i = 0; i < PrgAVMuxList.Length; i++)
		{
			if (PrgAVMuxList[i] != null)
			{
				foreach (MuxPidInfo_st avItem in PrgAVMuxList[i])
				{
					if (inPrgList[avItem.inChannel].prgNodes == null)
						continue;
					int oldPid = avItem.oldPid;

					dglt_showPidMap(avItem.inChannel, i + 1, "0x" + avItem.oldPid.ToString("x"),
									"0x" + avItem.newPid.ToString("x"), "", "", "");
				}
			}
		}
	}

	return true;
}