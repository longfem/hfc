
#include <stdio.h>
#include <stdlib.h>
#include "list.h"
#include "EnumDef.h"
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

	list_init(&sendList);
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

				 ErrorTypeEm rslt = SendOutPrgMuxMap(ip, i + 1, &sendList);
				 if (rslt != ok)
				 	return 0;
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


int NewProgramNum(int curNum, list_t prgNumList)
{
	int iNewPrgNum = -1;
	int isRepeat = 0;
	int i;

	int *prgNumTmp;
	for (i= 0; i < list_len(&prgNumList); i++)
	{

		list_get(&prgNumList,i,&prgNumTmp);
		if (*prgNumTmp == curNum)
		{
			isRepeat = 1;
			break;
		}
	}
	if (isRepeat==0)
	{
		list_append(&prgNumList,&curNum);
		return curNum;
	}

	for (i = 1; i < 0xffff; i++)
	{
		isRepeat = 0;
		for (i= 0; i < list_len(&prgNumList); i++)
		{
			list_get(&prgNumList,i,&prgNumTmp);
			if (*prgNumTmp == i)
			{
				isRepeat = 1;
				break;
			}

		}
		if (!isRepeat)
		{
			iNewPrgNum = i;
			list_append(&prgNumList,&iNewPrgNum);
			break;
		}
	}
	return iNewPrgNum;
}


int NewPid(int inChn, int oldPid, int curPid, int pidOffset, list_t usedPidTree, unsigned char isUsedPid[] , int isUsedPidLenth, int  repeatIsOk)// replacePidst
{
	int newPid = -1;
	int i;
	replacePidst *oldNodeTmp;

	// 检查此类输入PID是否已经有被映射过
	if (oldPid != 0x1fff && oldPid != -1)
	{
		for (i = 0; i <list_len(&usedPidTree); i++)
		{

			list_get(&usedPidTree, i, &oldNodeTmp);
			if (oldNodeTmp->oldPid == oldPid)
			{
				return oldNodeTmp->newPid;
			}

		}
	}
	// 还未被占用过的PID
	if (isUsedPid[curPid] == 0)
	{
		newPid = curPid;
	}
	else
	{
		if (repeatIsOk && oldPid != 0x1fff && oldPid != -1)
		{
			// 是否是被本通道占用
			//foreach (replacePidst oldNodeTmp in usedPidTree[inChn - 1])
			for (i = 0; i <list_len(&usedPidTree); i++)
			{

				list_get(&usedPidTree, i, &oldNodeTmp);
				if (oldNodeTmp->oldPid == oldPid)
				{
					return oldNodeTmp->newPid;
				}
			}

		}
	}
	// 重新分配PID
	if (newPid < 0)
	{
		for (i = pidOffset; i < isUsedPidLenth; i++)
		{
			if (isUsedPid[i]==0)
			{
				newPid = i;
				isUsedPid[i] = 1;
				break;
			}
		}
	}
	replacePidst *newPidNode = (replacePidst*)malloc(sizeof(replacePidst));
	newPidNode->oldPid = oldPid;
	newPidNode->newPid = newPid;
	isUsedPid[newPid] = 1;
	list_append(&usedPidTree,newPidNode);

	return newPid;
}

int DesPidRefresh(int inChn, int prgIndex, int avIndex,Commdes_t *desList,int desListLen,int pidOffset, unsigned char isUsedPid[],list_t usedPidTree)
{
#if 1
	int i;
	int newPid = 0x1fff;
	//bool isNeedSeekBefore = true;
	Commdes_t *desInfoTmp=desList;
	for (i = 0; i < desListLen; i++)
	{

	}
#endif
	return 1;
}

