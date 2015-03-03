
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
#include "getOutPrograms.h"
#include "getTableJson.h"
#include "sendPrograms.h"

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

static void cpyprg(ChannelProgramSt *outpst, Dev_prgInfo_st *outprg, Dev_prgInfo_st *inprg){
	int k = 0, cm = 0;
	outprg = malloc(sizeof(Dev_prgInfo_st));
	memcpy(outprg, inprg, sizeof(Dev_prgInfo_st));				
	//pmt
	outprg->pmtDesList = malloc(outprg->pmtDesListLen * sizeof(Commdes_t) );
	Commdes_t *pmtDesInfo = outprg->pmtDesList;
	Commdes_t *inpmtDesInfo = inprg->pmtDesList;
	for (k = 0; k < outprg->pmtDesListLen; k++)
	{  
		memcpy(pmtDesInfo, inpmtDesInfo, sizeof(Commdes_t) );
		pmtDesInfo++;
		inpmtDesInfo++;
	}
	//stream data
	outprg->pdataStreamList = malloc(outprg->pdataStreamListLen * sizeof(DataStream_t));
	DataStream_t *pdataStreamInfo = outprg->pdataStreamList;
	DataStream_t *inpdataStreamInfo = inprg->pdataStreamList;
	for (k = 0; k < outprg->pdataStreamListLen; k++)
	{  
		memcpy(pdataStreamInfo, inpdataStreamInfo, sizeof(DataStream_t) );		
		pdataStreamInfo->desNode = malloc(pdataStreamInfo->desNodeLen * sizeof(Commdes_t));                    
		Commdes_t *pdataStreamDesInfo = pdataStreamInfo->desNode;
		Commdes_t *inpdataStreamDesInfo = inpdataStreamInfo->desNode;
		for (cm = 0; cm < pdataStreamInfo->desNodeLen; cm++)
		{ 
			memcpy(pdataStreamDesInfo, inpdataStreamDesInfo, sizeof(Commdes_t) );
			pdataStreamDesInfo++;
			inpdataStreamDesInfo++;
		}
		pdataStreamInfo++;
		inpdataStreamInfo++;
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
	list_append(&(outpst->prgNodes), outprg);
}

static void getprg(HttpConn *conn) { 
	MprJson *jsonparam = httpGetParams(conn);
    char ip[16] = "192.168.1.134";//param("ip"); 
    char *inChn = mprGetJson(jsonparam, "inch"); 
	int inCh = atoi(inChn);
	char pProg[20480] = {0}; 	
    getprgsJson(ip, inCh, pProg);
	render(pProg);
    
} 

static void getoutprg(HttpConn *conn) { 
    char ip[16] = "192.168.1.134";
	int outChn = 0;
	for(outChn=0; outChn<clsProgram._outChannelCntMax; outChn++){
		getOutPrograms(ip, outChn);
	} 
	render("%d", clsProgram._intChannelCntMax);
    
} 

static void selectprgs(HttpConn *conn) {
	int i = 0, j = 0, k = 0, pos = 0, prgindex = 0, hascontained = 0;
	char str[6] = {0};
	char idstr[4] = {0};
	char result[10] = {0};
	ChannelProgramSt *pst = NULL;
	ChannelProgramSt *outpst = NULL;
	Dev_prgInfo_st *inprg = NULL;
	Dev_prgInfo_st *outprg = NULL;
	MprJson *jsonparam = mprParseJson(espGetQueryString(conn));
	pos = atoi(mprGetJson(jsonparam, "channel"));
	int prgnum = atoi(mprGetJson(jsonparam, "prgnum"));
	list_get(&(clsProgram.outPrgList), pos-1, &outpst);
	if(list_len(&outpst->prgNodes) == 0){
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
					prgindex = atoi(mprGetJson(mprGetJsonObj(jsonparam, str ), idstr));
					list_get(&(pst->prgNodes), prgindex-1, &inprg);	
					cpyprg(outpst, outprg, inprg);												
				}		
			}
		}
	}else{
		//判断选择的节目是增加还是减少
		if(prgnum > list_len(&outpst->prgNodes)){
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
						prgindex = atoi(mprGetJson(mprGetJsonObj(jsonparam, str ), idstr));
						list_get(&(pst->prgNodes), prgindex-1, &inprg);	
						for(k=0;k<list_len(&outpst->prgNodes);k++){	
							list_get(&(outpst->prgNodes), k, &outprg);	
							if(outprg->index == inprg->index){
								hascontained = 1;
								break;
							}
						}						
						if(hascontained == 0){
							cpyprg(outpst, outprg, inprg);
							break;
						}
					}
				}
			}
			
		}else{
			//delete		
			for(k=0;k<list_len(&outpst->prgNodes);k++){
				hascontained = 0;
				list_get(&(outpst->prgNodes), k, &outprg);	
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
							prgindex = atoi(mprGetJson(mprGetJsonObj(jsonparam, str ), idstr));
							list_get(&(pst->prgNodes), prgindex-1, &inprg);	
							if(outprg->index == inprg->index){
								hascontained = 1;
								break;
							}
						}
					}
				}
				if(hascontained == 0){
					//此节目已删除
					//释放内存
					freeProgramsMalloc(outprg);							
					list_pop(&(outpst->prgNodes), k);
					break;
				}
			}
		}
		
	}
	rendersts(result, 1);
	render(result);
}

