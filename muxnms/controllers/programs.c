
/*
    programs Controller for esp-html-mvc (esp-html-mvc)
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "esp.h" 
#include "cJSON.h"
#include "communicate.h"
#include "getPrograms.h"
#include "clsParams.h"
#include "clsProgram.h"
#include "getOutPrograms.h"
#include "getTableJson.h"
#include "sendPrograms.h"
#include "clsMuxprgInfoGet.h"

extern ClsProgram_st clsProgram;
extern ClsParams_st *pdb;

static void rendersts(const char *str,int status)
{
	cJSON *result = cJSON_CreateObject();
	char* jsonstring;
	cJSON_AddNumberToObject(result,"sts", status);
	jsonstring = cJSON_PrintUnformatted(result);
	memcpy(str, jsonstring, strlen(jsonstring));
	//释放内存	
	cJSON_Delete(result);		
	free(jsonstring);
} 

static void cpystream(Dev_prgInfo_st *outprg, Dev_prgInfo_st *inprg, int streamindex, int flag){
	int cm = 0, i = 0, j = 0, hascontained = 0, streamlen = outprg->pdataStreamListLen;
	DataStream_t *StreamList = NULL;
	//修改节目数据流信息
	if(flag == 1){//add stream
	    DataStream_t *outpdataStreamInfo = outprg->pdataStreamList;
	    StreamList = malloc((outprg->pdataStreamListLen+1) * sizeof(DataStream_t));
        for(j=0; j<outprg->pdataStreamListLen; j++){
            memcpy(StreamList, outpdataStreamInfo, sizeof(DataStream_t));
            StreamList->desNode = malloc(outpdataStreamInfo->desNodeLen * sizeof(Commdes_t));
            Commdes_t *pdataStreamDesInfo = StreamList->desNode;
            Commdes_t *inpdataStreamDesInfo = outpdataStreamInfo->desNode;
            for (cm = 0; cm < outpdataStreamInfo->desNodeLen; cm++)
            {
                memcpy(pdataStreamDesInfo, inpdataStreamDesInfo, sizeof(Commdes_t) );
                pdataStreamDesInfo->data = malloc(inpdataStreamDesInfo->dataLen * sizeof(unsigned char) );
                memcpy(pdataStreamDesInfo->data, inpdataStreamDesInfo->data, inpdataStreamDesInfo->dataLen * sizeof(unsigned char));
                pdataStreamDesInfo++;
                inpdataStreamDesInfo++;
            }
            outpdataStreamInfo++;
            StreamList++;
        }
        //找到新添加的数据流添加到outprg
        DataStream_t *pdataStreamInfo = inprg->pdataStreamList;
        for(j=0; j<inprg->pdataStreamListLen; j++){
            if(pdataStreamInfo->index == streamindex){
                hascontained = 1;
                memcpy(StreamList, pdataStreamInfo, sizeof(DataStream_t));
                StreamList->desNode = malloc(pdataStreamInfo->desNodeLen * sizeof(Commdes_t));
                Commdes_t *pdataStreamDesInfo = StreamList->desNode;
                Commdes_t *inpdataStreamDesInfo = pdataStreamInfo->desNode;
                for (cm = 0; cm < pdataStreamInfo->desNodeLen; cm++)
                {
                    memcpy(pdataStreamDesInfo, inpdataStreamDesInfo, sizeof(Commdes_t) );
                    pdataStreamDesInfo->data = malloc(inpdataStreamDesInfo->dataLen * sizeof(unsigned char) );
                    memcpy(pdataStreamDesInfo->data, inpdataStreamDesInfo->data, inpdataStreamDesInfo->dataLen * sizeof(unsigned char));
                    pdataStreamDesInfo++;
                    inpdataStreamDesInfo++;
                }
                streamlen++;
                break;
            }
        }
	}else{//delete stream
	    DataStream_t *outpdataStreamInfo = outprg->pdataStreamList;
        StreamList = malloc((outprg->pdataStreamListLen-1) * sizeof(DataStream_t));
        for(j=0; j<outprg->pdataStreamListLen; j++){
            if(outpdataStreamInfo->index != streamindex){
                memcpy(StreamList, outpdataStreamInfo, sizeof(DataStream_t));
                StreamList->desNode = malloc(outpdataStreamInfo->desNodeLen * sizeof(Commdes_t));
                Commdes_t *pdataStreamDesInfo = StreamList->desNode;
                Commdes_t *inpdataStreamDesInfo = outpdataStreamInfo->desNode;
                for (cm = 0; cm < outpdataStreamInfo->desNodeLen; cm++)
                {
                    memcpy(pdataStreamDesInfo, inpdataStreamDesInfo, sizeof(Commdes_t) );
                    pdataStreamDesInfo->data = malloc(inpdataStreamDesInfo->dataLen * sizeof(unsigned char) );
                    memcpy(pdataStreamDesInfo->data, inpdataStreamDesInfo->data, inpdataStreamDesInfo->dataLen * sizeof(unsigned char));
                    pdataStreamDesInfo++;
                    inpdataStreamDesInfo++;
                }
            }
            outpdataStreamInfo++;
            StreamList++;
        }
        hascontained = 1;
        streamlen--;
	}

    if(hascontained == 0){
        //not found
        //TODO
        printf("cpystream not found!\n");
    }else{
        //释放原始内存空间
        if(outprg->pdataStreamListLen>0){
            for(i=0; i< outprg->pdataStreamListLen; i++){
                for(j=0; j < outprg->pdataStreamList[i].desNodeLen; j++)
                {
                    if(outprg->pdataStreamList[i].desNode[j].dataLen){
                        free(outprg->pdataStreamList[i].desNode[j].data);
                        outprg->pdataStreamList[i].desNode[j].data = NULL;
                    }
                }
                if(outprg->pdataStreamList[i].desNodeLen>0){
                    free(outprg->pdataStreamList[i].desNode);
                    outprg->pdataStreamList[i].desNode = NULL;
                }
            }
            free(outprg->pdataStreamList);
            outprg->pdataStreamList = NULL;
        }
    }
	//重新指向新数据流地址
	outprg->pdataStreamListLen = streamlen;
	outprg->pdataStreamList = StreamList;
}

static void cpyprg2(ChannelProgramSt *outpst, Dev_prgInfo_st *outprg, Dev_prgInfo_st *inprg){
	int k = 0, cm = 0, i = 0;
	outprg = malloc(sizeof(Dev_prgInfo_st));
	memcpy(outprg, inprg, sizeof(Dev_prgInfo_st));				
	//pmt
	outprg->pmtDesList = malloc(outprg->pmtDesListLen * sizeof(Commdes_t) );
	Commdes_t *pmtDesInfo = outprg->pmtDesList;
	Commdes_t *inpmtDesInfo = inprg->pmtDesList;
	for (k = 0; k < outprg->pmtDesListLen; k++)
	{  
		memcpy(pmtDesInfo, inpmtDesInfo, sizeof(Commdes_t) );
		pmtDesInfo->data = malloc(pmtDesInfo->dataLen * sizeof(unsigned char) );
		memcpy(pmtDesInfo->data, inpmtDesInfo->data, pmtDesInfo->dataLen * sizeof(unsigned char));
		pmtDesInfo++;
		inpmtDesInfo++;
	}
	//stream data
	if(inprg->pdataStreamListLen>0){
        outprg->pdataStreamList = malloc(inprg->pdataStreamListLen * sizeof(DataStream_t));
        DataStream_t *pdataStreamInfo = inprg->pdataStreamList;
        DataStream_t *outpdataStreamInfo = outprg->pdataStreamList;
        for(i=0; i< inprg->pdataStreamListLen; i++){
            memcpy(outpdataStreamInfo, pdataStreamInfo, sizeof(DataStream_t));
            outpdataStreamInfo->desNode = malloc(pdataStreamInfo->desNodeLen * sizeof(Commdes_t));
            Commdes_t *pdataStreamDesInfo = pdataStreamInfo->desNode;
            Commdes_t *outpdataStreamDesInfo = outpdataStreamInfo->desNode;
            for (cm = 0; cm < pdataStreamInfo->desNodeLen; cm++)
            {
                memcpy(outpdataStreamDesInfo, pdataStreamDesInfo, sizeof(Commdes_t) );
                outpdataStreamDesInfo->data = malloc(pdataStreamDesInfo->dataLen * sizeof(unsigned char) );
                memcpy(outpdataStreamDesInfo->data, pdataStreamDesInfo->data, pdataStreamDesInfo->dataLen * sizeof(unsigned char));
                pdataStreamDesInfo++;
                outpdataStreamDesInfo++;
            }
            pdataStreamInfo++;
            outpdataStreamInfo++;
        }
    }
	
	//SDT 描述符
	outprg->psdtDesList = malloc(outprg->psdtDesListLen * sizeof(Commdes_t));
	Commdes_t *psdtDesInfo = outprg->psdtDesList;
	Commdes_t *inpsdtDesInfo = inprg->psdtDesList;
	for (k = 0; k < outprg->psdtDesListLen; k++)
	{ 					
		memcpy(psdtDesInfo, inpsdtDesInfo, sizeof(Commdes_t) );
		psdtDesInfo->data = malloc(inpsdtDesInfo->dataLen);
		memcpy(psdtDesInfo->data, inpsdtDesInfo->data, inpsdtDesInfo->dataLen);
		psdtDesInfo++;
		inpsdtDesInfo++;
	}
	outprg->prgName = malloc(inprg->prgNameLen);
	memcpy(outprg->prgName, inprg->prgName, inprg->prgNameLen);
	outprg->providerName = malloc(inprg->providerNameLen);
	memcpy(outprg->providerName, inprg->providerName, inprg->providerNameLen);
	
	list_append(&(outpst->prgNodes), outprg);
}

static void cpyprg(ChannelProgramSt *outpst, Dev_prgInfo_st *outprg, Dev_prgInfo_st *inprg, int streamindex){
	int k = 0;
	outprg = malloc(sizeof(Dev_prgInfo_st));
	memcpy(outprg, inprg, sizeof(Dev_prgInfo_st));				
	//pmt
	outprg->pmtDesList = malloc(outprg->pmtDesListLen * sizeof(Commdes_t) );
	Commdes_t *pmtDesInfo = outprg->pmtDesList;
	Commdes_t *inpmtDesInfo = inprg->pmtDesList;
	for (k = 0; k < outprg->pmtDesListLen; k++)
	{  
		memcpy(pmtDesInfo, inpmtDesInfo, sizeof(Commdes_t) );
		pmtDesInfo->data = malloc(pmtDesInfo->dataLen * sizeof(unsigned char) );
		memcpy(pmtDesInfo->data, inpmtDesInfo->data, pmtDesInfo->dataLen * sizeof(unsigned char));
		pmtDesInfo++;
		inpmtDesInfo++;
	}

	//stream data
	cpystream(outprg, inprg, streamindex, 1);
	//SDT 描述符
	outprg->psdtDesList = malloc(outprg->psdtDesListLen * sizeof(Commdes_t));
	Commdes_t *psdtDesInfo = outprg->psdtDesList;
	Commdes_t *inpsdtDesInfo = inprg->psdtDesList;
	for (k = 0; k < outprg->psdtDesListLen; k++)
	{ 					
		memcpy(psdtDesInfo, inpsdtDesInfo, sizeof(Commdes_t) );
		psdtDesInfo->data = malloc(inpsdtDesInfo->dataLen);
		memcpy(psdtDesInfo->data, inpsdtDesInfo->data, inpsdtDesInfo->dataLen);
		psdtDesInfo++;
		inpsdtDesInfo++;
	}
	outprg->prgName = malloc(inprg->prgNameLen);
	memcpy(outprg->prgName, inprg->prgName, inprg->prgNameLen);
	outprg->providerName = malloc(inprg->providerNameLen);
	memcpy(outprg->providerName, inprg->providerName, inprg->providerNameLen);
	
	list_append(&(outpst->prgNodes), outprg);
}

static int SeekReplacedPid(list_t *pidList, int chnId, int oldPid, int ifNotAddedUseNewPidValue)
{
    int i = 0;
    MuxPidInfo_st *mp = NULL;
    if(list_len(pidList)>0){
        for(i=0;i<list_len(pidList);i++){
            list_get(pidList, i, &mp);
            if(mp->oldPid == oldPid){
                return mp->newPid;
            }
        }
    }
    MuxPidInfo_st *mps = malloc(sizeof(MuxPidInfo_st));
    memcpy(&mps->inChannel, &chnId, sizeof(int));
    //mps->inChannel = chnId;
    memcpy(&mps->oldPid, &oldPid, sizeof(int));
    //mps->oldPid = oldPid;
    memcpy(&mps->newPid, &ifNotAddedUseNewPidValue, sizeof(int));
    //mps->newPid = ifNotAddedUseNewPidValue;
    list_append(pidList, mps);
    return ifNotAddedUseNewPidValue;
}

static void getprg(HttpConn *conn) { 
	MprJson *jsonparam = httpGetParams(conn);
    char ip[16] = "192.168.1.134";//param("ip"); 
    cchar *inChn = mprGetJson(jsonparam, "inch"); 
	int inCh = atoi(inChn);
	char pProg[20480] = {0};
	printf("-----------getinprg-----------------------\n");
    getprgsJson(ip, inCh, pProg);
	render(pProg);
    
} 

static void getoutprg(HttpConn *conn) {
	MprJson *jsonparam = httpGetParams(conn);
	int Chn = atoi(mprGetJson(jsonparam, "inch")); 
    char ip[16] = "192.168.1.134";
	char outprg[20480] = {0};
	int outChn = 0;	
	if(1){
		PrgMuxInfoGet(ip);
	}
	for(outChn=0; outChn<clsProgram._outChannelCntMax; outChn++){
		getOutPrograms(ip, outChn);
		LoadBitrateAndTableEnable(ip, outChn);
	}
	printf("-----------getoutprg-----------------------\n");
	getoutprgsJson(ip, Chn - 1, outprg);
	render(outprg);
    
} 

static void selectprgs(HttpConn *conn) {
	int i = 0, j = 0, pos = 0, ch = 0, prgindex = 0, hascontained = 0, flag = 0;
	char result[10] = {0};
	ChannelProgramSt *pst = NULL;
	ChannelProgramSt *outpst = NULL;
	Dev_prgInfo_st *inprg = NULL;
	Dev_prgInfo_st *outprg = NULL;
	MprJson *jsonparam = mprParseJson(espGetQueryString(conn));
	//printf("==========jsonparam===========%s\n", mprJsonToString (jsonparam, MPR_JSON_QUOTES));
	pos = atoi(mprGetJson(jsonparam, "channel"));
	flag = atoi(mprGetJson(jsonparam, "flag"));
	int selected = atoi(mprGetJson(jsonparam, "selected"));
	list_get(&(clsProgram.outPrgList), pos-1, &outpst);

	if(flag == 1){//根节点选择
	    //释放outprgs
        if(list_len(&outpst->prgNodes)>0)
            freePrograms(&outpst->prgNodes);
        if(selected){
            //拷贝inprgs to outprgs
            for(i=0; i<clsProgram._intChannelCntMax; i++){
                list_get(&(clsProgram.inPrgList), i, &pst);
                for(j=0; j<list_len(&pst->prgNodes); j++){
                    list_get(&(pst->prgNodes), j, &inprg);
                    cpyprg2(outpst, outprg, inprg);
                }
            }
        }
	}else if(flag == 2){//通道节点选择
        ch = atoi(mprGetJson(jsonparam, "ch"));
        //释放outprgs中通道号是CH的节目
        int outlen = list_len(&outpst->prgNodes);
        for(i=0; i<outlen; i++){
            list_get(&(outpst->prgNodes), i, &outprg);
            if(outprg->chnId == ch){
                freeProgramsMalloc(outprg);
                list_pop(&outpst->prgNodes, i);
                outlen--;
                i--;
                printf("========i=====%d\n", i);
            }
        }

        if(selected){
            //拷贝inprgs to outprgs
            list_get(&(clsProgram.inPrgList), ch-1, &pst);
            printf("========len=====%d\n", list_len(&pst->prgNodes));
            for(j=0; j<list_len(&pst->prgNodes); j++){
                list_get(&(pst->prgNodes), j, &inprg);
                if(inprg->chnId == ch){
                    cpyprg2(outpst, outprg, inprg);
                }
            }

        }
	}else if(flag ==3){//节目节点
        ch = atoi(mprGetJson(jsonparam, "ch"));
        prgindex = atoi(mprGetJson(jsonparam, "index"));
        hascontained = 0;
        if(selected){
            for(i=0; i<list_len(&outpst->prgNodes); i++){
                list_get(&(outpst->prgNodes), i, &outprg);
                if((outprg->chnId == ch)&&(outprg->index == prgindex)){
                    hascontained = 1;
                    break;
                }
            }
            if(hascontained == 0){
                //add prg
                list_get(&(clsProgram.inPrgList), ch-1, &pst);
                for(j=0; j<list_len(&pst->prgNodes); j++){
                    list_get(&(pst->prgNodes), j, &inprg);
                    if((inprg->chnId == ch)&&(inprg->index == prgindex)){
                        cpyprg2(outpst, outprg, inprg);
                    }
                }
            }
        }else{
            for(i=0; i<list_len(&outpst->prgNodes); i++){
                list_get(&(outpst->prgNodes), i, &outprg);
                if((outprg->chnId == ch)&&(outprg->index == prgindex)){
                    freeProgramsMalloc(outprg);
                    outprg = NULL;
                    list_pop(&outpst->prgNodes, i);
                    break;
                }
            }
        }
	}else if(flag ==4){//       流节点
        ch = atoi(mprGetJson(jsonparam, "ch"));
        prgindex = atoi(mprGetJson(jsonparam, "index"));
        int streamindex = atoi(mprGetJson(jsonparam, "streamindex"));
        if(selected){
            hascontained = 0;
            //判断outprg中是否有该节目
            for(i=0; i<list_len(&outpst->prgNodes); i++){
                list_get(&(outpst->prgNodes), i, &outprg);
                if((outprg->chnId == ch)&&(outprg->index == prgindex)){
                    hascontained = 1;
                    cpystream(outprg, inprg, streamindex, selected);
                    break;
                }
            }
            if(hascontained == 0){
                //add prg
                list_get(&(clsProgram.inPrgList), ch-1, &pst);
                for(j=0; j<list_len(&pst->prgNodes); j++){
                    list_get(&(pst->prgNodes), j, &inprg);
                    if((inprg->chnId == ch)&&(inprg->index == prgindex)){
                        cpyprg(outpst, outprg, inprg, streamindex);
                    }
                }
            }
        }else{
            for(i=0; i<list_len(&outpst->prgNodes); i++){
                list_get(&(outpst->prgNodes), i, &outprg);
                if((outprg->chnId == ch)&&(outprg->index == prgindex)){
                    hascontained = 1;
                    cpystream(outprg, inprg, streamindex, selected);
                    break;
                }
            }
        }
	}
	/*if(list_len(&outpst->prgNodes) == 0){
		list_init(&(outpst->prgNodes));
		//提取要制表的节目信息
		for(i=0; i<clsProgram._intChannelCntMax; i++){
			sprintf(str, "inCh%d", i+1);				
			if( 0 != mprGetJsonLength(mprGetJsonObj(jsonparam, str ))){				
				//获取输入通道信息
				list_get(&(clsProgram.inPrgList), i, &pst);
				//加入输出通道列表
				outpst->channelId = i+1;
				//加入输出节目信息
				for(j=0;j< mprGetJsonLength(mprGetJsonObj(jsonparam, str )); j++){
					sprintf(idstr, "id%d", j);
					MprJson *streamjson = mprGetJsonObj(mprGetJsonObj(jsonparam, str ), idstr);
					prgindex = atoi(mprGetJson(streamjson, "id"));
					list_get(&(pst->prgNodes), prgindex-1, &inprg);	
					cpyprg(outpst, outprg, inprg, streamjson);												
				}		
			}
		}
	}else{
		//判断选择的节目是增加还是减少
		if(prgcnt >= list_len(&outpst->prgNodes)){
			//add
			for(i=0; i<clsProgram._intChannelCntMax; i++){
				sprintf(str, "inCh%d", i+1);				
				if( 0 != mprGetJsonLength(mprGetJsonObj(jsonparam, str ))){					
					//获取输入通道信息
					list_get(&(clsProgram.inPrgList), i, &pst);
					//加入输出通道列表
					outpst->channelId = i+1;
					//加入输出节目信息
					for(j=0;j< mprGetJsonLength(mprGetJsonObj(jsonparam, str )); j++){
						hascontained = 0;
						sprintf(idstr, "id%d", j);
						MprJson *streamjson = mprGetJsonObj(mprGetJsonObj(jsonparam, str ), idstr);
						prgindex = atoi(mprGetJson(streamjson, "id"));
						list_get(&(pst->prgNodes), prgindex-1, &inprg);	
						for(k=0;k<list_len(&outpst->prgNodes);k++){	
							list_get(&(outpst->prgNodes), k, &outprg);	
							if((outprg->index == prgindex)&&(outprg->chnId == inprg->chnId)){
								hascontained = 1;
								//stream data
								cpystream(outprg, inprg, streamjson);
								break;
							}
						}						
						if(hascontained == 0){
							cpyprg(outpst, outprg, inprg, streamjson);													
						}
					}
				}
			}			
		}else{
			//delete
			list_t newpst;
			list_init(&newpst);
			for(k=0;k<list_len(&outpst->prgNodes);k++){
				list_get(&outpst->prgNodes, k, &outprg);
				for(i=0; i<clsProgram._intChannelCntMax; i++){
					sprintf(str, "inCh%d", i+1);				
					if( 0 != mprGetJsonLength(mprGetJsonObj(jsonparam, str ))){
						//获取输入通道信息
						list_get(&(clsProgram.inPrgList), i, &pst);
						//加入输出通道列表
						outpst->channelId = i+1;
						//加入输出节目信息
						for(j=0;j< mprGetJsonLength(mprGetJsonObj(jsonparam, str )); j++){
							sprintf(idstr, "id%d", j);
							MprJson *streamjson = mprGetJsonObj(mprGetJsonObj(jsonparam, str ), idstr);
							prgindex = atoi(mprGetJson(streamjson, "id"));
							list_get(&(pst->prgNodes), prgindex-1, &inprg);	
							if((outprg->index == prgindex)&&(outprg->chnId == i+1)){
								list_append(&newpst, outprg);
								break;
							}
						}
					}
				}
			}
			//释放内存
            while(list_len(&outpst->prgNodes)>0){
                list_pop_tail(&outpst->prgNodes);
            }
			memcpy(&outpst->prgNodes, &newpst, sizeof(newpst));

		}
		
	}*/
	rendersts(result, 1);
	render(result);
}

