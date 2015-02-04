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

/* static char* substr(const char*str,unsigned start, unsigned end)
{
   unsigned n = end - start;
   static char stbuf[256];
   strncpy(stbuf, str + start, n);
   stbuf[n] = 0;
   return stbuf;
} */

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

/*制表准备工作*/
static void maketable(HttpConn *conn) { 
	int i = 0, j = 0, k = 0,cm = 0, pos = 0, prgindex = 0;
	char str[6] = {0};
	char idstr[4] = {0};
	char outstring[20480] = {0};
	ChannelProgramSt *pst = NULL;
	ChannelProgramSt *outpst = NULL;
	Dev_prgInfo_st *inprg = NULL;
	Dev_prgInfo_st *outprg = NULL;
	
	//ChannelProgramSt *testoutpst = NULL;
	//Dev_prgInfo_st *testoutprg = NULL;
	MprJson *jsonparam = mprParseJson(espGetQueryString(conn));
	pos = atoi(mprGetJson(jsonparam, "channel"));
	
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
	
	//list_get(&(clsProgram.outPrgList), 0, &testoutpst);
	//list_get(&(testoutpst->prgNodes), 0, &testoutprg);
	//printf("===%d===>>>>%d\n", testoutprg->prgNum, testoutprg->chnId);

	//制表
	MakeTable(pos);
	
	
	//获取制表后结果
	getTableJson(pos, outstring);
	render(outstring);
    
} 

static void getTable(HttpConn *conn) { 
    char outstring[1024] = {0};
	MprJson *jsonparam = httpGetParams(conn); 
    char *prgcount = mprGetJson(jsonparam, "prgnum"); 
	int count = atoi(prgcount);
	
	render(outstring);    
} 

static void writetable(HttpConn *conn) { 
	MprJson *jsonparam = httpGetParams(conn); 
    char *inChn = mprGetJson(jsonparam, "channel"); 
	int inCh = atoi(inChn);
	/*if(!sendPrograms(inCh)){
		render("OK"); 
	}else{
		render("ERROR"); 
	}*/
	   
} 

static void getchanneloutinfo(HttpConn *conn) { 
	MprJson *jsonparam = httpGetParams(conn); 
    char *inChn = mprGetJson(jsonparam, "channel"); 
	int inCh = atoi(inChn);	
	if(!pdb){
		printf("getchanneloutinfo-----pdb is null!\n");
		render("ERROR"); 
	}
	cJSON *pdbjson;
	char* jsonstring;
	pdbjson = cJSON_CreateObject();
	cJSON_AddStringToObject(pdbjson,"networkId", pdb->pvalueTree->poutChnArray[inCh-1].networkId);
	cJSON_AddStringToObject(pdbjson,"streamId", pdb->pvalueTree->poutChnArray[inCh-1].streamId);
	cJSON_AddStringToObject(pdbjson,"oringal_networkid", pdb->pvalueTree->poutChnArray[inCh-1].oringal_networkid);
	cJSON_AddStringToObject(pdbjson,"outputRate", pdb->pvalueTree->poutChnArray[inCh-1].outputRate);
	cJSON_AddStringToObject(pdbjson,"isAutoRaiseVersion", pdb->pvalueTree->poutChnArray[inCh-1].isAutoRaiseVersion);
	cJSON_AddStringToObject(pdbjson,"version", pdb->pvalueTree->poutChnArray[inCh-1].version);
	cJSON_AddStringToObject(pdbjson,"isManualMapMode", pdb->pvalueTree->poutChnArray[inCh-1].isManualMapMode);
	cJSON_AddStringToObject(pdbjson,"isAutoRankPAT", pdb->pvalueTree->poutChnArray[inCh-1].isAutoRankPAT);
	
	cJSON_AddStringToObject(pdbjson,"isNeedSend_cat", pdb->pvalueTree->poutChnArray[inCh-1].isNeedSend_cat);
	cJSON_AddStringToObject(pdbjson,"isNeedSend_nit", pdb->pvalueTree->poutChnArray[inCh-1].isNeedSend_nit);
	cJSON_AddStringToObject(pdbjson,"isNeedSend_pat", pdb->pvalueTree->poutChnArray[inCh-1].isNeedSend_pat);
	cJSON_AddStringToObject(pdbjson,"isNeedSend_pmt", pdb->pvalueTree->poutChnArray[inCh-1].isNeedSend_pmt);
	cJSON_AddStringToObject(pdbjson,"isNeedSend_sdt", pdb->pvalueTree->poutChnArray[inCh-1].isNeedSend_sdt);
	jsonstring = cJSON_PrintUnformatted(pdbjson);
	render(jsonstring);
	printf("=====================-----after render!\n");
	cJSON_Delete(pdbjson);
	free(jsonstring);
} 

static void setchanneloutinfo(HttpConn *conn) { 
	MprJson *jsonparam = httpGetParams(conn); 
	printf("==========setchanneloutinfo===========%s\n", mprJsonToString (jsonparam, MPR_JSON_QUOTES));
    char *inChn = mprGetJson(jsonparam, "channel"); 
	int inCh = atoi(inChn);
	pdb->pvalueTree->poutChnArray[inCh-1].networkId = atoi(mprGetJson(jsonparam, "networkId"));
	pdb->pvalueTree->poutChnArray[inCh-1].oringal_networkid = atoi(mprGetJson(jsonparam, "oringal_networkid"));
	pdb->pvalueTree->poutChnArray[inCh-1].streamId = atoi(mprGetJson(jsonparam, "streamId"));
	pdb->pvalueTree->poutChnArray[inCh-1].outputRate = atoi(mprGetJson(jsonparam, "outputRate"));
	pdb->pvalueTree->poutChnArray[inCh-1].isAutoRaiseVersion = atoi(mprGetJson(jsonparam, "isAutoRaiseVersion"));
	pdb->pvalueTree->poutChnArray[inCh-1].version = atoi(mprGetJson(jsonparam, "version"));
	pdb->pvalueTree->poutChnArray[inCh-1].isManualMapMode = atoi(mprGetJson(jsonparam, "isManualMapMode"));
	pdb->pvalueTree->poutChnArray[inCh-1].isAutoRankPAT = atoi(mprGetJson(jsonparam, "isAutoRankPAT"));
	
	pdb->pvalueTree->poutChnArray[inCh-1].isNeedSend_cat = atoi(mprGetJson(jsonparam, "isNeedSend_cat"));
	pdb->pvalueTree->poutChnArray[inCh-1].isNeedSend_nit = atoi(mprGetJson(jsonparam, "isNeedSend_nit"));
	pdb->pvalueTree->poutChnArray[inCh-1].isNeedSend_pat = atoi(mprGetJson(jsonparam, "isNeedSend_pat"));
	pdb->pvalueTree->poutChnArray[inCh-1].isNeedSend_pmt = atoi(mprGetJson(jsonparam, "isNeedSend_pmt"));
	pdb->pvalueTree->poutChnArray[inCh-1].isNeedSend_sdt = atoi(mprGetJson(jsonparam, "isNeedSend_sdt"));
	render("OK"); 
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
	espDefineAction(route, "programs-cmd-getoutprg", getoutprg);
	espDefineAction(route, "programs-cmd-maketable", maketable);
	espDefineAction(route, "programs-cmd-writetable", writetable);
	espDefineAction(route, "programs-cmd-getTable", getTable);
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
