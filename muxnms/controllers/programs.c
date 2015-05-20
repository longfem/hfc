
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
#include "http.h"
#include "cJSON.h"
#include "communicate.h"
#include "getPrograms.h"
#include "clsParams.h"
#include "clsProgram.h"
#include "getOutPrograms.h"
#include "getPrgJson.h"
#include "getTableJson.h"
#include "sendPrograms.h"
#include "clsMuxprgInfoGet.h"

extern ClsProgram_st clsProgram;
extern ClsParams_st *pdb;

//char *tmpip = "192.168.1.49";
//conn->rx->parsedUri->host
char optstr[256] = {0};

static void rendersts(char *str,int status)
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
	DataStream_t *Newstream = NULL;
	DataStream_t *StreamList = NULL;
	//找到新添加的数据流添加到outprg
    DataStream_t *pdataStreamInfo = inprg->pdataStreamList;
    for(j=0; j<inprg->pdataStreamListLen; j++){
        if(pdataStreamInfo->index == streamindex){
            hascontained = 1;
            streamlen++;
            break;
        }
        pdataStreamInfo++;
    }
    if(hascontained == 1){
        //修改节目数据流信息
        if(flag == 1){//add stream
            DataStream_t *outpdataStreamInfo = outprg->pdataStreamList;
            Newstream = malloc(streamlen * sizeof(DataStream_t));
            StreamList = Newstream;
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
        }else{//delete stream
            DataStream_t *outpdataStreamInfo = outprg->pdataStreamList;
            Newstream = malloc((outprg->pdataStreamListLen-1) * sizeof(DataStream_t));
            StreamList = Newstream;
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
            streamlen = streamlen-2;
        }
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
        //重新指向新数据流地址
        outprg->pdataStreamListLen = streamlen;
        outprg->pdataStreamList = Newstream;
    }else{

    }

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
    outprg->pdataStreamListLen = 0;
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
    cchar *inChn = mprGetJson(jsonparam, "inch"); 
	int inCh = atoi(inChn);
	char pProg[204800] = {0};
    getprgsJson(conn->rx->parsedUri->host, inCh, pProg);
	render(pProg);
    
} 