/*制表准备工作*/
static void maketable(HttpConn *conn) { 
	int pos = 0;
	char outstring[30480] = {0};
	cchar *role = getSessionVar("role");
	if(role == NULL){
        rendersts(outstring, 9);
        render(outstring);
        return;
    }
	if((strcmp(role, "root") !=0) && (strcmp(role, "admin") !=0)){
        rendersts(outstring, 5);//无权限
        render(outstring);
        return;
    }
	MprJson *jsonparam = mprParseJson(espGetQueryString(conn));
	pos = atoi(mprGetJson(jsonparam, "channel"));
	//制表
	MakeTable(pos);	
	//获取制表后结果
	getTableJson(pos, outstring);
	render(outstring);
    
} 

/*制表后获取输出流表*/
static void streamtable(HttpConn *conn) { 
	int pos = 0, i = 0;
	char outstring[4096] = {0};
	cJSON *streamsarray,*streamjson;
	char* streamjsonstring;
	MprJson *jsonparam = mprParseJson(espGetQueryString(conn));
	pos = atoi(mprGetJson(jsonparam, "channel"));
	streamsarray = cJSON_CreateArray();
	if(list_len(&clsProgram.PrgAVMuxList)>0)
	{
		list_t *AVMuxList = clsProgram.PrgAVMuxList[pos - 1];
		for ( i = 0; i < list_len(AVMuxList); i++)
		{
			MuxPidInfo_st *pidMapTmp;
			list_get(AVMuxList,i,&pidMapTmp);
			cJSON_AddItemToArray(streamsarray,streamjson = cJSON_CreateObject());
			cJSON_AddNumberToObject(streamjson, "NO", i+1);
			cJSON_AddNumberToObject(streamjson, "chn", pidMapTmp->inChannel);
			cJSON_AddNumberToObject(streamjson, "newPid", pidMapTmp->newPid);
			cJSON_AddNumberToObject(streamjson, "oldPid", pidMapTmp->oldPid);
			cJSON_AddStringToObject(streamjson,"type", "Programme");
		}
	}
	streamjsonstring = cJSON_PrintUnformatted(streamsarray);
		
	memcpy(outstring, streamjsonstring, strlen(streamjsonstring));
	//printf("---len--->>>%d\n",strlen(streamjsonstring));
	//释放内存	
	cJSON_Delete(streamsarray);		
	free(streamjsonstring);
	render(outstring);
    
} 

