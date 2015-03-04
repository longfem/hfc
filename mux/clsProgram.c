
#include <stdio.h>
#include <stdlib.h>
#include "list.h"
#include "EnumDef.h"
#include "datastructdef.h"
#include "communicate.h"
#include "clsProgram.h"
#include "clsMuxprgInfoGet.h"

extern ClsProgram_st clsProgram;


void initClsProgram(int inChannelNum, int outChannelNum)//, Shawn.WL.WanLongPannel.language.ILanguage lang)
{	
	// _intChannelCntMax = inChannelNum;
	// _outChannelCntMax = outChannelNum;

	// clsProgram.inPrgList = malloc(sizeof(ChannelProgramSt) *_intChannelCntMax);
	// for (int i = 0; i < inPrgList.Length; i++)
	// {
	// 	inPrgList[i] = new ChannelProgramSt();
	// 	inPrgList[i].channelId = i + 1;
	// }

	// outPrgList = new ChannelProgramSt[_outChannelCntMax];
	// for (int i = 0; i < outPrgList.Length; i++)
	// {
	// 	outPrgList[i] = new ChannelProgramSt();
	// 	outPrgList[i].channelId = i + 1;

	// 	outPrgList[i].prgNodes = new ArrayList();
	// 	outPrgList[i].userPrgNodes = new ArrayList();
	// 	outPrgList[i].dtPidList = new ArrayList();
	// 	outPrgList[i].caNode = new Chn_ca_st();
	// 	outPrgList[i].caNode.caIdenList = new ArrayList();
	// }
	// m_autoMuxStartPid = new int[_outChannelCntMax];
	// for (int i = 0; i < m_autoMuxStartPid.Length; i++)
	// {
	// 	m_autoMuxStartPid[i] = 0x100;
	// }

	// nitSection = new ArrayList[_outChannelCntMax];
	// for (int i = 0; i < nitSection.Length; i++)
	// {
	// 	nitSection[i] = new ArrayList();
	// }

	// needInputData = new bool[_outChannelCntMax, _intChannelCntMax];

	// scramblePrgList = new List<ScramblePrgSt>();
	// scramblePrgListV2 = new List<ScramblePrgSt_v2>();
}


unsigned char AutoMux_makeMuxInfoAndSend(char *ip, int outChannel, unsigned char isNeedSendDevMux)
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

