
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

void cSerialize(ChannelProgramSt *poutList, unsigned char **pOutbuf, int *outLen)
{
	Dev_prgInfo_st *proginfo = NULL;

	Commdes_t *pPmrDes = NULL;
	DataStream_t *pdataStreamInfo = NULL;
	unsigned char *pBuf = NULL, *pTemp= NULL;
	int i=0, j=0, bufLen=0, k=0;
	if(poutList == NULL){
		printf("proginfo NULL \n");
		return -1;
	}


	bufLen += sizeof(ChannelProgramSt);

	int listlen = list_len(&poutList->prgNodes);

	bufLen += 4;  //listlen

	for(k=0; k < listlen; k ++){
		//compute on prg need malloc buf len

		list_get(&poutList->prgNodes, k, &proginfo);
		bufLen += sizeof(Dev_prgInfo_st); // struct sizeof(Dev_prgInfo_st) 	
		bufLen += proginfo->pmtDesListLen;   //pmtDesListLen
		

		pPmrDes = proginfo->pmtDesList;
		for(i=0; i< proginfo->pmtDesListLen; i++){
			bufLen += sizeof(Commdes_t);
			bufLen += pPmrDes->dataLen;
			pPmrDes++;
		}

		bufLen += 4;   //pdataStreamListLen;	 

		pdataStreamInfo = proginfo->pdataStreamList;
		for(i=0;  i < proginfo->pdataStreamListLen; i++){
			bufLen += sizeof(DataStream_t);

			pPmrDes = pdataStreamInfo->desNode;
			for(j=0; j< pdataStreamInfo->desNodeLen; j++){
				bufLen += sizeof(Commdes_t);
				bufLen += pPmrDes->dataLen;
				pPmrDes++;
			}

			pdataStreamInfo++;
		}

		bufLen+= proginfo->prgNameLen; //
		bufLen+= proginfo->providerNameLen; //

	
	}

	bufLen +=4; // Crc 32;
	
	//malloc now
	pBuf = malloc(bufLen);

	
	//copy now	
	//copy pmt
	pTemp = pBuf;

	memcpy(pTemp, (unsigned char *)poutList, sizeof(ChannelProgramSt));

	pTemp += sizeof(ChannelProgramSt);
	memcpy(pTemp, (unsigned char *)&listlen, 4 );
	pTemp +=4;

	
	for(k=0; k < listlen; k ++){
		list_get(&poutList->prgNodes, k, &proginfo);
		memcpy(pTemp, proginfo, sizeof(Dev_prgInfo_st));
		pTemp += sizeof(Dev_prgInfo_st);	
		memcpy(pTemp, (unsigned char *)&proginfo->pmtDesListLen, 4 );
		pTemp +=4;

		
		pPmrDes = proginfo->pmtDesList;
		for(i=0; i< proginfo->pmtDesListLen; i++){
			memcpy(pTemp, pPmrDes, sizeof(Commdes_t));
			pTemp += sizeof(Commdes_t);

			memcpy(pTemp, pPmrDes->data, pPmrDes->dataLen);				
			pTemp+=	pPmrDes->dataLen;
			pPmrDes++;
		}

		//copy datastream
		memcpy(pTemp, (unsigned char *)&proginfo->pdataStreamListLen, 4 );
		pTemp +=4;

		
		pdataStreamInfo = proginfo->pdataStreamList;
		
		for(i=0 ; i < proginfo->pdataStreamListLen; i++){
			memcpy(pTemp, pdataStreamInfo, sizeof(DataStream_t));
			pTemp += sizeof(DataStream_t);
			
			
			pPmrDes = pdataStreamInfo->desNode;
			for(j=0; j< pdataStreamInfo->desNodeLen; j++){
				memcpy(pTemp, pPmrDes, sizeof(Commdes_t));
				pTemp += sizeof(Commdes_t);
				memcpy(pTemp, pPmrDes->data, pPmrDes->dataLen);
				pTemp += pPmrDes->dataLen;
				
				pPmrDes++;
			}
			
			pdataStreamInfo++;
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

	}

	int  icrc32 = 0;
	memcpy(pTemp, (unsigned char *)&icrc32, 4 );

	//psdt

	if(pBuf != NULL){
		printf(" serial okkkk  \n");
		*pOutbuf = pBuf;
		*outLen = bufLen;
		
	} 
	else{
		*pOutbuf = NULL;
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
	
	Commdes_t *pPmrDes = pBuf->pmtDesList;
	
	for(i=0; i< pBuf->pmtDesListLen; i++){
		memcpy(pPmrDes, pTemp, sizeof(Commdes_t));		
		pTemp += sizeof(Commdes_t);
		memcpy(pPmrDes->data, pTemp, pPmrDes->dataLen);				
		pTemp+=	pPmrDes->dataLen;	

		pPmrDes++;		
	}

	//deserialize datastream
	int pdataStreamListLen=0;
	memcpy(&pdataStreamListLen, pTemp, 4 );
	pTemp +=4;

	if(pdataStreamListLen > 0){
		
		pBuf->pdataStreamList = malloc( sizeof(DataStream_t) * pdataStreamListLen);
		DataStream_t *pdataStreamInfo = pBuf->pdataStreamList;
		
		for(i=0 ; i < pBuf->pdataStreamListLen; i++){
			memcpy(pdataStreamInfo, pTemp, sizeof(DataStream_t));
			pTemp += sizeof(DataStream_t);
			
			pPmrDes = pdataStreamInfo->desNode;
			for(j=0; j< pdataStreamInfo->desNodeLen; j++){
				memcpy(pPmrDes, pTemp, sizeof(Commdes_t));
				pTemp += sizeof(Commdes_t);
				memcpy(pPmrDes->data, pTemp, pPmrDes->dataLen);
				pTemp += pPmrDes->dataLen;
				
				pPmrDes++;
			}
			pdataStreamInfo++;
		}	
	}else{
		pBuf->pdataStreamListLen = 0;
		pBuf->pdataStreamList = NULL;
	}
		
	if(pBuf->prgNameLen > 0){	
		memcpy(pBuf->prgName, pTemp, pBuf->prgNameLen);
		pTemp += 4;
	}

	//providerName 
	if(pBuf->providerNameLen > 0){   
		memcpy(pBuf->providerName, pTemp, pBuf->providerNameLen);
		pTemp += 4;	
	}	

	proginfo = pBuf;
	return 0;	

}

int MakeOutPutBytes(int outChn, unsigned char **outBytes, int *outLen)
{
	ChannelProgramSt *pst = NULL;	
	unsigned char *ptemp = NULL;
	int i=0;
	ChannelProgramSt * pOutList = NULL;

	list_get(&clsProgram.outPrgList, outChn - 1, &pOutList);

	cSerialize(pOutList, &ptemp, outLen);
	
	*outBytes =ptemp;

	return *outLen;

}


unsigned char MakeOutputBytesAndSend(char *ip, int outChn)
{
	unsigned char *tmpBytes = NULL;
	unsigned int tmpBytesLen=0;

	int i=0;
	
	int sendLen = MakeOutPutBytes(outChn,  &tmpBytes, &tmpBytesLen);

	if (sendLen > 0)
	{
		if (SendOutputPrgInfo(ip, outChn, tmpBytes, sendLen)){
			printf("bytes 31\n");
			free(tmpBytes);
			return 1;
		}
	}
	
	free(tmpBytes);
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

int SeekPrgPmtDes_inChn(int inChannel, int prgId, int pmtDesId, Commdes_t *desInfo)
{
    int i = 0, j = 0;
    ChannelProgramSt *pst = NULL;
    Dev_prgInfo_st *prg = NULL;
    Commdes_t *cmt = NULL;
    list_get(&clsProgram.inPrgList, inChannel - 1, &pst);
    if ((list_len(&clsProgram.inPrgList)<1) || (inChannel > list_len(&clsProgram.inPrgList)) || (list_len(&pst->prgNodes)>0))
        return 0;

    for(i=0;i<list_len(&pst->prgNodes);i++){
        list_get(&pst->prgNodes, i, &prg);
        if(prg->chnId == inChannel){
            if(prg->index == prgId){
                if(prg->pmtDesListLen > 0){
                    cmt = prg->pmtDesList;
                    for(j=0;j<prg->pmtDesListLen;j++){
                        if(cmt->index == pmtDesId){
                            desInfo = cmt;
                            return 1;
                        }
                    }
                    break;
                }
                break;
            }
        }
    }
    return 0;
}

int SeekPrgAvDes_inChn(int inChannel, int prgId, int avId, int desId, Commdes_t *desInfo)
{
    int i = 0, j = 0, k = 0;
    ChannelProgramSt *pst = NULL;
    Dev_prgInfo_st *prg = NULL;
    DataStream_t *dst = NULL;
    Commdes_t *cmt = NULL;
    list_get(&clsProgram.inPrgList, inChannel - 1, &pst);
    if ((list_len(&clsProgram.inPrgList)<1) || (inChannel > list_len(&clsProgram.inPrgList)) || (list_len(&pst->prgNodes)>0))
        return 0;

    for(i=0;i<list_len(&pst->prgNodes);i++){
        list_get(&pst->prgNodes, i, &prg);
        if(prg->chnId == inChannel){
            if(prg->index == prgId){
                if(prg->pdataStreamListLen>0){
                    dst = prg->pdataStreamList;
                    for(j=0;j<prg->pdataStreamListLen;j++){
                        if(dst->index == avId){
                            if(dst->desNodeLen > 0){
                                cmt = dst->desNode;
                                for(k=0;k<dst->desNodeLen;k++){
                                    if(cmt->index == desId){
                                        desInfo = cmt;
                                        return 1;
                                    }
                                }
                                break;
                            }
                            break;
                        }
                    }
                    break;
                }
                break;
            }
        }
    }
    return 0;
}

int DesPid_getInPid(int inChn, int prgIndex, int avIndex, int desIndex)
{
    int pid = 0x1fff;
    Commdes_t *desInfoTmp = NULL;
    //enum DesTypeEnum dte;
    if (inChn > 0 && prgIndex >= 0)
    {
        if (avIndex == -1)
        { // PMT的DES
            if (SeekPrgPmtDes_inChn(inChn, prgIndex, desIndex, desInfoTmp))
            {
                if (desInfoTmp->tag == 9)//CA
                {
                    pid = (((desInfoTmp->data[2] << 8) | desInfoTmp->data[3]) & 0x1fff);
                }
            }
        }
        else
        { // AV的DES
            if(SeekPrgAvDes_inChn(inChn, prgIndex, avIndex, desIndex, desInfoTmp))
            {
                if (desInfoTmp->tag == 9)//CA
                {
                    pid = (((desInfoTmp->data[2] << 8) | desInfoTmp->data[3]) & 0x1fff);
                }
            }
        }
    }
    return pid;
}

int DesPidRefresh2(int inChn, int prgIndex, int avIndex,
			Commdes_t *desList, int desListLen, int pidOffset, list_t *usingPidList)
{
    int i = 0, j = 0;
    //enum DesTypeEnum dte;
    if (desList != NULL && desListLen > 0)
    {
        //int newPid = 0x1fff;
        //bool isNeedSeekBefore = true;
        for (i = 0; i < desListLen; i++)
        {
            if (desList->tag == 9)//CA
            {
                if (desList->data != NULL && desList->dataLen >= 4)
                {
                    int lastReplacePid = -1;
                    int inPid = 0x1fff;
                    int caPid = (((desList->data[2] << 8) | desList->data[3]) & 0x1fff);
                    if (inChn > 0)
                    {
                        inPid = DesPid_getInPid(inChn, prgIndex, avIndex, desList->index);
                        if (inPid < 0x1fff)
                        {
                            MuxPidInfo_st *mp = NULL;
                            for (j = 0; j < list_len(usingPidList); j++)
                            {
                                list_get(usingPidList, j, &mp);
                                if (mp->inChannel == inChn && mp->oldPid == inPid)
                                {
                                    lastReplacePid = mp->newPid;
                                    break;
                                }
                            }
                        }
                    }
                    if (lastReplacePid != -1)
                    {
                        caPid = lastReplacePid;
                        desList->data[2] = (unsigned char)(desList->data[2] & 0xe0 | ((lastReplacePid >> 8) & 0x1f));
                        desList->data[3] = (unsigned char)lastReplacePid;
                    }
                    else
                    {
                        if (caPid != pidOffset)
                        {
                            caPid = pidOffset;
                            desList->data[2] = (unsigned char)(desList->data[2] & 0xe0 | ((pidOffset >> 8) & 0x1f));
                            desList->data[3] = (unsigned char)pidOffset;

                            MuxPidInfo_st *muxPidInfo = malloc(sizeof(MuxPidInfo_st));;
                            muxPidInfo->inChannel = inChn;
                            muxPidInfo->oldPid = inPid;
                            muxPidInfo->newPid = caPid;
                            list_append(usingPidList, muxPidInfo);
                        }
                        pidOffset++;
                    }
                }
            }
            desList++;
        }
    }
    return pidOffset;
}

int NewProgramNum(int curNum, list_t *prgNumList)
{
	int iNewPrgNum = -1;
	int isRepeat = 0;
	int i;
	int j;

	int *prgNumTmp;
	for (i= 0; i < list_len(prgNumList); i++)
	{

		list_get(prgNumList,i,&prgNumTmp);
		if (*prgNumTmp == curNum)
		{
			isRepeat = 1;
			break;
		}
	}
	if (isRepeat==0)
	{
		int *pcurNum=malloc(sizeof(int));
		*pcurNum=curNum;
		list_append(prgNumList,pcurNum);
		return curNum;
	}

	for (i = 1; i < 0xffff; i++)
	{
		isRepeat = 0;
		for (j= 0; j < list_len(prgNumList); j++)
		{
			list_get(prgNumList,j,&prgNumTmp);
			if (*prgNumTmp == i)
			{
				isRepeat = 1;
				break;
			}

		}
		if (isRepeat==0)
		{
			iNewPrgNum = i;

			int *piNewPrgNum=malloc(sizeof(int));
			*piNewPrgNum=iNewPrgNum;
			list_append(prgNumList,piNewPrgNum);
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
	list_t *usedPidTreeChin;
	replacePidst *newPidNode;


	list_get(&usedPidTree, inChn, &usedPidTreeChin);


	if (oldPid != 0x1fff && oldPid != -1)
	{	
		for (i = 0; i <list_len(usedPidTreeChin); i++)
		{

			list_get(usedPidTreeChin, i, &oldNodeTmp);
			if (oldNodeTmp->oldPid == oldPid)
			{
				// printf("oldNodeTmp->newPid111  %d\n",oldNodeTmp->newPid);
				return oldNodeTmp->newPid;

			}

		}
	}

	if (isUsedPid[curPid] == 0)
	{

		newPid = curPid;
		//printf("newPid  %d\n",newPid);
	}
	else
	{
		if (repeatIsOk==1 && oldPid != 0x1fff && oldPid != -1)
		{
			for (i = 0; i <list_len(usedPidTreeChin); i++)
			{

				list_get(usedPidTreeChin, i, &oldNodeTmp);
				if (oldNodeTmp->oldPid == oldPid)
				{
					//printf("oldNodeTmp->newPid222  %d\n",oldNodeTmp->newPid);
					return oldNodeTmp->newPid;
				}
			}

		}
	}
	//	printf("got new pid  \n");
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
	newPidNode = (replacePidst*)malloc(sizeof(replacePidst));
	newPidNode->oldPid = oldPid;
	newPidNode->newPid = newPid;
	isUsedPid[newPid] = 1;
	list_append(usedPidTreeChin,newPidNode);
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

	list_init(&prgNumList);


	for (i = 0; i < newUsedPidListLen; i++)
	{
		list_t *tmpList=(list_t*)malloc(sizeof(list_t));
		list_init(tmpList);
		list_append(&newUsedPidList,tmpList);
	}

	//for (i = 0; i < 1; i++)//for test
	
	for (i = 0; i < clsProgram._outChannelCntMax; i++)
	{

		if (outChannel == 0 || outChannel == i + 1)
		{

			for (j = 0; j < isUsedPidLenth; j++)
			{
				isUsedPid[j]=0;
			}			

			for (iTmp = 0; iTmp < newUsedPidListLen; iTmp++)
			{
				list_t *tmpList;
				list_get(&newUsedPidList, iTmp, &tmpList);

				while(list_len(tmpList)>0)
				{
					replacePidst *replacePidst1;
					list_get(tmpList,list_len(tmpList)-1,&replacePidst1);
					free(replacePidst1);
					list_pop_tail(tmpList);
				}
			}


			while(list_len(&prgNumList)>0)
			{
				int *piNewPrgNum;
				list_get(&prgNumList,list_len(&prgNumList)-1,&piNewPrgNum);	
				free(piNewPrgNum);				
				list_pop_tail(&prgNumList);
			}
			ChannelProgramSt *outpst;
		    list_get(&clsProgram.outPrgList, i, &outpst);


			if (list_len(&outpst->prgNodes)>0)
			{
				for (j = 0; j < list_len(&outpst->prgNodes); j++)
				{
					unsigned char  isNewPrgPid = 0;
					Dev_prgInfo_st *prgInfoTmp;
					list_get(&outpst->prgNodes,j,&prgInfoTmp);
					printf("program number: %d\n",j);
					//printPRG(prgInfoTmp);
					int newPid;

					//if (prgInfoTmp.chnId > 0 && prgInfoTmp.chnId <= _outChannelCntMax)
					{
						newPid = NewPid(prgInfoTmp->chnId, 0x1fff, prgInfoTmp->pmtPid, pidPrgStart, newUsedPidList, isUsedPid,isUsedPidLenth,0);
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


						//DesPidRefresh(prgInfoTmp->chnId, prgInfoTmp->index, -1,
						//prgInfoTmp->pmtDesList,prgInfoTmp->psdtDesListLen, pidAvStart, isUsedPid, newUsedPidList);


						int newPrgNum = NewProgramNum(prgInfoTmp->prgNum, &prgNumList);
						if (newPrgNum < 0)
							return 0;
						if (isNewPrgPid || newPrgNum != prgInfoTmp->prgNum)
						{
							prgInfoTmp->prgNum = newPrgNum;
						}

#if 1
						DataStream_t *dsInfoTmp = prgInfoTmp->pdataStreamList;
						for (iDs = 0; iDs < prgInfoTmp->pdataStreamListLen; iDs++)
						{	
							//printf("com in now\n");
							newPid = NewPid(prgInfoTmp->chnId, dsInfoTmp->inPid, dsInfoTmp->outPid, pidAvStart, newUsedPidList, isUsedPid,isUsedPidLenth, 1);

							//printf("get out now\n");

							if (dsInfoTmp->outPid != newPid)
							{
								isNewPrgPid = 1;
								dsInfoTmp->outPid = newPid;								
							}
							dsInfoTmp++;
						}
					
#endif
			//printPRG(prgInfoTmp);
					}

				}
			}
		}
	}


		//	printf("aaaaaaaaaaaaaaa\n");
	//free all
	for (iTmp = 0; iTmp < newUsedPidListLen; iTmp++)
	{
		list_t *tmpList;
		list_get(&newUsedPidList, iTmp, &tmpList);

		while(list_len(tmpList)>0)
		{
			replacePidst *replacePidst1;
			list_get(tmpList,list_len(tmpList)-1,&replacePidst1);
			free(replacePidst1);
			list_pop_tail(tmpList);
		}
	}
				//printf("bbbbbbbbbbbbb\n");
	while(list_len(&newUsedPidList)>0)
	{
		list_t *tmpList;
		list_get(&newUsedPidList, list_len(&newUsedPidList)-1, &tmpList);
		free(tmpList);
		list_pop_tail(&newUsedPidList);
	}
	//	free(&newUsedPidList);
		//printf("cccccccccccccccccccc\n");
	while(list_len(&prgNumList)>0)
	{
		int *piNewPrgNum;
		list_get(&prgNumList,list_len(&prgNumList)-1,&piNewPrgNum);	
		free(piNewPrgNum);				
		list_pop_tail(&prgNumList);
	}
	//free(&prgNumList);
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
	
		list_t *PrgAVMuxListI = (list_t*)malloc(sizeof(list_t));
		list_init(PrgAVMuxListI); 

		freeMuxPrgInfoList(clsProgram.PrgAVMuxList[i]);		

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
			//printPrgAVMuxList();
			if (outPrgInfoTmp->pmtDesListLen>0)
			{
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
							return 0;
						}
						DataStream_stTmp->outPid = lastThisPidMapToNewPid;
					}
				}
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
		//	printPrgAVMuxList(clsProgram.PrgAVMuxList[i]);
		}
	}
	return 1;
}

void printPrgAVMuxList(list_t *PrgAVMuxListI)
{
	int j;
	int k;
	for ( k = 0; k < list_len(PrgAVMuxListI); k++)
	{
		MuxPidInfo_st *pidMapTmp;
		list_get(PrgAVMuxListI,k,&pidMapTmp);
		printf("pidMapTmp->inChannel %d   ",pidMapTmp->inChannel);
		printf("pidMapTmp->newPid %d   ",pidMapTmp->newPid);
		printf("pidMapTmp->oldPid %d\n",pidMapTmp->oldPid);
	}
}

void printPRG(Dev_prgInfo_st* SDTS)
{
	int i;
	int j;
	printf("********\n");
		printf("SDTS->chnId----%d\n",SDTS->chnId);
	printf("newPcrPid----%d\n",SDTS->newPcrPid);
	printf("oldPcrPid----%d\n",SDTS->oldPcrPid);
	printf("pmtPid----%d\n",SDTS->pmtPid);
	printf("prgNum----%d\n",SDTS->prgNum);

	DataStream_t* dsInfoTmp = SDTS->pdataStreamList;
	for (i = 0; i < SDTS->pdataStreamListLen; i++)
	{
		printf("dsInfoTmp->outPid----%d\n",dsInfoTmp->outPid);
		dsInfoTmp++;
	}
}