static void makestreamtable(HttpConn *conn) { 
    char outstring[1024] = {0};
	MprJson *jsonparam = httpGetParams(conn); 
    cchar *inChn = mprGetJson(jsonparam, "channel"); 
	int inCh = atoi(inChn);
	getStreamJson(inCh, outstring);
	render(outstring);    
} 

static void writetable(HttpConn *conn) { 
	MprJson *jsonparam = httpGetParams(conn); 
    cchar *inChn = mprGetJson(jsonparam, "channel"); 
	int inCh = atoi(inChn);
	char rsts[20] = {0};
	cchar *role = getSessionVar("role");
	if(role == NULL){
        rendersts(rsts, 9);
        render(rsts);
        return;
    }
	if((strcmp(role, "root") !=0) && (strcmp(role, "admin") !=0)){
        rendersts(rsts, 5);//无权限
        render(rsts);
        return;
    }
	if(!sendPrograms("192.168.1.134", inCh)){
		rendersts(rsts, 1);
	}else{
		rendersts(rsts, 0);
	}
	render(rsts);
} 

static void getchanneloutinfo(HttpConn *conn) { 
	MprJson *jsonparam = httpGetParams(conn); 
    cchar *inChn = mprGetJson(jsonparam, "channel"); 
	int inCh = atoi(inChn);	
	char rsts[20] = {0};
	if(!pdb){
		printf("getchanneloutinfo-----pdb is null!\n");
		rendersts(rsts, 0);
	}
	cJSON *pdbjson;
	char* jsonstring;
	pdbjson = cJSON_CreateObject();
	cJSON_AddNumberToObject(pdbjson,"networkId", pdb->pvalueTree->poutChnArray[inCh-1].networkId);
	cJSON_AddNumberToObject(pdbjson,"streamId", pdb->pvalueTree->poutChnArray[inCh-1].streamId);
	cJSON_AddNumberToObject(pdbjson,"oringal_networkid", pdb->pvalueTree->poutChnArray[inCh-1].oringal_networkid);
	cJSON_AddNumberToObject(pdbjson,"outputRate", pdb->pvalueTree->poutChnArray[inCh-1].outputRate);
	cJSON_AddNumberToObject(pdbjson,"isAutoRaiseVersion", pdb->pvalueTree->poutChnArray[inCh-1].isAutoRaiseVersion);
	cJSON_AddNumberToObject(pdbjson,"version", pdb->pvalueTree->poutChnArray[inCh-1].version);
	cJSON_AddNumberToObject(pdbjson,"isManualMapMode", pdb->pvalueTree->poutChnArray[inCh-1].isManualMapMode);
	cJSON_AddNumberToObject(pdbjson,"isAutoRankPAT", pdb->pvalueTree->poutChnArray[inCh-1].isAutoRankPAT);
	
	cJSON_AddNumberToObject(pdbjson,"isNeedSend_cat", pdb->pvalueTree->poutChnArray[inCh-1].isNeedSend_cat);
	cJSON_AddNumberToObject(pdbjson,"isNeedSend_nit", pdb->pvalueTree->poutChnArray[inCh-1].isNeedSend_nit);
	cJSON_AddNumberToObject(pdbjson,"isNeedSend_pat", pdb->pvalueTree->poutChnArray[inCh-1].isNeedSend_pat);
	cJSON_AddNumberToObject(pdbjson,"isNeedSend_pmt", pdb->pvalueTree->poutChnArray[inCh-1].isNeedSend_pmt);
	cJSON_AddNumberToObject(pdbjson,"isNeedSend_sdt", pdb->pvalueTree->poutChnArray[inCh-1].isNeedSend_sdt);
	jsonstring = cJSON_PrintUnformatted(pdbjson);
	//printf("------------outrate------------>>>>%d\n", pdb->pvalueTree->poutChnArray[inCh-1].outputRate);
	render(jsonstring);
	cJSON_Delete(pdbjson);
	free(jsonstring);
} 