void cSerialize(Dev_prgInfo_st *proginfo, unsigned char * pOutbuf, int *outLen)
{
	unsigned char *pBuf = NULL, pTemp= NULL;
	int i=0, j=0, bufLen=0;
	if(proginfo == NULL){
		printf("proginfo NULL \n");
		return -1;
	}

	//compute need malloc buf len
	bufLen += sizeof(Dev_prgInfo_st); // struct sizeof(Dev_prgInfo_st) 	
	bufLen += proginfo->pmtDesListLen;   //pmtDesListLen
	

	for(i=0; i< proginfo->pmtDesListLen; i++){
		bufLen += sizeof(Commdes_t);
		bufLen += proginfo->pmtDesList->dataLen;
	}

	bufLen += 4;   //pdataStreamListLen;	 

	for(i=0;  i < proginfo->pdataStreamListLen; i++){
		bufLen += sizeof(DataStream_t);

		for(j=0; j< proginfo->pdataStreamList->destlen; j++){
			bufLen += sizeof(Commdes_t);
			bufLen += proginfo->pdataStreamList->desNode->dataLen;
		}
	}

	bufLen+= proginfo->prgNameLen; //
	bufLen+= proginfo->providerNameLen; //
	bufLen +=4; // Crc 32;
	
	//malloc now
	pBuf = malloc(bufLen);

	//copy now	
	//copy pmt
	pTemp = pBuf;
	memcpy(pBuf, proginfo, sizeof(Dev_prgInfo_st));
	pTemp += sizeof(Dev_prgInfo_st);	
	memcpy(pTemp, (unsigned char *)&proginfo->pmtDesListLen, 4 );
	pTemp +=4;

	

	for(i=0; i< proginfo->pmtDesListLen; i++){
		memcpy(pTemp, proginfo->pmtDesList + i, sizeof(Commdes_t));
		pTemp += sizeof(Commdes_t);

		memcpy(pTemp, proginfo->pmtDesList->data, proginfo->pmtDesList->dataLen);				
		pTemp+=	proginfo->pmtDesList->dataLen;
	}

	//copy datastream
	memcpy(pTemp, (unsigned char *)&proginfo->pdataStreamListLen, 4 );
	pTemp +=4;
	for(i=0 ; i < proginfo->pdataStreamListLen; i++){
		memcpy(pTemp, &proginfo->pdataStreamList[i], sizeof(DataStream_t));
		pTemp += sizeof(DataStream_t);
		for(j=0; j< proginfo->pdataStreamList[i].destlen; j++){
			memcpy(pTemp, &proginfo->pdataStreamList[i].desNode[j], sizeof(Commdes_t));
			pTemp += sizeof(Commdes_t);
			memcpy(pTemp, proginfo->pdataStreamList[i].desNode[j].data, proginfo->pdataStreamList[i].desNode[j].dataLen);
			pTemp += proginfo->pdataStreamList[i].desNode[j].dataLen;
		}
	}	

	//prgName
	if(proginfo->prgNameLen > 0){
		memcpy(pTemp, proginfo->prgName, proginfo->prgNameLen);
		pTemp += 4;	
	}
	
	//providerName    
	if(proginfo->providerNameLen > 0){
		memcpy(pTemp, proginfo->providerName, proginfo->providerNameLen);
		pTemp += 4;		
	}
	
	//psdt


	if(pBuf != NULL){
		pOutbuf = pBuf;
		*outLen = bufLen;
	} 
	else{
		pOutbuf = NULL;
		*outLen = 0;
	}       
}

int  cDeSerialize(unsigned char * pInbuf, int inLen, Dev_prgInfo_st *proginfo)
{
	Dev_prgInfo_st *pBuf = NULL;
	unsigned char *pTemp = NULL ;
	int i=0, j=0, bufLen=0;
	if(pInbuf == NULL || inLen < sizeof(Dev_prgInfo_st) + 4){
		printf("proginfo NULL or len too small\n");
		return NULL;
	}

	pBuf = malloc(sizeof(Dev_prgInfo_st));
	pTemp = pInbuf;
	memcpy(pBuf, pTemp, sizeof(Dev_prgInfo_st));	
	pTemp += sizeof(Dev_prgInfo_st);

	int pmtDesListLen = 0;
	memcpy(&pmtDesListLen, pInbuf, 4 );
	pTemp +=4;

	pBuf->pmtDesList = malloc(sizeof(Commdes_t) * pmtDesListLen);
	
	for(i=0; i< proginfo->pmtDesListLen; i++){
		memcpy(&proginfo->pmtDesList[i], pTemp, sizeof(Commdes_t));		
		pTemp += sizeof(Commdes_t);
		memcpy(proginfo->pmtDesList[i].data, pTemp, proginfo->pmtDesList[i].dataLen);				
		pTemp+=	proginfo->pmtDesList[i].dataLen;		
	}

	//deserialize datastream
	int pdataStreamListLen=0;
	memcpy(&pdataStreamListLen, pTemp, 4 );
	pTemp +=4;

	if(pdataStreamListLen > 0){
		for(i=0 ; i < proginfo->pdataStreamListLen; i++){
			memcpy(&proginfo->pdataStreamList[i], pTemp, sizeof(DataStream_t));
			pTemp += sizeof(DataStream_t);
			for(j=0; j< proginfo->pdataStreamList[i].destlen; j++){
				memcpy(&proginfo->pdataStreamList[i].desNode[j], pTemp, sizeof(Commdes_t));
				pTemp += sizeof(Commdes_t);
				memcpy(proginfo->pdataStreamList[i].desNode[j].data, pTemp, proginfo->pdataStreamList[i].desNode[j].dataLen);
				pTemp += proginfo->pdataStreamList[i].desNode[j].dataLen;
			}
		}	
	}else{
		proginfo->pdataStreamListLen = 0;
		proginfo->pdataStreamList = NULL;
	}
		
	if(proginfo->prgNameLen > 0){	
		memcpy(proginfo->prgName, pTemp, proginfo->prgNameLen);
		pTemp += 4;
	}

	//providerName 
	if(proginfo->providerNameLen > 0){   
		memcpy(proginfo->providerName, pTemp, proginfo->providerNameLen);
		pTemp += 4;	
	}	


	return pBuf;	

}