/*制表准备工作*/
static void maketable(HttpConn *conn) { 
	//int i = 0, j = 0, k = 0,cm = 0, pos = 0, prgindex = 0;
	//char str[6] = {0};
	//char idstr[4] = {0};
	int pos = 0;
	char outstring[20480] = {0};
	//ChannelProgramSt *pst = NULL;
	//ChannelProgramSt *outpst = NULL;
	//Dev_prgInfo_st *inprg = NULL;
	//Dev_prgInfo_st *outprg = NULL;
	
	//ChannelProgramSt *testoutpst = NULL;
	//Dev_prgInfo_st *testoutprg = NULL;
	MprJson *jsonparam = mprParseJson(espGetQueryString(conn));
	pos = atoi(mprGetJson(jsonparam, "channel"));
	/*
	list_get(&(clsProgram.outPrgList), pos-1, &outpst);
	if(outpst != NULL){
		if(list_len(&(outpst->prgNodes)) !=0){					
			freePrograms(&outpst->prgNodes);
		}
	}
	list_init(&(outpst->prgNodes));
	//提取要制表的节目信息
	for(i=0; i<clsProgram._intChannelCntMax; i++){
		sprintf(str, "inCh%d", i+1);				
		if( 0 != mprGetJsonLength(mprGetJsonObj(jsonparam, str ))){				
			//获取输入通道信息
			list_get(&(clsProgram.inPrgList), i, &pst);
			//加入输出通道列表
			outpst->channelId = i+1;
			//memcpy(outpst, pst, sizeof(ChannelProgramSt));
			//加入输出节目信息
			for(j=0;j< mprGetJsonLength(mprGetJsonObj(jsonparam, str )); j++){
				sprintf(idstr, "id%d", j);
				prgindex = atoi(mprGetJson(mprGetJsonObj(jsonparam, str ), idstr));
				//printf("===ch===>>>>%d======index=====>>>%d\n", i+1, prgindex);
				list_get(&(pst->prgNodes), prgindex-1, &inprg);	
				outprg = malloc(sizeof(Dev_prgInfo_st));
				memcpy(outprg, inprg, sizeof(Dev_prgInfo_st));				
				//pmt
				outprg->pmtDesList = malloc(outprg->pmtDesListLen * sizeof(Commdes_t) );
                Commdes_t *pmtDesInfo = outprg->pmtDesList;
				Commdes_t *inpmtDesInfo = inprg->pmtDesList;
				for (k = 0; k < outprg->pmtDesListLen; k++)
                {  
					memcpy(pmtDesInfo, inpmtDesInfo, sizeof(Commdes_t) );
					pmtDesInfo++;
					inpmtDesInfo++;
                }
				//stream data
				outprg->pdataStreamList = malloc(outprg->pdataStreamListLen * sizeof(DataStream_t));
                DataStream_t *pdataStreamInfo = outprg->pdataStreamList;
				DataStream_t *inpdataStreamInfo = inprg->pdataStreamList;
				for (k = 0; k < outprg->pdataStreamListLen; k++)
                {  
					memcpy(pdataStreamInfo, inpdataStreamInfo, sizeof(DataStream_t) );
                    pdataStreamInfo->desNode = malloc(pdataStreamInfo->desNodeLen * sizeof(Commdes_t));                    
                    Commdes_t *pdataStreamDesInfo = pdataStreamInfo->desNode;
					Commdes_t *inpdataStreamDesInfo = inpdataStreamInfo->desNode;
					for (cm = 0; cm < pdataStreamInfo->desNodeLen; cm++)
					{ 
						memcpy(pdataStreamDesInfo, inpdataStreamDesInfo, sizeof(Commdes_t) );
						pdataStreamDesInfo++;
						inpdataStreamDesInfo++;
					}
					pdataStreamInfo++;
					inpdataStreamInfo++;
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
				
				list_append(&(outpst->prgNodes), outprg);
			}		
		}
	}	
	*/
	//list_get(&(clsProgram.outPrgList), 0, &testoutpst);
	//list_get(&(testoutpst->prgNodes), 0, &testoutprg);
	//printf("===%d===>>>>%d\n", testoutprg->prgNum, testoutprg->chnId);

	//制表
	MakeTable(pos);
	
	
	//获取制表后结果
	getTableJson(pos, outstring);
	render(outstring);
    
} 