static void setchanneloutinfo(HttpConn *conn) { 
	char rsts[20] = {0};
	cchar *role = getSessionVar("role");
	if(role == NULL){
	    rendersts(rsts, 9);
        render(rsts);
        return;
	}
	if((strcmp(role, "root") !=0) && (strcmp(role, "admin") !=0)){
        rendersts(rsts, 5);//无权限
        render(rsts);
        return;
    }
	MprJson *jsonparam = httpGetParams(conn); 
	//printf("==========setchanneloutinfo===========%s\n", mprJsonToString (jsonparam, MPR_JSON_QUOTES));
    cchar *inChn = mprGetJson(jsonparam, "channel"); 	
	int inCh = atoi(inChn);
	pdb->pvalueTree->poutChnArray[inCh-1].networkId = atoi(mprGetJson(jsonparam, "networkId"));
	pdb->pvalueTree->poutChnArray[inCh-1].oringal_networkid = atoi(mprGetJson(jsonparam, "oringal_networkid"));
	pdb->pvalueTree->poutChnArray[inCh-1].streamId = atoi(mprGetJson(jsonparam, "streamId"));
	pdb->pvalueTree->poutChnArray[inCh-1].outputRate = atoi(mprGetJson(jsonparam, "outputRate"));
	pdb->pvalueTree->poutChnArray[inCh-1].isAutoRaiseVersion = (unsigned char)atoi(mprGetJson(jsonparam, "isAutoRaiseVersion"));
	pdb->pvalueTree->poutChnArray[inCh-1].version = atoi(mprGetJson(jsonparam, "version"));
	//memcpy(&pdb->pvalueTree->poutChnArray[inCh-1].isManualMapMode, mprGetJson(jsonparam, "isManualMapMode"), sizeof(char));
	pdb->pvalueTree->poutChnArray[inCh-1].isAutoRankPAT = (unsigned char)atoi(mprGetJson(jsonparam, "isAutoRankPAT"));
	pdb->pvalueTree->poutChnArray[inCh-1].isNeedSend_cat = (unsigned char)atoi(mprGetJson(jsonparam, "isNeedSend_cat"));
	pdb->pvalueTree->poutChnArray[inCh-1].isNeedSend_nit = (unsigned char)atoi(mprGetJson(jsonparam, "isNeedSend_nit"));
	pdb->pvalueTree->poutChnArray[inCh-1].isNeedSend_pat = (unsigned char)atoi(mprGetJson(jsonparam, "isNeedSend_pat"));
	pdb->pvalueTree->poutChnArray[inCh-1].isNeedSend_pmt = (unsigned char)atoi(mprGetJson(jsonparam, "isNeedSend_pmt"));
	pdb->pvalueTree->poutChnArray[inCh-1].isNeedSend_sdt = (unsigned char)atoi(mprGetJson(jsonparam, "isNeedSend_sdt"));
	//printf(":::%d:::%d:::%d\n", pdb->pvalueTree->poutChnArray[inCh-1].isAutoRaiseVersion, pdb->pvalueTree->poutChnArray[inCh-1].isAutoRankPAT,pdb->pvalueTree->poutChnArray[inCh-1].isNeedSend_cat);
	rendersts(rsts, 1);
	render(rsts); 
} 