int AutoMakeNewPid(int outChannel)
{

	//printf("get int AutoMakeNewPid\n");

	int i;
	int j;
	int iTmp;
	int iDs;

	int pidPrgStart = clsProgram.prgPid_min;
	int pidAvStart = clsProgram.subPrgPid_min;


	int isUsedPidLenth=0x2000;
	unsigned char isUsedPid[isUsedPidLenth];
	list_t newUsedPidList;// replacePidst + 1自增通道

	int  newUsedPidListLen=clsProgram._intChannelCntMax + 1;
	list_t prgNumList;

	list_init(&newUsedPidList);


	for (i = 0; i < newUsedPidListLen; i++)
	{
		list_t tmpList;
		list_init(&tmpList);
		list_append(&newUsedPidList,&tmpList);
	}

	for (i = 0; i < clsProgram._outChannelCntMax; i++)
	{
		if (outChannel == 0 || outChannel == i + 1)
		{

			for (j = 0; j < isUsedPidLenth; i++)
			{
				isUsedPid[j]=0;
			}
			for (iTmp = 0; iTmp < newUsedPidListLen; iTmp++)
			{
				list_t *tmpList;
				list_get(&newUsedPidList, iTmp, &tmpList);
				while(list_len(tmpList))
				{
					list_pop_tail(tmpList);
				}
			}

			while(list_len(&prgNumList))
			{
				list_pop_tail(&prgNumList);
			}
			ChannelProgramSt *outpst;
			list_get(&clsProgram.outPrgList, i, &outpst);
			//如果存在节目
			if (list_len(&outpst->prgNodes)>0)
			{
				for (j = 0; j < list_len(&outpst->prgNodes); j++)
				{
					unsigned char  isNewPrgPid = 0;
					//取出节目结构体
					Dev_prgInfo_st *prgInfoTmp;
					list_get(&outpst->prgNodes,j,&prgInfoTmp);
					int newPid;

					//if (prgInfoTmp.chnId > 0 && prgInfoTmp.chnId <= _outChannelCntMax)
					{
						newPid = NewPid(prgInfoTmp->chnId, 0x1fff, prgInfoTmp->pmtPid, pidPrgStart, newUsedPidList, isUsedPid,isUsedPidLenth,0);
						//如果pmtpid发生改变需要变新
						if (prgInfoTmp->pmtPid != newPid)
						{
							prgInfoTmp->pmtPid = newPid;
							isNewPrgPid = 1;
						}

						if (prgInfoTmp->newPcrPid != 0x1fff)
						{
							newPid = NewPid(prgInfoTmp->chnId, prgInfoTmp->oldPcrPid, prgInfoTmp->newPcrPid, pidAvStart, newUsedPidList, isUsedPid, isUsedPidLenth,1);
							if (prgInfoTmp->newPcrPid != newPid)
							{
								prgInfoTmp->newPcrPid = newPid;
								isNewPrgPid = 1;
							}
						}

						//Commdes_st *tmpdesList=prgInfoTmp->psdtDesList;
						//	tmpdesList+=j;
						DesPidRefresh(prgInfoTmp->chnId, prgInfoTmp->index, -1,
							prgInfoTmp->pmtDesList,prgInfoTmp->psdtDesListLen, pidAvStart, isUsedPid, newUsedPidList);

						int newPrgNum = NewProgramNum(prgInfoTmp->prgNum, prgNumList);
						if (newPrgNum < 0)
							return 0;
						if (isNewPrgPid || newPrgNum != prgInfoTmp->prgNum)
						{
							//prgInfoTmp->prgNum = newPrgNum;
							//outPrgList[i].prgNodes.RemoveAt(j);
							//outPrgList[i].prgNodes.Insert(j, prgInfoTmp);
						}

						//如果该节目下存在数据流
						for (iDs = 0; iDs < prgInfoTmp->pdataStreamListLen; iDs++)
						{
							DataStream_t *dsInfoTmp = prgInfoTmp->pdataStreamList;
							newPid = NewPid(prgInfoTmp->chnId, dsInfoTmp->inPid, dsInfoTmp->outPid, pidAvStart, newUsedPidList, isUsedPid,isUsedPidLenth, 1);
							if (dsInfoTmp->outPid != newPid)
							{
								//isNewPrgPid = true;
								//dsInfoTmp.outPid = newPid;
								//((Dev_prgInfo_st)(outPrgList[i].prgNodes[j])).dataStreamList.RemoveAt(iDs);
								//((Dev_prgInfo_st)(outPrgList[i].prgNodes[j])).dataStreamList.Insert(iDs, dsInfoTmp);
							}
							DesPidRefresh(prgInfoTmp->chnId, prgInfoTmp->index, dsInfoTmp->index,
								dsInfoTmp->desNode,dsInfoTmp->desNodeLen, pidAvStart,isUsedPid, newUsedPidList);
						}

					}
				}
			}
			//	if (outPrgList[i].caNode.caIdenList != null)
			if(0)
			{


			}

			if (0)
			{

			}
			else
			{}
		}
	}
	return 1;
}