static void makestreamtable(HttpConn *conn) { 
    char outstring[1024] = {0};
	MprJson *jsonparam = httpGetParams(conn); 
    char *inChn = mprGetJson(jsonparam, "channel"); 
	int inCh = atoi(inChn);
	getStreamJson(inCh, outstring);
	render(outstring);    
} 

static void writetable(HttpConn *conn) { 
	MprJson *jsonparam = httpGetParams(conn); 
    char *inChn = mprGetJson(jsonparam, "channel"); 
	int inCh = atoi(inChn);
	char rsts[20] = {0};
	if(!sendPrograms("192.168.1.134", inCh)){
		rendersts(rsts, 1);
	}else{
		rendersts(rsts, 0);
	}
	render(rsts);
} 

static void getchanneloutinfo(HttpConn *conn) { 
	MprJson *jsonparam = httpGetParams(conn); 
    char *inChn = mprGetJson(jsonparam, "channel"); 
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
	render(jsonstring);
	cJSON_Delete(pdbjson);
	free(jsonstring);
} 

static void setchanneloutinfo(HttpConn *conn) { 
	char rsts[20] = {0};
	MprJson *jsonparam = httpGetParams(conn); 
	//printf("==========setchanneloutinfo===========%s\n", mprJsonToString (jsonparam, MPR_JSON_QUOTES));
    char *inChn = mprGetJson(jsonparam, "channel"); 	
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
	int prgnum = atoi(mprGetJson(jsonparam, "prgNum"));
	list_get(&(clsProgram.outPrgList), inCh-1, &outpst);
	for(i=0; i<list_len(&outpst->prgNodes); i++){
		list_get(&outpst->prgNodes, i, &outprg);	
		if(outprg->prgNum == prgnum){	
			cJSON_AddNumberToObject(result,"prgNum", outprg->prgNum);
			cJSON_AddNumberToObject(result,"chnId", outprg->chnId);
			cJSON_AddNumberToObject(result,"streamId", outprg->streamId);
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
	
	//释放内存	
	cJSON_Delete(result);		
	free(jsonstring);
	render(str); 
} 

static void setprginfo(HttpConn *conn) { 	
	char rsts[20] = {0};
	MprJson *jsonparam = httpGetParams(conn); 	
	int inCh = atoi(mprGetJson(jsonparam, "channel"));
	char *prgname = mprGetJson(jsonparam, "prgname");
	int prgNum = atoi(mprGetJson(jsonparam, "prgNum"));
	int pmtpid = atoi(mprGetJson(jsonparam, "pmtpid"));
	int oldpcrpid = atoi(mprGetJson(jsonparam, "oldpcrpid"));
	int newpcrpid = atoi(mprGetJson(jsonparam, "newpcrpid"));
	
	
	rendersts(rsts, 1);
	render(rsts); 
}

static void common(HttpConn *conn) {
	
	
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
	espDefineAction(route, "programs-cmd-writetable", writetable);
	espDefineAction(route, "programs-cmd-makestreamtable", makestreamtable);
    espDefineAction(route, "programs-cmd-getchanneloutinfo", getchanneloutinfo);
	espDefineAction(route, "programs-cmd-setchanneloutinfo", setchanneloutinfo);
	
#if SAMPLE_VALIDATIONS
    Edi *edi = espGetRouteDatabase(route);
    ediAddValidation(edi, "present", "programs", "title", 0);
    ediAddValidation(edi, "unique", "programs", "title", 0);
    ediAddValidation(edi, "banned", "programs", "body", "(swear|curse)");
    ediAddValidation(edi, "format", "programs", "phone", "/^\\(?([0-9]{3})\\)?[-. ]?([0-9]{3})[-. ]?([0-9]{4})$/");
#endif
    return 0;
}