static void getpidtransinfo(HttpConn *conn) { 
	int i = 0;
	char outprg[512] = {0};
	MprJson *jsonparam = httpGetParams(conn); 
    cchar *inChn = mprGetJson(jsonparam, "channel"); 	
	int inCh = atoi(inChn);
	cJSON *result, *pidarrayjsn,*pidjson;
	char *jsonstring;
	MuxPidInfo_st *mpf = NULL;
	ChannelProgramSt *outpst = NULL;
	result = cJSON_CreateObject();
	list_get(&(clsProgram.outPrgList), inCh-1, &outpst);	
	if(list_len(&outpst->dtPidList)>0){
		cJSON_AddNumberToObject(result,"cnt", list_len(&outpst->dtPidList));
		cJSON_AddItemToObject(result, "children", pidarrayjsn = cJSON_CreateArray());
		for(i=0;i<list_len(&outpst->dtPidList);i++){			
			list_get(&outpst->dtPidList,i, &mpf);
			cJSON_AddItemToArray(pidarrayjsn,pidjson = cJSON_CreateObject());
			cJSON_AddNumberToObject(pidjson,"NO", i);
			cJSON_AddNumberToObject(pidjson,"ch", mpf->inChannel);
			cJSON_AddNumberToObject(pidjson,"oldPid", mpf->oldPid);
			cJSON_AddNumberToObject(pidjson,"newPid", mpf->newPid);
		}
	}else{		
		cJSON_AddNumberToObject(result,"cnt", 0);		
	}
	jsonstring = cJSON_PrintUnformatted(result);
	memcpy(outprg, jsonstring, strlen(jsonstring));
	//释放内存	
	cJSON_Delete(result);		
	free(jsonstring);
	render(outprg);
}