int MakeOutPutBytes(int outChn, unsigned char *outBytes, int *outLen)
{
	
	Dev_prgInfo_st * proginfo = NULL;
	outBytes = NULL;
	
	list_get(&clsProgram.outPrgList, outChn - 1, &proginfo);


	cSerialize(proginfo, outBytes, outLen);
	return outLen;
}


unsigned char MakeOutputBytesAndSend(char *ip, int outChn)
{
	unsigned char *tmpBytes;
	unsigned int tmpBytesLen=0;

	int sendLen = MakeOutPutBytes(outChn,  tmpBytes, &tmpBytesLen);
	if (sendLen > 0)
	{
		if (SendOutputPrgInfo(ip, outChn, tmpBytes, sendLen))
			return 1;
	}
	return 0;
}


unsigned char PrgMuxInfoGet(char *ip)
{

	int i=0;
	//MuxPrgInfoGet_st list array
	//list_t *PrgPmtMuxList = NULL;
	//MuxPidInfo_st list array
	//PrgAVMuxList

	ErrorTypeEm res;
  
  	list_t *PrgPmtMux = NULL;
  	list_t   *prgAVMux = NULL;
	for (i = 0; i < clsProgram._outChannelCntMax; i++)
	{	
		 	 
		if (GetOutProgramMuxMap(ip, i + 1, clsProgram.PrgPmtMuxList[i]) != ok)
			return 0;

		
		
		if (GetOutPidMuxMap(ip, i + 1, clsProgram.PrgAVMuxList[i]) != ok)
			return 0;
	}

	// if (dglt_showPidMap != null)
	// {
	// 	for (int i = 0; i < PrgPmtMuxList.Length; i++)
	// 	{
	// 		if (PrgPmtMuxList[i] != null)
	// 		{
	// 			foreach (MuxPrgInfoGet_st pmtItem in PrgPmtMuxList[i])
	// 			{
	// 				if (inPrgList[pmtItem.inChannel].prgNodes == null)
	// 					continue;
	// 				if (inPrgList[pmtItem.inChannel].prgNodes.Count < pmtItem.prgIndex)
	// 					continue;
	// 				Dev_prgInfo_st tmpPrgInfo = ((Dev_prgInfo_st)(inPrgList[pmtItem.inChannel].prgNodes[pmtItem.prgIndex - 1]));
	// 				int oldPid = tmpPrgInfo.pmtPid;

	// 				dglt_showPidMap(pmtItem.inChannel, i + 1, "0x" + oldPid.ToString("x"),
	// 						"0x" + pmtItem.prgPid.ToString("x"), tmpPrgInfo.prgNum.ToString(),
	// 						pmtItem.prgNum.ToString(), "PMT [" + tmpPrgInfo.prgName + "]");
	// 			}
	// 		}
	// 	}

	// 	for (int i = 0; i < PrgAVMuxList.Length; i++)
	// 	{
	// 		if (PrgAVMuxList[i] != null)
	// 		{
	// 			foreach (MuxPidInfo_st avItem in PrgAVMuxList[i])
	// 			{
	// 				if (inPrgList[avItem.inChannel].prgNodes == null)
	// 					continue;
	// 				int oldPid = avItem.oldPid;

	// 				dglt_showPidMap(avItem.inChannel, i + 1, "0x" + avItem.oldPid.ToString("x"),
	// 								"0x" + avItem.newPid.ToString("x"), "", "", "");
	// 			}
	// 		}
	// 	}
	// }

	return 1;
}