int MakePidMapTable(int outChannel,list_t  prginfolist)
{
	int i;
	int j;
	int k;
	int l;
	int m,n,o,oprgPidInfo;

	int isAddedPidLenth=0x2000;
	unsigned char isAddedPid[isAddedPidLenth];
	for ( i = 0; i < isAddedPidLenth; i++)
	{
		isAddedPid[i]=0;
	}

	int lastThisPidMapToNewPid = -1;


	ChannelProgramSt *inpst;
	Dev_prgInfo_st *inPrgInfoTmp;
	for ( i = 0; i <clsProgram._outChannelCntMax; i++)
	{
		if (outChannel != 0 && (outChannel != i + 1))
			continue;
		int  isNoticeSamePid = 1;
		MuxPidInfo_st *pidMapTmp;

		//for test ,西贡弄好，需要关掿

		#if 0
			
		list_t *PrgAVMuxList=(list_t*)malloc(sizeof(list_t));
		list_init(PrgAVMuxList);
		if (list_len(PrgAVMuxList)==0)
		{
			for ( j = 0; j < clsProgram._outChannelCntMax; j++)
			{
				list_t *table_pmt=(list_t*)malloc(sizeof(list_t));
				list_init(table_pmt);
				list_append(PrgAVMuxList,table_pmt);
			}
		}

		//子数组下的元素清穿
		list_t *PrgAVMuxListI;
		list_get(PrgAVMuxList,i,&PrgAVMuxListI);
		if(list_len(PrgAVMuxListI)>0)//清空
		{
			while(list_len(PrgAVMuxListI))
			{
				list_pop_tail(PrgAVMuxListI);
			}
		}

		#endif

	//list_t *PrgAVMuxList=(list_t*)malloc(sizeof(list_t));
	//list_init(PrgAVMuxList);
		
    	list_t *PrgAVMuxListI = (list_t*)malloc(sizeof(list_t));
  		list_init(PrgAVMuxListI);  
		
        freeMuxPrgInfoList(clsProgram.PrgAVMuxList[i]);		
	#if 0  
		//ȥࠕclsProgram.PrgAVMuxList[i]		
		if(clsProgram.PrgAVMuxList[i] != NULL)
		{
			//list_t *tmpAVMuxList=clsProgram.PrgAVMuxList[i];
			while(list_len(clsProgram.PrgAVMuxList[i]))
			{
			    MuxPidInfo_st  *outPMTBuffer;
				list_get(clsProgram.PrgAVMuxList[i],list_len(clsProgram.PrgAVMuxList[i])-1,&outPMTBuffer);
				free(outPMTBuffer);
				list_pop_tail(clsProgram.PrgAVMuxList[i]);	
			}
		}

		#endif
		clsProgram.PrgAVMuxList[i] = NULL;

		//printPrgAVMuxList();
		// ChannelProgramSt *outpst = NULL;
		//list_get(&(clsProgram.outPrgList), i, &outpst);
		//for (j = 0; j < list_len(&outpst->prgNodes); j++)
		//for test
		for (j = 0; j <  list_len(&prginfolist); j++)
		{
			Dev_prgInfo_st *outPrgInfoTmp;
			//list_get(&outpst->prgNodes,j,&outPrgInfoTmp);
			list_get(&prginfolist,j,&outPrgInfoTmp);
			//printf(" outPrgInfoTmp->newPcrPid %d\n",outPrgInfoTmp->newPcrPid);
			if (outPrgInfoTmp->newPcrPid != 0x1fff)
			{
				//printf(" isAddedPid[outPrgInfoTmp->newPcrPid] %d\n",isAddedPid[outPrgInfoTmp->newPcrPid]);
				if (isAddedPid[outPrgInfoTmp->newPcrPid]==0)
				{
					isAddedPid[outPrgInfoTmp->newPcrPid] = 1;

					pidMapTmp =(MuxPidInfo_st*)malloc(sizeof(MuxPidInfo_st));
					pidMapTmp->inChannel = outPrgInfoTmp->chnId;
					pidMapTmp->oldPid = outPrgInfoTmp->oldPcrPid;
					pidMapTmp->newPid = outPrgInfoTmp->newPcrPid;
					list_append(PrgAVMuxListI,pidMapTmp);
				}
			}


			//		printPrgAVMuxList();

			if (outPrgInfoTmp->pmtDesListLen>0)
			{
				//foreach (Commdes_st outDesInfoTmp in outPrgInfoTmp.pmtDesList)
				Commdes_t *outDesInfoTmp = outPrgInfoTmp->pmtDesList;
				for (k = 0; k < outPrgInfoTmp->pmtDesListLen; k++)
				{

					if (outDesInfoTmp->tag == 0x9 && (outDesInfoTmp->userNew!=0))
					{
						int oldPid = 0x1fff;
						int isBreakDesSeek = 0;


					
						list_get(&(clsProgram.inPrgList), outPrgInfoTmp->chnId - 1, &inpst);
					
						
						for (k = 0; k < list_len(&inpst->prgNodes); k++)
						{
							list_get(&prginfolist,k,&inPrgInfoTmp);//这里我先直接从输出中取，测试for
							if (inPrgInfoTmp->index != outPrgInfoTmp->index)
								continue;

							Commdes_t *inDesInfoTmp=inPrgInfoTmp->pmtDesList;
							for (l = 0; l < inPrgInfoTmp->pmtDesListLen; l++)
							{
								if (inDesInfoTmp->index != outDesInfoTmp->index)
									continue;
								unsigned char *data=inDesInfoTmp->data;
								data+=2;
								oldPid = (*data << 8) & 0x1f00;
								data+=1;
								oldPid |= *data;
								isBreakDesSeek = 1;
								break;
							}
							if (isBreakDesSeek==1)
								break;
						}
						if (oldPid == 0x1fff)
							continue;

						unsigned char *outdata=outDesInfoTmp->data;
						outdata+=2;
						int pid = (*outdata<< 8) & 0x1f00;
						outdata+=1;
						pid |= *outdata;

						// 查找是否有替换过此PID
						lastThisPidMapToNewPid = -1;
						for (k= 0; k < list_len(&PrgAVMuxListI); k++)
						{

							MuxPidInfo_st *lastPidInfoTmp;
							list_get(&PrgAVMuxListI,k,&lastPidInfoTmp);

							if (lastPidInfoTmp->oldPid == oldPid && lastPidInfoTmp->inChannel == outPrgInfoTmp->chnId)
							{
								lastThisPidMapToNewPid = lastPidInfoTmp->newPid;
							}
						}
						if (lastThisPidMapToNewPid != -1)
						{
							if (pid != lastThisPidMapToNewPid)
							{
								if (isNoticeSamePid)
								{
									//这里我直接终歿
									return 0;
								}
								outdata=outDesInfoTmp->data;
								outdata+=2;
								*outdata&= 0xe0;
								*outdata|= (unsigned char)((lastThisPidMapToNewPid >> 8) & 0x1f);
								outdata+=1;
								*outdata |= (unsigned char)(lastThisPidMapToNewPid);
								pid = lastThisPidMapToNewPid;
							}
						}
						// 添加映射信息
						if (isAddedPid[pid]==0)
						{
							isAddedPid[pid]=1;
							pidMapTmp =(MuxPidInfo_st*)malloc(sizeof(MuxPidInfo_st));
							pidMapTmp->inChannel = outPrgInfoTmp->chnId;
							pidMapTmp->oldPid = oldPid;
							pidMapTmp->newPid = pid;
							list_append(PrgAVMuxListI,pidMapTmp);
						}
					}
					outDesInfoTmp++;
				}
			}


		//	printPrgAVMuxList();


			DataStream_t *DataStream_stTmp = outPrgInfoTmp->pdataStreamList;
			for (k = 0; k < outPrgInfoTmp->pdataStreamListLen; k++)
			{

				// 查找是否有替换过此PID
				lastThisPidMapToNewPid = -1;
				for (  l= 0; l < list_len(PrgAVMuxListI); l++)
				{
					MuxPidInfo_st *lastPidInfoTmp;
					list_get(PrgAVMuxListI,l,&lastPidInfoTmp);

					if(lastPidInfoTmp->oldPid == DataStream_stTmp->inPid && lastPidInfoTmp->inChannel == outPrgInfoTmp->chnId)
					{
						lastThisPidMapToNewPid = lastPidInfoTmp->newPid;
						break;
					}

				}

				if (lastThisPidMapToNewPid != -1)
				{
					if (DataStream_stTmp->outPid != lastThisPidMapToNewPid)
					{
						if (isNoticeSamePid==1)
						{
							//这里我直接终歿
							return 0;
						}

						DataStream_stTmp->outPid = lastThisPidMapToNewPid;

					}
				}
				// 添加映射信息
				if (isAddedPid[DataStream_stTmp->outPid]==0)
				{
					isAddedPid[DataStream_stTmp->outPid] = 1;
					pidMapTmp =(MuxPidInfo_st*)malloc(sizeof(MuxPidInfo_st));
					pidMapTmp->inChannel = outPrgInfoTmp->chnId;
					pidMapTmp->oldPid = DataStream_stTmp->inPid;
					pidMapTmp->newPid = DataStream_stTmp->outPid;
					list_append(PrgAVMuxListI,pidMapTmp);
				}

				int isBreakDesSeek = 0;
				Commdes_t *outDesInfoTmp = DataStream_stTmp->desNode;
				for (l = 0; l < DataStream_stTmp->desNodeLen; l++)//
				{
					if (outDesInfoTmp->tag == 0x9 && !outDesInfoTmp->userNew)
					{
						int oldPid = 0x1fff;

	
						list_get(&(clsProgram.inPrgList), outPrgInfoTmp->chnId - 1, &inpst);


						for (m = 0; m < 1; m++)//list inprg list
						{
							list_get(&inpst->prgNodes,m,&inPrgInfoTmp);	
							if (inPrgInfoTmp->index != outPrgInfoTmp->index)
								continue;

							DataStream_t *DataStream_stTmpTmp=inPrgInfoTmp->pdataStreamList;

							for (n = 0; n < inPrgInfoTmp->pdataStreamListLen; n++)//list datastreamlist of inprglist
							{
								if (DataStream_stTmpTmp->index != DataStream_stTmp->index)
									continue;

								Commdes_t *inDesInfoTmp=DataStream_stTmpTmp->desNode;

								for (o = 0; o < DataStream_stTmpTmp->desNodeLen; o++)//list datastreamlist of inprglist
								{
									if (inDesInfoTmp->index != outDesInfoTmp->index)
										continue;
									unsigned char *outdata=inDesInfoTmp->data;
									outdata+=2;
									oldPid= (*outdata<< 8) & 0x1f00;
									outdata+=1;
									oldPid |= *outdata;
									isBreakDesSeek=1;
									break;
								}
								if (isBreakDesSeek==1)
									break;

							}

							if (isBreakDesSeek)
								break;
						}
						if (oldPid == 0x1fff)
							continue;

						unsigned char *outdata=outDesInfoTmp->data;
						outdata+=2;
						int pid = (*outdata<< 8) & 0x1f00;
						outdata+=1;
						pid |= *outdata;

						// 查找是否有替换过此PID
						lastThisPidMapToNewPid = -1;

						for (oprgPidInfo= 0; oprgPidInfo < list_len(&PrgAVMuxListI); oprgPidInfo++)
						{

							MuxPidInfo_st *lastPidInfoTmp;
							list_get(&PrgAVMuxListI,oprgPidInfo,&lastPidInfoTmp);

							if (lastPidInfoTmp->oldPid == oldPid && lastPidInfoTmp->inChannel == outPrgInfoTmp->chnId)
							{
								lastThisPidMapToNewPid = lastPidInfoTmp->newPid;
							}
						}

						if (lastThisPidMapToNewPid != -1)
						{
							if (pid != lastThisPidMapToNewPid)
							{
								if (isNoticeSamePid)
								{
									return 0;
								}

								outdata=outDesInfoTmp->data;
								outdata+=2;
								*outdata&= 0xe0;
								*outdata|= (unsigned char)((lastThisPidMapToNewPid >> 8) & 0x1f);
								outdata+=1;
								*outdata |= (unsigned char)(lastThisPidMapToNewPid);
								pid = lastThisPidMapToNewPid;
							}
						}

						// 添加映射信息
						if (isAddedPid[pid]==0)
						{
							isAddedPid[pid] = 1;
							pidMapTmp =(MuxPidInfo_st*)malloc(sizeof(MuxPidInfo_st));
							pidMapTmp->inChannel = outPrgInfoTmp->chnId;
							pidMapTmp->oldPid = oldPid;
							pidMapTmp->newPid = pid;
							list_append(PrgAVMuxListI, pidMapTmp);
						}
					}
				}
				DataStream_stTmp++;
			}
			//printPrgAVMuxList();
		}
	
		if(list_len(PrgAVMuxListI)>0)
				{
					clsProgram.PrgAVMuxList[i]=PrgAVMuxListI;
					printPrgAVMuxList(clsProgram.PrgAVMuxList[i]);
				}
	}
	return 1;
}



void printPrgAVMuxList(list_t *PrgAVMuxListI)
{
	int j;
	int k;

	//printf("list_len(&PrgAVMuxList %d\n",list_len(PrgAVMuxList));

	//for ( j = 0; j < 1; j++)
	//{
	//	list_t *PrgAVMuxListI;
		//list_get(PrgAVMuxList,j,&PrgAVMuxListI);

	/*for ( k = 0; k < list_len(PrgAVMuxListI); k++)
	{
		MuxPidInfo_st *pidMapTmp;
		list_get(PrgAVMuxListI,k,&pidMapTmp);
		printf("pidMapTmp->inChannel %d   ",pidMapTmp->inChannel);
		printf("pidMapTmp->newPid %d   ",pidMapTmp->newPid);
		printf("pidMapTmp->oldPid %d\n",pidMapTmp->oldPid);

	}
	*/
	//}
}