static void setpidtransinfo(HttpConn *conn) { 
	MprJson *jsonparam = httpGetParams(conn); 
	//printf("==========setpidtransinfo===========%s\n", mprJsonToString (jsonparam, MPR_JSON_QUOTES));
	char idstr[16] = {0};
	cchar *role = getSessionVar("role");
	if(role == NULL){
        rendersts(idstr, 9);
        render(idstr);
        return;
    }
	 if((strcmp(role, "root") !=0) && (strcmp(role, "admin") !=0)){
        rendersts(idstr, 5);//无权限
        render(idstr);
        return;
    }
	cchar *tmpstr;
	int i = 0, val = 0;
	int inCh = atoi(mprGetJson(jsonparam, "channel"));
	int pidcnt = atoi(mprGetJson(jsonparam, "pidcnt"));
	MuxPidInfo_st *mpf = NULL;
	ChannelProgramSt *outpst = NULL;
	list_get(&(clsProgram.outPrgList), inCh-1, &outpst);
	//验证数据
	for(i=0;i<pidcnt;i++){
		memset(idstr, 0, sizeof(idstr));
		sprintf(idstr, "p_ch%d", i);
		tmpstr = mprGetJson(jsonparam, idstr);
		sscanf(tmpstr, "%d", &val);
		if(val>9 || val<1){
			memset(idstr, 0, sizeof(idstr));
			rendersts(idstr, 0);
			render(idstr);
			return;
		}
		sprintf(idstr, "p_oldpid%d", i);
		tmpstr = mprGetJson(jsonparam, idstr);
		sscanf(tmpstr, "%x", &val);
		if(val>0xffff || val<1){
			memset(idstr, 0, sizeof(idstr));
			rendersts(idstr, 0);
			render(idstr);
			return;
		}
		sprintf(idstr, "p_newpid%d", i);
		tmpstr = mprGetJson(jsonparam, idstr);	
		sscanf(tmpstr, "%x", &val);
		if(val>0xffff || val<1){
			memset(idstr, 0, sizeof(idstr));
			rendersts(idstr, 0);
			render(idstr);
			return;
		}
	}
	//释放原dtPidList内存空间
	if(list_len(&outpst->dtPidList)>0){
		for(i=list_len(&outpst->dtPidList)-1;i>-1;i--){			
			list_get(&outpst->dtPidList,i, &mpf);
			free(mpf);
			mpf = NULL;
			list_pop_tail(&outpst->dtPidList);			
		}
	}
	//存储新数据
	for(i=0;i<pidcnt;i++){
		memset(idstr, 0, sizeof(idstr));
		sprintf(idstr, "p_ch%d", i);
		mpf = malloc(sizeof(MuxPidInfo_st));
		tmpstr = mprGetJson(jsonparam, idstr);
		sscanf(tmpstr, "%d", &mpf->inChannel);
		sprintf(idstr, "p_oldpid%d", i);
		tmpstr = mprGetJson(jsonparam, idstr);
		sscanf(tmpstr, "%x", &mpf->oldPid);
		sprintf(idstr, "p_newpid%d", i);
		tmpstr = mprGetJson(jsonparam, idstr);	
		sscanf(tmpstr, "%x", &mpf->newPid);
		list_append(&outpst->dtPidList, mpf);
	}
	memset(idstr, 0, sizeof(idstr));
	rendersts(idstr, 1);
	render(idstr); 
}

static void getprginfo(HttpConn *conn) {
	int i = 0, j = 0;	
	char str[512] = {0};
	char prgname[32] = {0};
	ChannelProgramSt *outpst = NULL;
	Dev_prgInfo_st *outprg = NULL;
	cJSON *result = cJSON_CreateObject();
	cJSON *streamarray, *streamjson;
	char* jsonstring;	
	MprJson *jsonparam = mprParseJson(espGetQueryString(conn)); 
	//printf("==========jsonparam===========%s\n", mprJsonToString (jsonparam, MPR_JSON_QUOTES));
	int inCh = atoi(mprGetJson(jsonparam, "channel"));
	int pmtPid = atoi(mprGetJson(jsonparam, "pmtPid"));
	int chnid = atoi(mprGetJson(jsonparam, "chnid"));
	list_get(&(clsProgram.outPrgList), inCh-1, &outpst);
	for(i=0; i<list_len(&outpst->prgNodes); i++){
		list_get(&outpst->prgNodes, i, &outprg);
		//printf("==========read=====%d=====%d\n", pmtPid, outprg->pmtPid);
		if((outprg->pmtPid == pmtPid)&&(outprg->chnId == chnid)){
			cJSON_AddNumberToObject(result,"prgNum", outprg->prgNum);
			cJSON_AddNumberToObject(result,"chnId", outprg->chnId);
			cJSON_AddNumberToObject(result,"serviceType", outprg->serviceType);
			cJSON_AddNumberToObject(result,"networkId", outprg->networkId);
			cJSON_AddNumberToObject(result,"pmtPid", outprg->pmtPid);
			cJSON_AddNumberToObject(result,"oldPcrPid", outprg->oldPcrPid);
			cJSON_AddNumberToObject(result,"newPcrPid", outprg->newPcrPid);
			memcpy(prgname, outprg->prgName, outprg->prgNameLen);			
			cJSON_AddStringToObject(result,"prgName", prgname);
			cJSON_AddItemToObject(result, "children", streamarray = cJSON_CreateArray());
			DataStream_t *streaminfo = outprg->pdataStreamList;
			for(j=0; j<outprg->pdataStreamListLen; j++) {
				cJSON_AddItemToArray(streamarray,streamjson = cJSON_CreateObject());
				cJSON_AddNumberToObject(streamjson,"NO", j);
				cJSON_AddNumberToObject(streamjson,"index", streaminfo->index);
				cJSON_AddNumberToObject(streamjson,"streamtype", streaminfo->streamType);
				cJSON_AddNumberToObject(streamjson,"inpid", streaminfo->inPid);
				cJSON_AddNumberToObject(streamjson,"outpid", streaminfo->outPid);
				cJSON_AddNumberToObject(streamjson,"inChn", streaminfo->inChn);
				streaminfo++;
			}
			break;
		}
	}
	jsonstring = cJSON_PrintUnformatted(result);
	memcpy(str, jsonstring, strlen(jsonstring));
	//printf("==========jsonstring===========%s\n", jsonstring);
	//释放内存	
	cJSON_Delete(result);		
	free(jsonstring);
	render(str); 
} 