static void getoutprg(HttpConn *conn) {
	MprJson *jsonparam = httpGetParams(conn);
	int Chn = atoi(mprGetJson(jsonparam, "inch"));
	char outprg[81920] = {0};
	int outChn = 0;	
	if(1){
		PrgMuxInfoGet(conn->rx->parsedUri->host);
	}
	for(outChn=0; outChn<clsProgram._outChannelCntMax; outChn++){
		getOutPrograms(conn->rx->parsedUri->host, outChn);
		//LoadBitrateAndTableEnable(conn->rx->parsedUri->host, outChn);

		ChnBypass_read(conn->rx->parsedUri->host, outChn);
		RecordInputChnUseStatus(outChn);

	}
	getoutprgsJson(conn->rx->parsedUri->host, Chn - 1, outprg);
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
	//printf("==========selectprgs===========%s\n", mprJsonToString (jsonparam, MPR_JSON_QUOTES));
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
        }else{  //del user prgs
            freeUserPrograms(&outpst->userPrgNodes);
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
            }
        }

        if(selected){
            //拷贝inprgs to outprgs
            list_get(&(clsProgram.inPrgList), ch-1, &pst);
            for(j=0; j<list_len(&pst->prgNodes); j++){
                list_get(&(pst->prgNodes), j, &inprg);
                if(inprg->chnId == ch){
                    cpyprg2(outpst, outprg, inprg);
                }
            }
        }else{//del user prgs
            if(ch == 9){
                freeUserPrograms(&outpst->userPrgNodes);
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
        }else{ //delete prg
            if(ch == 9){ //user prg
                User_prgInfo_t *usrprg = NULL;
                for(i=0; i<list_len(&outpst->userPrgNodes); i++){
                    list_get(&outpst->userPrgNodes, i, &usrprg);
                    if(usrprg->index == prgindex){
                        freeUserProgramsMalloc(usrprg);
                        usrprg = NULL;
                        list_pop(&outpst->userPrgNodes, i);
                        break;
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
                    list_get(&(clsProgram.inPrgList), ch-1, &pst);
                    for(j=0; j<list_len(&pst->prgNodes); j++){
                        list_get(&(pst->prgNodes), j, &inprg);
                        if((inprg->chnId == ch)&&(inprg->index == prgindex)){
                            cpystream(outprg, inprg, streamindex, selected);
                            break;
                        }
                    }
                }
            }
            if(hascontained == 0){
                //add prg
                list_get(&(clsProgram.inPrgList), ch-1, &pst);
                for(j=0; j<list_len(&pst->prgNodes); j++){
                    list_get(&(pst->prgNodes), j, &inprg);
                    if((inprg->chnId == ch)&&(inprg->index == prgindex)){
                        cpyprg(outpst, outprg, inprg, streamindex);
                        break;
                    }
                }
            }
        }else{
            for(i=0; i<list_len(&outpst->prgNodes); i++){
                list_get(&(outpst->prgNodes), i, &outprg);
                if((outprg->chnId == ch)&&(outprg->index == prgindex)){
                    hascontained = 1;
                    list_get(&(clsProgram.inPrgList), ch-1, &pst);
                    for(j=0; j<list_len(&pst->prgNodes); j++){
                        list_get(&(pst->prgNodes), j, &inprg);
                        if((inprg->chnId == ch)&&(inprg->index == prgindex)){
                            cpystream(outprg, inprg, streamindex, selected);
                            break;
                        }
                    }
                }
            }
        }
	}else if(flag ==8){//cat
	    ch = atoi(mprGetJson(jsonparam, "ch"));
        list_get(&(clsProgram.inPrgList), ch-1, &pst);
        list_t *caIdenList = &pst->caNode.caIdenList;
        list_t *outcaIdenList = &outpst->caNode.caIdenList;
        CA_descriptor *ca = NULL;
        CA_descriptor *outca = NULL;
        if(selected){
            if(outcaIdenList == NULL){
                outcaIdenList = malloc(sizeof(list_t));
                list_init(outcaIdenList);
            }
            for(i=0;i<list_len(caIdenList);i++){
                list_get(caIdenList, i, &ca);
                outca = malloc(sizeof(CA_descriptor));
                memcpy(outca, ca, sizeof(CA_descriptor));
                outca->private_data_byte = malloc(ca->private_data_byte_len);
                memcpy(outca->private_data_byte, ca->private_data_byte, ca->private_data_byte_len);
                list_append(outcaIdenList, outca);
            }
        }else{
            for(i=list_len(outcaIdenList)-1;i>-1;i--){
                list_get(outcaIdenList, i, &outca);
                if(outca->inChannel == ch){
                    if(outca->private_data_byte_len>0){
                        free(outca->private_data_byte);
                    }
                    free(outca);
                    outca = NULL;
                    list_pop(outcaIdenList, i);
                    if(list_len(outcaIdenList) == 0){
                        //free(outcaIdenList);
                        outcaIdenList = NULL;
                    }
                }
            }
        }
	}else if(flag ==9){//cat node
        ch = atoi(mprGetJson(jsonparam, "ch"));
        prgindex = atoi(mprGetJson(jsonparam, "index"));
        list_get(&(clsProgram.inPrgList), ch-1, &pst);
        list_t *caIdenList = &pst->caNode.caIdenList;
        list_t *outcaIdenList = &outpst->caNode.caIdenList;
        CA_descriptor *ca = NULL;
        CA_descriptor *outca = NULL;
        if(selected){
            if(outcaIdenList == NULL){
                outcaIdenList = malloc(sizeof(list_t));
                list_init(outcaIdenList);
            }
            for(i=0;i<list_len(caIdenList);i++){
                list_get(caIdenList, i, &ca);
                if(ca->inChannel == ch && ca->index == prgindex){
                    outca = malloc(sizeof(CA_descriptor));
                    memcpy(outca, ca, sizeof(CA_descriptor));
                    outca->private_data_byte = malloc(ca->private_data_byte_len);
                    memcpy(outca->private_data_byte, ca->private_data_byte, ca->private_data_byte_len);
                    list_append(outcaIdenList, outca);
                }
            }
        }else{
            for(i=0;i<list_len(outcaIdenList);i++){
                list_get(outcaIdenList, i, &outca);
                if(outca->inChannel == ch && outca->index == prgindex){
                    if(outca->private_data_byte_len>0){
                        free(outca->private_data_byte);
                    }
                    free(outca);
                    outca = NULL;
                    list_pop(outcaIdenList, i);
                    if(list_len(outcaIdenList) == 0){
                        //free(outcaIdenList);
                        outcaIdenList = NULL;
                    }
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
	int pos = 0, flag = 0;
	char outstring[60960] = {0};
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
	flag = MakeTable(pos);
	//获取制表后结果
	getTableJson(pos, outstring, flag);
	render(outstring);
    
}

/*获取制表信息*/
static void gettableinfo(HttpConn *conn) {
	int pos = 0;
	char outstring[60960] = {0};
	MprJson *jsonparam = httpGetParams(conn);
    pos = atoi(mprGetJson(jsonparam, "inch"));
	//获取制表后结果

	getTableJson(pos, outstring, 0);
	render(outstring);

}

/*制表后获取输出流表*/
static void streamtable(HttpConn *conn) { 
	int pos = 0, i = 0;
	char outstring[8192] = {0};
	cJSON *streamsarray,*streamjson;
	char* streamjsonstring;
	MprJson *jsonparam = mprParseJson(espGetQueryString(conn));
	pos = atoi(mprGetJson(jsonparam, "channel"));
	streamsarray = cJSON_CreateArray();
	if(clsProgram.PrgAVMuxList != NULL)
	{
		list_t *AVMuxList = clsProgram.PrgAVMuxList[pos - 1];
		if(AVMuxList == NULL){
		    rendersts(outstring, 5);
		    render(outstring);
		    return;
		}
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
	printf("---streamtable--->>>%d\n",strlen(streamjsonstring));
	//释放内存	
	cJSON_Delete(streamsarray);		
	free(streamjsonstring);
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
	if(!sendPrograms(conn->rx->parsedUri->host, inCh)){
		rendersts(rsts, 1);
	}else{
		rendersts(rsts, 0);
	}
	render(rsts);
	//add optlog
	Edi *db = ediOpen("db/muxnms.mdb", "mdb", EDI_AUTO_SAVE);
    EdiRec *optlog = ediCreateRec(db, "optlog");
    if(optlog == NULL){
       printf("================>>>optlog is NULL!!\n");
       return;
    }
    time_t curTime;
    time(&curTime);
    memset(optstr, 0, 256);
    sprintf(optstr, "{'user': '%s', 'desc': '用户下发配置.', 'level': '1', 'logtime':'%d'}", getSessionVar("userName"), curTime);
    MprJson  *row = mprParseJson(optstr);
    if(ediSetFields(optlog, row) == 0){
       printf("================>>>ediSetFields Failed!!\n");
    }
    ediUpdateRec(db, optlog);
    //ediClose(db);
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
	//add optlog
    Edi *db = ediOpen("db/muxnms.mdb", "mdb", EDI_AUTO_SAVE);
    EdiRec *optlog = ediCreateRec(db, "optlog");
    if(optlog == NULL){
       printf("================>>>optlog is NULL!!\n");
       return;
    }
    time_t curTime;
    time(&curTime);
    memset(optstr, 0, 256);
    sprintf(optstr, "{'user': '%s', 'desc': '用户配置选项信息.', 'level': '1', 'logtime':'%d'}", getSessionVar("userName"), curTime);
    MprJson  *row = mprParseJson(optstr);
    if(ediSetFields(optlog, row) == 0){
       printf("================>>>ediSetFields Failed!!\n");
    }
    ediUpdateRec(db, optlog);
    //ediClose(db);
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
	//add optlog
    Edi *db = ediOpen("db/muxnms.mdb", "mdb", EDI_AUTO_SAVE);
    EdiRec *optlog = ediCreateRec(db, "optlog");
    if(optlog == NULL){
       printf("================>>>optlog is NULL!!\n");
       return;
    }
    time_t curTime;
    time(&curTime);
    memset(optstr, 0, 256);
    sprintf(optstr, "{'user': '%s', 'desc': '用户修改PID透传表.', 'level': '1', 'logtime':'%d'}", getSessionVar("userName"), curTime);
    MprJson  *row = mprParseJson(optstr);
    if(ediSetFields(optlog, row) == 0){
       printf("================>>>ediSetFields Failed!!\n");
    }
    ediUpdateRec(db, optlog);
    //ediClose(db);
}

static void getprginfo(HttpConn *conn) {
	int i = 0, j = 0;	
	char str[512] = {0};
	char prgname[100] = {0};
	ChannelProgramSt *outpst = NULL;
	User_prgInfo_t *userprg = NULL;
	Dev_prgInfo_st *outprg = NULL;
	cJSON *result = cJSON_CreateObject();
	cJSON *streamarray, *streamjson;
	char* jsonstring;	
	MprJson *jsonparam = mprParseJson(espGetQueryString(conn)); 
	//printf("==========jsonparam===========%s\n", mprJsonToString (jsonparam, MPR_JSON_QUOTES));
	int inCh = atoi(mprGetJson(jsonparam, "channel"));
	int index = atoi(mprGetJson(jsonparam, "index"));
	int chnid = atoi(mprGetJson(jsonparam, "chnid"));
	list_get(&(clsProgram.outPrgList), inCh-1, &outpst);
	if(chnid == 9){
	    for(i=0; i<list_len(&outpst->userPrgNodes); i++){
            list_get(&outpst->userPrgNodes, i, &userprg);
            if(userprg->index == index){
                cJSON_AddNumberToObject(result,"prgNum", userprg->prgNum);
                cJSON_AddNumberToObject(result,"chnId", userprg->pcrPidInChn);
                cJSON_AddNumberToObject(result,"index", userprg->index);
                cJSON_AddNumberToObject(result,"serviceType", userprg->serviceType);
                cJSON_AddNumberToObject(result,"networkId", userprg->networkId);
                cJSON_AddNumberToObject(result,"pmtPid", userprg->pmtPid);
                cJSON_AddNumberToObject(result,"oldPcrPid", userprg->oldPcrPid);
                cJSON_AddNumberToObject(result,"newPcrPid", userprg->newPcrPid);
                memcpy(prgname, userprg->prgName, userprg->prgNameLen);
                cJSON_AddStringToObject(result,"prgName", prgname);
                memset(prgname, 0, sizeof(prgname));
                memcpy(prgname, userprg->providerName, userprg->providerNameLen);
                cJSON_AddStringToObject(result,"providerName", prgname);
                cJSON_AddItemToObject(result, "children", streamarray = cJSON_CreateArray());
                DataStream_t *streaminfo = userprg->pdataStreamList;
                for(j=0; j<userprg->pdataStreamListLen; j++) {
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
	}else{
        for(i=0; i<list_len(&outpst->prgNodes); i++){
            list_get(&outpst->prgNodes, i, &outprg);
            //printf("==========read=====%d=====%d\n", pmtPid, outprg->pmtPid);
            if((outprg->index == index)&&(outprg->chnId == chnid)){
                cJSON_AddNumberToObject(result,"prgNum", outprg->prgNum);
                cJSON_AddNumberToObject(result,"chnId", outprg->chnId);
                cJSON_AddNumberToObject(result,"index", outprg->index);
                cJSON_AddNumberToObject(result,"serviceType", outprg->serviceType);
                cJSON_AddNumberToObject(result,"networkId", outprg->networkId);
                cJSON_AddNumberToObject(result,"pmtPid", outprg->pmtPid);
                cJSON_AddNumberToObject(result,"oldPcrPid", outprg->oldPcrPid);
                cJSON_AddNumberToObject(result,"newPcrPid", outprg->newPcrPid);
                memcpy(prgname, outprg->prgName, outprg->prgNameLen);
                cJSON_AddStringToObject(result,"prgName", prgname);
                memset(prgname, 0, sizeof(prgname));
                memcpy(prgname, outprg->providerName, outprg->providerNameLen);
                cJSON_AddStringToObject(result,"providerName", prgname);
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
	}

	jsonstring = cJSON_PrintUnformatted(result);
	//printf("==========jsonstring===========%s\n", jsonstring);
	memcpy(str, jsonstring, strlen(jsonstring));
	//释放内存	
	cJSON_Delete(result);
	free(jsonstring);
	render(str);
} 

static void setprginfo(HttpConn *conn) { 	
	int i = 0, j = 0, k = 0, cm = 0, index = 0, matched = 0, val = 0;
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
	cchar *providername = mprGetJson(jsonparam, "providername");
	int prgNum = atoi(mprGetJson(jsonparam, "prgNum"));
	sscanf(mprGetJson(jsonparam, "pmtpid"), "%x", &val);
    int pmtPid = val;
    sscanf(mprGetJson(jsonparam, "oldpcrpid"), "%x", &val);
    int oldpcrpid = val;
    sscanf(mprGetJson(jsonparam, "newpcrpid"), "%x", &val);
    int newpcrpid = val;
	int streamcnt = atoi(mprGetJson(jsonparam, "streamcnt"));;
	sscanf(mprGetJson(jsonparam, "orgiralindex"), "%x", &val);
    int orgiralindex = val;
	int chnid = atoi(mprGetJson(jsonparam, "chnid"));
	int servicetype = atoi(mprGetJson(jsonparam, "servicetype"));
	int prgoptflag = atoi(mprGetJson(jsonparam, "prgoptflag"));
	int isuserprg = atoi(mprGetJson(jsonparam, "isuserprg"));
    list_get(&(clsProgram.outPrgList), inCh-1, &outpst);
    time_t curTime;
    time(&curTime);
    if(isuserprg == 0){
        //user prg
        User_prgInfo_t *userinfo = NULL;
        if(prgoptflag == 0){ //edit
            if((&outpst->userPrgNodes != NULL)&&(list_len(&outpst->userPrgNodes)>0)){
                for(i=0; i<list_len(&outpst->userPrgNodes); i++){
                    list_get(&outpst->userPrgNodes, i, &userinfo);
                    if(userinfo->index == orgiralindex){
                        userinfo->prgNum = prgNum;
                        userinfo->pmtPid = pmtPid;
                        userinfo->pcrPidInChn = chnid;
                        userinfo->oldPcrPid = oldpcrpid;
                        userinfo->newPcrPid = newpcrpid;
                        userinfo->serviceType = servicetype;
                        userinfo->prgNameLen = strlen(prgname);
                        memcpy(userinfo->prgName, prgname, userinfo->prgNameLen);
                        userinfo->providerNameLen = strlen(providername);
                        memcpy(userinfo->providerName, providername, userinfo->providerNameLen);
                        //comment
                        free(userinfo->psdtDesList->data);
                        free(userinfo->psdtDesList);
                        userinfo->psdtDesListLen = 0;
                        Commdes_t *sdtDes = malloc(sizeof(Commdes_t));
                        int iAddr = 0;
                        sdtDes->tag = 0x48;
                        sdtDes->index = 1;
                        sdtDes->userNew = 1;
                        char sdtdata[3 + strlen(prgname) + 1 + strlen(providername)];
                        sdtdata[iAddr++] = (unsigned char)sizeof(sdtdata);
                        sdtdata[iAddr++] = (unsigned char)servicetype;
                        sdtdata[iAddr++] = (unsigned char)strlen(providername);
                        if(strlen(providername)>0){
                            memcpy(sdtdata+iAddr, providername, strlen(providername));
                            iAddr += strlen(providername);
                        }
                        sdtdata[iAddr++] = (unsigned char)strlen(prgname);
                        if(strlen(prgname)>0){
                            memcpy(sdtdata+iAddr, prgname, strlen(prgname));
                            iAddr += strlen(prgname);
                        }
                        sdtDes->dataLen = sizeof(sdtdata);
                        sdtDes->data = malloc(sdtDes->dataLen);
                        memset(sdtDes->data, 0, sdtDes->dataLen);
                        memcpy(sdtDes->data, sdtdata, sdtDes->dataLen);
                        if(userinfo->psdtDesListLen == 0){
                            userinfo->psdtDesList = sdtDes;
                            userinfo->psdtDesListLen = 1;
                        }else{

                        }
                        //修改节目数据流信息
                        //重新分配新的数据流内存
                        DataStream_t *StreamList = malloc(streamcnt * sizeof(DataStream_t));
                        DataStream_t *pdataStreamInfo = StreamList;
                        for(k=0; k<streamcnt; k++){
                            DataStream_t *oldpdataStreamInfo = userinfo->pdataStreamList;
                            memset(rsts, 0, sizeof(rsts));
                            sprintf(rsts, "index%d", k);
                            index = atoi(mprGetJson(jsonparam, rsts));
                            matched = 0;
                            for(j=0; j<userinfo->pdataStreamListLen; j++){
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
                            sscanf(mprGetJson(jsonparam, rsts), "%x", &val);
                            pdataStreamInfo->inPid = val;
                            memset(rsts, 0, sizeof(rsts));
                            sprintf(rsts, "r_outpid%d", k);
                            sscanf(mprGetJson(jsonparam, rsts), "%x", &val);
                            pdataStreamInfo->outPid = val;
                            //pdataStreamInfo->outPid = atoi(mprGetJson(jsonparam, rsts));
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
                        if(userinfo->pdataStreamListLen>0){
                            for(k=0; k< userinfo->pdataStreamListLen; k++){
                                for(j=0; j < userinfo->pdataStreamList[k].desNodeLen; j++)
                                {
                                    if(userinfo->pdataStreamList[k].desNode[j].dataLen){
                                        free(userinfo->pdataStreamList[k].desNode[j].data);
                                        userinfo->pdataStreamList[k].desNode[j].data = NULL;
                                    }
                                }
                                if(userinfo->pdataStreamList[k].desNodeLen>0){
                                    free(userinfo->pdataStreamList[k].desNode);
                                    userinfo->pdataStreamList[k].desNode = NULL;
                                }
                            }
                            free(userinfo->pdataStreamList);
                            userinfo->pdataStreamList = NULL;
                        }
                        //重新指向新的内存空间
                        userinfo->pdataStreamListLen = streamcnt;
                        userinfo->pdataStreamList = StreamList;
                    }
                }
                memset(optstr, 0, 256);
                sprintf(optstr, "{'user': '%s', 'desc': '用户修改了自定义节目【pmtPid:%d】信息.', 'level': '1', 'logtime':'%d'}", getSessionVar("userName"),pmtPid, curTime);
            }
        }else{ //add
            userinfo = malloc(sizeof(User_prgInfo_t));
            userinfo->index = GetNewFreeUserPrgIndex(0, inCh);
            userinfo->prgNum = prgNum;
            userinfo->pmtPid = pmtPid;
            userinfo->streamId = 1;
            userinfo->pcrPidInChn = chnid;
            userinfo->oldPcrPid = oldpcrpid;
            userinfo->newPcrPid = newpcrpid;
            userinfo->serviceType = servicetype;
            userinfo->prgNameLen = strlen(prgname);
            userinfo->prgName = malloc(userinfo->prgNameLen);
            memcpy(userinfo->prgName, prgname, userinfo->prgNameLen);
            userinfo->providerNameLen = strlen(providername);
            userinfo->providerName = malloc(userinfo->providerNameLen);
            memcpy(userinfo->providerName, providername, userinfo->providerNameLen);
            userinfo->pmtDesListLen = 0;
            userinfo->psdtDesListLen = 0;
            //验证
            if(userinfo->pmtPid<0 || userinfo->pmtPid>0x1fff){
                rendersts(rsts, 3);
                render(rsts);
                return;
            }
            if(userinfo->oldPcrPid<0 || userinfo->oldPcrPid>0x1fff || userinfo->newPcrPid<0 || userinfo->newPcrPid>0x1fff){
                rendersts(rsts, 4);
                render(rsts);
                return;
            }
            if(userinfo->prgNum<0 || userinfo->prgNum>0x1ffff){
                rendersts(rsts, 5);
                render(rsts);
                return;
            }
            if(userinfo->pcrPidInChn<0 || userinfo->pcrPidInChn>clsProgram._intChannelCntMax ){
                rendersts(rsts, 5);
                render(rsts);
                return;
            }
            //comment
            Commdes_t *sdtDes = malloc(sizeof(Commdes_t));
            int iAddr = 0;
            sdtDes->tag = 0x48;
            sdtDes->index = 1;
            sdtDes->userNew = 1;
            char sdtdata[3 + strlen(prgname) + 1 + strlen(providername)];
            sdtdata[iAddr++] = (unsigned char)sizeof(sdtdata);
            sdtdata[iAddr++] = (unsigned char)servicetype;
            sdtdata[iAddr++] = (unsigned char)strlen(providername);
            if(strlen(providername)>0){
                memcpy(sdtdata+iAddr, providername, strlen(providername));
                iAddr += strlen(providername);
            }
            sdtdata[iAddr++] = (unsigned char)strlen(prgname);
            if(strlen(prgname)>0){
                memcpy(sdtdata+iAddr, prgname, strlen(prgname));
                iAddr += strlen(prgname);
            }
            sdtDes->dataLen = sizeof(sdtdata);
            sdtDes->data = malloc(sdtDes->dataLen);
            memset(sdtDes->data, 0, sdtDes->dataLen);
            memcpy(sdtDes->data, sdtdata, sdtDes->dataLen);
            if(userinfo->psdtDesListLen == 0){
                userinfo->psdtDesList = sdtDes;
                userinfo->psdtDesListLen = 1;
            }else{

            }

            DataStream_t *StreamList = malloc(streamcnt * sizeof(DataStream_t));
            DataStream_t *pdataStreamInfo = StreamList;
            for(k=0; k<streamcnt; k++){
                memset(rsts, 0, sizeof(rsts));
                sprintf(rsts, "index%d", k);
                index = atoi(mprGetJson(jsonparam, rsts));
                //修改新数据流
                memset(rsts, 0, sizeof(rsts));
                sprintf(rsts, "r_streamtype%d", k);
                pdataStreamInfo->streamType = atoi(mprGetJson(jsonparam, rsts));
                memset(rsts, 0, sizeof(rsts));
                sprintf(rsts, "r_inpid%d", k);
                sscanf(mprGetJson(jsonparam, rsts), "%x", &val);
                pdataStreamInfo->inPid = val;
                memset(rsts, 0, sizeof(rsts));
                sprintf(rsts, "r_outpid%d", k);
                sscanf(mprGetJson(jsonparam, rsts), "%x", &val);
                pdataStreamInfo->outPid = val;
                pdataStreamInfo->index = index;
                pdataStreamInfo->inChn = chnid;
                pdataStreamInfo->desNodeLen = 0;

                pdataStreamInfo++;
            }
            userinfo->pdataStreamListLen = streamcnt;
            userinfo->pdataStreamList = StreamList;
            list_append(&outpst->userPrgNodes, userinfo);
            memset(optstr, 0, 256);
            sprintf(optstr, "{'user': '%s', 'desc': '用户添加了自定义节目【pmtPid:%d】信息.', 'level': '1', 'logtime':'%d'}", getSessionVar("userName"),pmtPid, curTime);
        }
    }else{
        //dev prg
        if(prgoptflag == 0){
            //查找编辑节目
            for(i=0; i<list_len(&outpst->prgNodes); i++){
                list_get(&outpst->prgNodes, i, &outprg);
                if((outprg->index == orgiralindex)&&(outprg->chnId == chnid)){
                    //printf("============pmtpid===%d", pmtPid);
                    outprg->prgNum = prgNum;
                    outprg->pmtPid = pmtPid;
                    outprg->oldPcrPid = oldpcrpid;
                    outprg->newPcrPid = newpcrpid;
                    outprg->serviceType = servicetype;
                    outprg->prgNameLen = strlen(prgname);
                    memcpy(outprg->prgName, prgname, outprg->prgNameLen);
                    outprg->providerNameLen = strlen(providername);
                    memcpy(outprg->providerName, providername, outprg->providerNameLen);
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
                        sscanf(mprGetJson(jsonparam, rsts), "%x", &val);
                        pdataStreamInfo->inPid = val;
                        memset(rsts, 0, sizeof(rsts));
                        sprintf(rsts, "r_outpid%d", k);
                        sscanf(mprGetJson(jsonparam, rsts), "%x", &val);
                        pdataStreamInfo->outPid = val;
                        //pdataStreamInfo->outPid = atoi(mprGetJson(jsonparam, rsts));
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
                memset(optstr, 0, 256);
                sprintf(optstr, "{'user': '%s', 'desc': '用户修改了节目【pmtPid:%d】信息.', 'level': '1', 'logtime':'%d'}", getSessionVar("userName"),pmtPid, curTime);
            }
        }else if(prgoptflag == 1){
            Dev_prgInfo_st *newprg = malloc(sizeof(Dev_prgInfo_st));
            newprg->index = GetNewFreePrgIndex(chnid, inCh);
            newprg->userNew = 1;
            newprg->prgNum = prgNum;
            newprg->streamId = 1;
            newprg->pmtPid = pmtPid;
            newprg->oldPcrPid = oldpcrpid;
            newprg->newPcrPid = newpcrpid;
            newprg->serviceType = servicetype;
            newprg->chnId = chnid;
            newprg->prgNameLen = strlen(prgname);
            newprg->prgName = malloc(newprg->prgNameLen);
            memcpy(newprg->prgName, prgname, newprg->prgNameLen);
            newprg->providerNameLen = strlen(providername);
            newprg->providerName = malloc(newprg->providerNameLen);
            memcpy(newprg->providerName, providername, newprg->providerNameLen);
            //验证
            if(newprg->pmtPid<0 || newprg->pmtPid>0x1fff){
                rendersts(rsts, 3);
                render(rsts);
                return;
            }
            if(newprg->oldPcrPid<0 || newprg->oldPcrPid>0x1fff || newprg->newPcrPid<0 || newprg->newPcrPid>0x1fff){
                rendersts(rsts, 4);
                render(rsts);
                return;
            }
            if(newprg->prgNum<0 || newprg->prgNum>0x1ffff){
                rendersts(rsts, 5);
                render(rsts);
                return;
            }
            DataStream_t *StreamList = malloc(streamcnt * sizeof(DataStream_t));
            DataStream_t *pdataStreamInfo = StreamList;
            for(k=0; k<streamcnt; k++){
                memset(rsts, 0, sizeof(rsts));
                sprintf(rsts, "index%d", k);
                index = atoi(mprGetJson(jsonparam, rsts));
                //修改新数据流
                memset(rsts, 0, sizeof(rsts));
                sprintf(rsts, "r_streamtype%d", k);
                pdataStreamInfo->streamType = atoi(mprGetJson(jsonparam, rsts));
                memset(rsts, 0, sizeof(rsts));
                sprintf(rsts, "r_inpid%d", k);
                sscanf(mprGetJson(jsonparam, rsts), "%x", &val);
                pdataStreamInfo->inPid = val;
                memset(rsts, 0, sizeof(rsts));
                sprintf(rsts, "r_outpid%d", k);
                sscanf(mprGetJson(jsonparam, rsts), "%x", &val);
                pdataStreamInfo->outPid = val;
                pdataStreamInfo->index = index;
                pdataStreamInfo->inChn = chnid;
                pdataStreamInfo->desNodeLen = 0;

                pdataStreamInfo++;
            }
            newprg->pdataStreamListLen = streamcnt;
            newprg->pdataStreamList = StreamList;
            //add sdt comment
            newprg->psdtDesListLen = 0;
            //TODO
//            Commdes_t *sdtDes = malloc(sizeof(Commdes_t));
//            sdtDes->tag = 0x48;
//            sdtDes->index = 1;
//            sdtDes->userNew = 1;
            newprg->pmtDesListLen = 0;
            list_append(&outpst->prgNodes, newprg);
            memset(optstr, 0, 256);
            sprintf(optstr, "{'user': '%s', 'desc': '用户添加了自定义节目【pmtPid:%d】信息.', 'level': '1', 'logtime':'%d'}", getSessionVar("userName"),pmtPid, curTime);
        }
    }
	rendersts(rsts, 1);
	render(rsts);
	//add optlog
    Edi *db = ediOpen("db/muxnms.mdb", "mdb", EDI_AUTO_SAVE);
    EdiRec *optlog = ediCreateRec(db, "optlog");
    if(optlog == NULL){
       printf("================>>>optlog is NULL!!\n");
       return;
    }
    MprJson  *row = mprParseJson(optstr);
    if(ediSetFields(optlog, row) == 0){
       printf("================>>>ediSetFields Failed!!\n");
    }
    ediUpdateRec(db, optlog);
    //ediClose(db);
}

static void common(HttpConn *conn) {
	
}

static void getglobalinfo(HttpConn *conn) { 
	char str[64] = {0};
    cchar *role = getSessionVar("role");
    if(role == NULL){
        rendersts(str, 9);
        render(str);
        return;
    }
	cJSON *result = cJSON_CreateObject();
	char* jsonstring;
	cJSON_AddNumberToObject(result, "_intChannelCntMax", clsProgram._intChannelCntMax);
	cJSON_AddNumberToObject(result, "sts", 0);
	jsonstring = cJSON_PrintUnformatted(result);
	memcpy(str, jsonstring, strlen(jsonstring));
	//释放内存	
	cJSON_Delete(result);		
	free(jsonstring);
	render(str);
}

static void search(HttpConn *conn) {
	char str[64] = {0};
	int i = 0, rst = 0;
    char* jsonstring;
    MprJson *jsonparam = httpGetParams(conn);
    //printf("==========setprginfo===========%s\n", mprJsonToString (jsonparam, MPR_JSON_QUOTES));
    int inCh = atoi(mprGetJson(jsonparam, "inch"));
    rst = Search(conn->rx->parsedUri->host, inCh);

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
    //add optlog
    if(inCh == 1){
        Edi *db = ediOpen("db/muxnms.mdb", "mdb", EDI_AUTO_SAVE);
        EdiRec *optlog = ediCreateRec(db, "optlog");
        if(optlog == NULL){
           printf("================>>>optlog is NULL!!\n");
           return;
        }
        time_t curTime;
        time(&curTime);
        memset(optstr, 0, 256);
        sprintf(optstr, "{'user': '%s', 'desc': '用户节目搜索.', 'level': '2', 'logtime':'%d'}", getSessionVar("userName"), curTime);
        MprJson  *row = mprParseJson(optstr);
        if(ediSetFields(optlog, row) == 0){
           printf("================>>>ediSetFields Failed!!\n");
        }
        ediUpdateRec(db, optlog);
        //ediClose(db);
    }
}

static void reprgnum(HttpConn *conn) {
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
	User_prgInfo_t *userprg = NULL;
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
            //clsProgram.userPrgNodes
            if((&outpst->userPrgNodes != NULL) && (list_len(&outpst->userPrgNodes)>0)){
                for(j=0; j<list_len(&outpst->userPrgNodes); j++){
                    list_get(&outpst->userPrgNodes, j, &userprg);
                    memcpy(&userprg->prgNum, &prgNumCnt, sizeof(int));
                    prgNumCnt++;
                }
            }
        }
    }
    rendersts(str, 1);
    render(str);
    //add optlog
    Edi *db = ediOpen("db/muxnms.mdb", "mdb", EDI_AUTO_SAVE);
    EdiRec *optlog = ediCreateRec(db, "optlog");
    if(optlog == NULL){
       printf("================>>>optlog is NULL!!\n");
       return;
    }
    time_t curTime;
    time(&curTime);
    memset(optstr, 0, 256);
    sprintf(optstr, "{'user': '%s', 'desc': '重新分配节目号.', 'level': '2', 'logtime':'%d'}", getSessionVar("userName"), curTime);
    MprJson  *row = mprParseJson(optstr);
    if(ediSetFields(optlog, row) == 0){
       printf("================>>>ediSetFields Failed!!\n");
    }
    ediUpdateRec(db, optlog);
    //ediClose(db);
}

static void reprgpid(HttpConn *conn) {
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
	User_prgInfo_t *userprg = NULL;
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

            //clsProgram.userPrgNodes
            if((&outpst->userPrgNodes != NULL) && (list_len(&outpst->userPrgNodes)>0)){
                for(j=0; j<list_len(&outpst->userPrgNodes); j++){
                    list_get(&outpst->userPrgNodes, j, &userprg);
                    int newPid = SeekReplacedPid(usingPidList, 9, userprg->pmtPid, pidPrgStart);
                    if (newPid != userprg->pmtPid || pidPrgStart == userprg->pmtPid)
                    {
                        memcpy(&userprg->pmtPid, &newPid, sizeof(int));
                        pidPrgStart++;
                    }
                    if (userprg->newPcrPid != 0x1fff)
                    {
                        newPid = SeekReplacedPid(usingPidList, userprg->pcrPidInChn , userprg->oldPcrPid, pidAvStart);
                        if (newPid != userprg->oldPcrPid || pidAvStart == userprg->oldPcrPid)
                        {
                            memcpy(&userprg->newPcrPid, &newPid, sizeof(int));
                            pidAvStart++;
                        }
                    }
                    pidAvStart = DesPidRefresh2(9, userprg->index, -1, userprg->pmtDesList, userprg->pmtDesListLen, pidAvStart, NULL);
                    if(userprg->pdataStreamList != NULL && userprg->pdataStreamListLen>0){
                        DataStream_t *dst = userprg->pdataStreamList;
                        for(k=0;k<userprg->pdataStreamListLen;k++){
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
                            pidAvStart = DesPidRefresh2(9, userprg->index, dst->index,
                                        dst->desNode, dst->desNodeLen, pidAvStart, NULL);
                            dst++;
                        }
                    }
                }
            }
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
    //add optlog
    Edi *db = ediOpen("db/muxnms.mdb", "mdb", EDI_AUTO_SAVE);
    EdiRec *optlog = ediCreateRec(db, "optlog");
    if(optlog == NULL){
       printf("================>>>optlog is NULL!!\n");
       return;
    }
    time_t curTime;
    time(&curTime);
    memset(optstr, 0, 256);
    sprintf(optstr, "{'user': '%s', 'desc': '重新分配PID.', 'level': '2', 'logtime':'%d'}", getSessionVar("userName"), curTime);
    MprJson  *row = mprParseJson(optstr);
    if(ediSetFields(optlog, row) == 0){
       printf("================>>>ediSetFields Failed!!\n");
    }
    ediUpdateRec(db, optlog);
    //ediClose(db);
}

/*
    Dynamic module initialization
 */
ESP_EXPORT int esp_controller_muxnms_programs(HttpRoute *route, MprModule *module) {
    espDefineBase(route, common);
    espDefineAction(route, "programs-cmd-getprg", getprg);
	espDefineAction(route, "programs-cmd-getprginfo", getprginfo);//menu-edit
	espDefineAction(route, "programs-cmd-setprginfo", setprginfo);//menu-edit
	espDefineAction(route, "programs-cmd-selectprgs", selectprgs);
	espDefineAction(route, "programs-cmd-getoutprg", getoutprg);
	espDefineAction(route, "programs-cmd-maketable", maketable);
	espDefineAction(route, "programs-cmd-streamtable", streamtable);
	espDefineAction(route, "programs-cmd-writetable", writetable);
	espDefineAction(route, "programs-cmd-getglobalinfo", getglobalinfo);
    espDefineAction(route, "programs-cmd-getchanneloutinfo", getchanneloutinfo);
	espDefineAction(route, "programs-cmd-setchanneloutinfo", setchanneloutinfo);
	espDefineAction(route, "programs-cmd-getpidtransinfo", getpidtransinfo);
	espDefineAction(route, "programs-cmd-setpidtransinfo", setpidtransinfo);
	espDefineAction(route, "programs-cmd-search", search);
	espDefineAction(route, "programs-cmd-reprgnum", reprgnum);
	espDefineAction(route, "programs-cmd-reprgpid", reprgpid);
	espDefineAction(route, "programs-cmd-gettableinfo", gettableinfo);

#if SAMPLE_VALIDATIONS
    Edi *edi = espGetRouteDatabase(route);
    ediAddValidation(edi, "present", "programs", "title", 0);
    ediAddValidation(edi, "unique", "programs", "title", 0);
    ediAddValidation(edi, "banned", "programs", "body", "(swear|curse)");
    ediAddValidation(edi, "format", "programs", "phone", "/^\\(?([0-9]{3})\\)?[-. ]?([0-9]{3})[-. ]?([0-9]{4})$/");
#endif
    return 0;
}
