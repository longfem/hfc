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
#include "communicate.h"
#include "getPrograms.h"
#include "clsParams.h"
#include "getOutPrograms.h"
extern ClsProgram_st clsProgram;

static char* substr(const char*str,unsigned start, unsigned end)
{
   unsigned n = end - start;
   static char stbuf[256];
   strncpy(stbuf, str + start, n);
   stbuf[n] = 0;
   return stbuf;
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

/*制表准备工作*/
static void maketable(HttpConn *conn) { 
	int i = 0, j = 0, k = 0,cm = 0, pos = 0, outprglen = 0, prgindex = 0;
	char str[6] = {0};
	char idstr[4] = {0};
	ChannelProgramSt *pst = NULL;
	ChannelProgramSt *outpst = NULL;
	Dev_prgInfo_st *inprg = NULL;
	Dev_prgInfo_st *outprg = NULL;
	
	ChannelProgramSt *testoutpst = NULL;
	Dev_prgInfo_st *testoutprg = NULL;
	MprJson *jsonparam = mprParseJson(espGetQueryString(conn));
	//提取要制表的节目信息
	for(i=0; i<clsProgram._intChannelCntMax; i++){
		sprintf(str, "inCh%d", i+1);				
		if( 0 != mprGetJsonLength(mprGetJsonObj(jsonparam, str ))){	
			list_get(&(clsProgram.outPrgList), pos, &outpst);
			if(outpst != NULL){
				if(list_len(&(outpst->prgNodes)) !=0){					
					freePrograms(&outpst->prgNodes);
				}				
				/* outprglen = list_len(&(outpst->prgNodes));
				//释放输出通道占用的节目内存
				if(0 != outprglen){
					for(j=0; j< outprglen; j++){
						list_get(&(outpst->prgNodes), j, &outprg);
						if(outprg != NULL){
							free(outprg);
							outprg = NULL;
						}
					}
				} */
			}
			list_init(&(outpst->prgNodes));
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
			pos++;
			if(pos >=  clsProgram._outChannelCntMax){
				//制表超过最大允许通道数，后面的忽略
				break;
			}		
		}
	}	
	
	list_get(&(clsProgram.outPrgList), 0, &testoutpst);
	list_get(&(testoutpst->prgNodes), 0, &testoutprg);
	printf("===%d===>>>>%d\n", testoutprg->prgNum, testoutprg->chnId);	
	
	render("");
    
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
    
#if SAMPLE_VALIDATIONS
    Edi *edi = espGetRouteDatabase(route);
    ediAddValidation(edi, "present", "programs", "title", 0);
    ediAddValidation(edi, "unique", "programs", "title", 0);
    ediAddValidation(edi, "banned", "programs", "body", "(swear|curse)");
    ediAddValidation(edi, "format", "programs", "phone", "/^\\(?([0-9]{3})\\)?[-. ]?([0-9]{3})[-. ]?([0-9]{4})$/");
#endif
    return 0;
}