static void setprginfo(HttpConn *conn) { 	
	int i = 0, j = 0, k = 0, cm = 0, index = 0, matched = 0;
	char rsts[20] = {0};
	cchar *role = getSessionVar("role");
	if(role == NULL){
        rendersts(rsts, 9);
        render(rsts);
        return;
    }
     if((strcmp(role, "root") !=0) && (strcmp(role, "admin") !=0)){
        rendersts(rsts, 5);//无权限
        render(rsts);
        return;
    }
	ChannelProgramSt *outpst = NULL;
	Dev_prgInfo_st *outprg = NULL;
	MprJson *jsonparam = httpGetParams(conn); 
	//printf("==========setprginfo===========%s\n", mprJsonToString (jsonparam, MPR_JSON_QUOTES));
	int inCh = atoi(mprGetJson(jsonparam, "channel"));
	cchar *prgname = mprGetJson(jsonparam, "prgname");
	int prgNum = atoi(mprGetJson(jsonparam, "prgNum"));
	int pmtPid = atoi(mprGetJson(jsonparam, "pmtpid"));
	int oldpcrpid = atoi(mprGetJson(jsonparam, "oldpcrpid"));
	int newpcrpid = atoi(mprGetJson(jsonparam, "newpcrpid"));
	int streamcnt = atoi(mprGetJson(jsonparam, "streamcnt"));
	int orgiralpmtPid = atoi(mprGetJson(jsonparam, "orgiralpmtpid"));
	int chnid = atoi(mprGetJson(jsonparam, "chnid"));
	int servicetype = atoi(mprGetJson(jsonparam, "servicetype"));
	//查找编辑节目
	list_get(&(clsProgram.outPrgList), inCh-1, &outpst);	
	for(i=0; i<list_len(&outpst->prgNodes); i++){
		list_get(&outpst->prgNodes, i, &outprg);	
		if((outprg->pmtPid == orgiralpmtPid)&&(outprg->chnId == chnid)){
		    //printf("============pmtpid===%d", pmtPid);
			outprg->prgNum = prgNum;
			outprg->pmtPid = pmtPid;
			outprg->oldPcrPid = oldpcrpid;
			outprg->newPcrPid = newpcrpid;
			outprg->serviceType = servicetype;
			outprg->prgNameLen = strlen(prgname);
			memcpy(outprg->prgName, prgname, outprg->prgNameLen);			
			//修改节目数据流信息
			//重新分配新的数据流内存
			DataStream_t *StreamList = malloc(streamcnt * sizeof(DataStream_t));			
			DataStream_t *pdataStreamInfo = StreamList;	
			for(k=0; k<streamcnt; k++){				
				DataStream_t *oldpdataStreamInfo = outprg->pdataStreamList;
				memset(rsts, 0, sizeof(rsts));
				sprintf(rsts, "index%d", k);
				index = atoi(mprGetJson(jsonparam, rsts));
				matched = 0;
				for(j=0; j<outprg->pdataStreamListLen; j++){
					if(oldpdataStreamInfo->index == index){
						matched = 1;
						//复制数据流到新内存空间
						memcpy(pdataStreamInfo, oldpdataStreamInfo, sizeof(DataStream_t));
						pdataStreamInfo->desNode = malloc(oldpdataStreamInfo->desNodeLen * sizeof(Commdes_t));                    
						Commdes_t *pdataStreamDesInfo = pdataStreamInfo->desNode;
						Commdes_t *inpdataStreamDesInfo = oldpdataStreamInfo->desNode;
						for (cm = 0; cm < pdataStreamInfo->desNodeLen; cm++)
						{ 
							memcpy(pdataStreamDesInfo, inpdataStreamDesInfo, sizeof(Commdes_t) );
							pdataStreamDesInfo->data = malloc(inpdataStreamDesInfo->dataLen * sizeof(unsigned char) );
							memcpy(pdataStreamDesInfo->data, inpdataStreamDesInfo->data, inpdataStreamDesInfo->dataLen * sizeof(unsigned char));
							pdataStreamDesInfo++;
							inpdataStreamDesInfo++;
						}	
						break;
					}
					oldpdataStreamInfo++;
				}
				//修改新数据流
				memset(rsts, 0, sizeof(rsts));
				sprintf(rsts, "r_streamtype%d", k);
				pdataStreamInfo->streamType = atoi(mprGetJson(jsonparam, rsts));
				memset(rsts, 0, sizeof(rsts));
				sprintf(rsts, "r_inpid%d", k);
				pdataStreamInfo->inPid = atoi(mprGetJson(jsonparam, rsts));
				memset(rsts, 0, sizeof(rsts));
				sprintf(rsts, "r_outpid%d", k);
				pdataStreamInfo->outPid = atoi(mprGetJson(jsonparam, rsts));
				if(matched == 0){
					//新增加的流数据
					pdataStreamInfo->index = index;
					oldpdataStreamInfo--;
					pdataStreamInfo->inChn = oldpdataStreamInfo->inChn;
					pdataStreamInfo->desNodeLen = 0;					
				}
				pdataStreamInfo++;
			}
			//释放原始内存空间
			if(outprg->pdataStreamListLen>0){
				for(k=0; k< outprg->pdataStreamListLen; k++){
					for(j=0; j < outprg->pdataStreamList[k].desNodeLen; j++)
					{				
						if(outprg->pdataStreamList[k].desNode[j].dataLen){
							free(outprg->pdataStreamList[k].desNode[j].data);
							outprg->pdataStreamList[k].desNode[j].data = NULL;					
						}												
					}
					if(outprg->pdataStreamList[k].desNodeLen>0){
						free(outprg->pdataStreamList[k].desNode);
						outprg->pdataStreamList[k].desNode = NULL;
					}				
				}		
				free(outprg->pdataStreamList);
				outprg->pdataStreamList = NULL;
			}
			//重新指向新的内存空间
			outprg->pdataStreamListLen = streamcnt;
			outprg->pdataStreamList = StreamList;			
		}
	}
	rendersts(rsts, 1);
	render(rsts); 
}

static void common(HttpConn *conn) {
	
	
}

static void getglobalinfo(HttpConn *conn) { 
	char str[64] = {0};
    ChannelProgramSt *pst = NULL;
	cJSON *result = cJSON_CreateObject();
	char* jsonstring;
	cJSON_AddNumberToObject(result, "_intChannelCntMax", clsProgram._intChannelCntMax);
	jsonstring = cJSON_PrintUnformatted(result);
	memcpy(str, jsonstring, strlen(jsonstring));
	//释放内存	
	cJSON_Delete(result);		
	free(jsonstring);
	render(str);
}

static void search(HttpConn *conn) {
	char ip[16] = "192.168.1.134";
	char str[64] = {0};
	int i = 0, rst = 0;
    char* jsonstring;
    MprJson *jsonparam = httpGetParams(conn);
    //printf("==========setprginfo===========%s\n", mprJsonToString (jsonparam, MPR_JSON_QUOTES));
    int inCh = atoi(mprGetJson(jsonparam, "inch"));
    rst = Search(ip, inCh);

    cJSON *result = cJSON_CreateObject();
    cJSON_AddNumberToObject(result, "sts", rst);
    cJSON_AddNumberToObject(result, "_intChannelCntMax", clsProgram._intChannelCntMax);
    cJSON_AddNumberToObject(result, "process", inCh);
    jsonstring = cJSON_PrintUnformatted(result);
    memset(str, 0, sizeof(str));
    memcpy(str, jsonstring, strlen(jsonstring));
    render(str);
    //释放内存
    cJSON_Delete(result);
    free(jsonstring);
}

static void reprgnum(HttpConn *conn) {
	char ip[16] = "192.168.1.134";
	char str[64] = {0};
	cchar *role = getSessionVar("role");
	if(role == NULL){
        rendersts(str, 9);
        render(str);
        return;
    }
     if((strcmp(role, "root") !=0) && (strcmp(role, "admin") !=0)){
        rendersts(str, 5);//无权限
        render(str);
        return;
    }
	int i = 0,j = 0, prgNumCnt = 0;
	ChannelProgramSt *outpst = NULL;
	Dev_prgInfo_st *outprg = NULL;
    MprJson *jsonparam = httpGetParams(conn);
    int inCh = atoi(mprGetJson(jsonparam, "inch"));
    for ( i = 0; i < clsProgram._outChannelCntMax; i++){
        if (inCh == 0 || inCh == i + 1){
            prgNumCnt = clsProgram.prgNum_min;
            list_get(&(clsProgram.outPrgList), i, &outpst);
            if(list_len(&outpst->prgNodes)>0){
                for(j=0; j<list_len(&outpst->prgNodes); j++){
                	list_get(&outpst->prgNodes, j, &outprg);
                	memcpy(&outprg->prgNum, &prgNumCnt, sizeof(int));
                    prgNumCnt++;
                }
            }
        }
    }
    //TODO
    //clsProgram.userPrgNodes

    rendersts(str, 1);
    render(str);
}

static void reprgpid(HttpConn *conn) {
	char ip[16] = "192.168.1.134";
	char str[64] = {0};
	cchar *role = getSessionVar("role");
	if(role == NULL){
        rendersts(str, 9);
        render(str);
        return;
    }
     if((strcmp(role, "root") !=0) && (strcmp(role, "admin") !=0)){
        rendersts(str, 5);//无权限
        render(str);
        return;
    }
	int i = 0,j = 0, k = 0, pidPrgStart = 0, pidAvStart = 0;
	list_t *usingPidList;
	list_init(usingPidList);
	ChannelProgramSt *outpst = NULL;
	Dev_prgInfo_st *outprg = NULL;
    MprJson *jsonparam = httpGetParams(conn);
    int inCh = atoi(mprGetJson(jsonparam, "inch"));
    for ( i = 0; i < clsProgram._outChannelCntMax; i++){
        if (inCh == 0 || inCh == i + 1){
            pidPrgStart = clsProgram.prgPid_min;
            pidAvStart = clsProgram.subPrgPid_min;
            list_get(&clsProgram.outPrgList, i, &outpst);
            if(list_len(&outpst->prgNodes)>0){
                for(j=0; j<list_len(&outpst->prgNodes); j++){
                	list_get(&outpst->prgNodes, j, &outprg);
                	int newPid = SeekReplacedPid(usingPidList, outprg->chnId, outprg->pmtPid, pidPrgStart);
                	if (newPid != outprg->pmtPid || pidPrgStart == outprg->pmtPid)
                    {
                        memcpy(&outprg->pmtPid, &newPid, sizeof(int));
                        pidPrgStart++;
                    }
                    if (outprg->newPcrPid != 0x1fff)
                    {
                        newPid = SeekReplacedPid(usingPidList, outprg->chnId, outprg->oldPcrPid, pidAvStart);
                        if (newPid != outprg->oldPcrPid || pidAvStart == outprg->oldPcrPid)
                        {
                            memcpy(&outprg->newPcrPid, &newPid, sizeof(int));
                            pidAvStart++;
                        }
                    }
                    pidAvStart = DesPidRefresh2(outprg->chnId, outprg->index, -1, outprg->pmtDesList, outprg->pmtDesListLen, pidAvStart, usingPidList);
                    if(outprg->pdataStreamList != NULL && outprg->pdataStreamListLen>0){
                        DataStream_t *dst = outprg->pdataStreamList;
                        for(k=0;k<outprg->pdataStreamListLen;k++){
                            newPid = SeekReplacedPid(usingPidList, dst->inChn, dst->inPid, pidAvStart);
                            if (newPid != dst->inPid)
                            {
                                memcpy(&dst->outPid, &newPid, sizeof(int));
                                pidAvStart++;
                            }
                            else if (pidAvStart == dst->inPid)
                            {
                                pidAvStart++;
                            }
                            pidAvStart = DesPidRefresh2(outprg->chnId, outprg->index, dst->index,
                            			dst->desNode, dst->desNodeLen, pidAvStart, usingPidList);
                            dst++;
                        }
                    }
                }
            }

            //TODO
            //clsProgram.userPrgNodes
        }
    }

    //释放内存
    MuxPidInfo_st *mp = NULL;
    for(i=0;i<list_len(usingPidList);i++){
        list_get(usingPidList, i, &mp);
        free(mp);
        list_pop(usingPidList, i);
    }
    rendersts(str, 1);
    render(str);
}

static void espinit() {	
	int i=0;
	ChannelProgramSt *pst = NULL;
	//全局变量初始化
	clsProgram._outChannelCntMax = 2;
	clsProgram._intChannelCntMax = 8;
	clsProgram._pmtMaxCnt = 29;
	clsProgram.prgNum_min = 1;
	clsProgram.prgPid_min = 0x100;
	clsProgram.prgPid_max = 0xfff;
	clsProgram.subPrgPid_min = 0x1000;
	clsProgram.subPrgPid_max = 0x1ff0;
	//给全局变量申请内存
	for(i=0; i<clsProgram._intChannelCntMax; i++){
		pst = malloc(sizeof(ChannelProgramSt));
		memset(pst, 0, sizeof(ChannelProgramSt));
		pst->channelId = i + 1;		
		list_append(&(clsProgram.inPrgList), pst);
	}	
	for(i=0; i<clsProgram._outChannelCntMax; i++){
		pst = malloc(sizeof(ChannelProgramSt));
		memset(pst, 0, sizeof(ChannelProgramSt));
		pst->channelId = i + 1;
		list_append(&(clsProgram.outPrgList), pst);
	}	
	
	clsProgram.m_autoMuxStartPid = malloc(clsProgram._outChannelCntMax);
	for (i = 0; i < clsProgram._outChannelCntMax; i++)
	{
		clsProgram.m_autoMuxStartPid[i] = 0x100;
	}

	Init(clsProgram._outChannelCntMax);

	
	printf("======>>>>esp init!!!!!!!\n");
}

/*
    Dynamic module initialization
 */
ESP_EXPORT int esp_controller_muxnms_programs(HttpRoute *route, MprModule *module) {
    espDefineBase(route, common);
	espinit();	
    espDefineAction(route, "programs-cmd-getprg", getprg);
	espDefineAction(route, "programs-cmd-getprginfo", getprginfo);//menu-edit
	espDefineAction(route, "programs-cmd-setprginfo", setprginfo);//menu-edit
	espDefineAction(route, "programs-cmd-selectprgs", selectprgs);
	espDefineAction(route, "programs-cmd-getoutprg", getoutprg);
	espDefineAction(route, "programs-cmd-maketable", maketable);
	espDefineAction(route, "programs-cmd-streamtable", streamtable);
	espDefineAction(route, "programs-cmd-writetable", writetable);
	espDefineAction(route, "programs-cmd-getglobalinfo", getglobalinfo);
	espDefineAction(route, "programs-cmd-makestreamtable", makestreamtable);
    espDefineAction(route, "programs-cmd-getchanneloutinfo", getchanneloutinfo);
	espDefineAction(route, "programs-cmd-setchanneloutinfo", setchanneloutinfo);
	espDefineAction(route, "programs-cmd-getpidtransinfo", getpidtransinfo);
	espDefineAction(route, "programs-cmd-setpidtransinfo", setpidtransinfo);
	espDefineAction(route, "programs-cmd-search", search);
	espDefineAction(route, "programs-cmd-reprgnum", reprgnum);
	espDefineAction(route, "programs-cmd-reprgpid", reprgpid);
	
#if SAMPLE_VALIDATIONS
    Edi *edi = espGetRouteDatabase(route);
    ediAddValidation(edi, "present", "programs", "title", 0);
    ediAddValidation(edi, "unique", "programs", "title", 0);
    ediAddValidation(edi, "banned", "programs", "body", "(swear|curse)");
    ediAddValidation(edi, "format", "programs", "phone", "/^\\(?([0-9]{3})\\)?[-. ]?([0-9]{3})[-. ]?([0-9]{4})$/");
#endif
    return 0;
}
