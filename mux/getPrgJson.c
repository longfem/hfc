#include <string.h>

#include "datastructdef.h"
#include "getPrograms.h"
#include "cJSON.h"

ClsProgram_st clsProgram;

void getprgsJson(char *ip, int inChn, char *outprg){
	int i = 0, j = 0, res = 0;	
	char str[200] = {0};
	char idstr[100] = {0};
	Dev_prgInfo_st *ptmpPrgInfo= NULL;
	Dev_prgInfo_st *PrgInfo;
	ChannelProgramSt *pst = NULL;
	//释放输入通道节目占用的内存
	list_get(&(clsProgram.inPrgList), inChn-1, &pst);
	if( list_len(&pst->prgNodes) > 0){
		freePrograms(&pst->prgNodes);
	}
	Chn_ca_st *caNode = &pst->caNode;
    list_t *caIdenList = &caNode->caIdenList;
    freecanode(caIdenList);
    res = getPrograms(ip, inChn, &pst->prgNodes);
	if(0 != res){
		GetCatDesList(ip, inChn, caIdenList);
		cJSON *prgjson,*channelsarray,*channeljson,*subprgjson,*subprgsarray,*streamjson,*streamsarray,*audiosarray,*prgsarray;//*prgsjson,
		char* prgjsonstring;
		channelsarray = cJSON_CreateArray();
		cJSON_AddItemToArray(channelsarray,channeljson = cJSON_CreateObject());
		cJSON_AddTrueToObject(channeljson,"folder");
		cJSON_AddTrueToObject(channeljson,"expanded");
		sprintf(idstr, "id1.%d", inChn);//1.2	
		cJSON_AddStringToObject(channeljson,"key", idstr);
		cJSON_AddStringToObject(channeljson,"icon", "img/channel_in.ico");
		cJSON_AddItemToObject(channeljson, "children", prgsarray = cJSON_CreateArray());
		for(i=0; i<list_len(&pst->prgNodes); i++) {
			cJSON_AddItemToArray(prgsarray,prgjson = cJSON_CreateObject());
			ptmpPrgInfo = NULL;
			list_get(&pst->prgNodes, i, &ptmpPrgInfo);
			//添加节目节点TITLE					
			memset(idstr, 0, sizeof(idstr));
			memcpy(idstr, ptmpPrgInfo->prgName, ptmpPrgInfo->prgNameLen);
			sprintf(str,"节目%d(0X%x):PID(0X%x) PCR_PID(0X%x) - %s",ptmpPrgInfo->prgNum, ptmpPrgInfo->prgNum, ptmpPrgInfo->pmtPid, ptmpPrgInfo->newPcrPid, idstr );
			memset(idstr, 0, sizeof(idstr));
			cJSON_AddStringToObject(prgjson,"title", str);
			cJSON_AddTrueToObject(prgjson,"folder");
			cJSON_AddFalseToObject(prgjson,"expanded");
			cJSON_AddNumberToObject(prgjson, "index", ptmpPrgInfo->index);
			cJSON_AddNumberToObject(prgjson, "pmtPid", ptmpPrgInfo->pmtPid);
			cJSON_AddNumberToObject(prgjson, "chnid", ptmpPrgInfo->chnId);
			sprintf(idstr, "id1.%d.%d", inChn, ptmpPrgInfo->index);//1.2.1
			cJSON_AddStringToObject(prgjson,"key", idstr);
			cJSON_AddStringToObject(prgjson,"icon", "img/notebook.ico");

			cJSON_AddItemToObject(prgjson, "children", subprgsarray = cJSON_CreateArray());
			//subprgjson
			cJSON_AddItemToArray(subprgsarray,subprgjson = cJSON_CreateObject());
			cJSON_AddStringToObject(subprgjson,"title", "maximun bitrate descriptor");
			cJSON_AddTrueToObject(subprgjson,"folder");
			cJSON_AddFalseToObject(subprgjson,"expanded");
			sprintf(idstr, "id1.%d.%d.1", inChn, ptmpPrgInfo->index);
			cJSON_AddStringToObject(subprgjson,"key", idstr);
			cJSON_AddStringToObject(subprgjson,"icon", "img/channel_in.ico");
			
			cJSON_AddItemToArray(subprgsarray,subprgjson = cJSON_CreateObject());	
			sprintf(idstr, "id1.%d.%d.2", inChn, ptmpPrgInfo->index);
			cJSON_AddStringToObject(subprgjson,"title", "multiplex buffer utilization descriptor");
			cJSON_AddTrueToObject(subprgjson,"folder");
			cJSON_AddFalseToObject(subprgjson,"expanded");
			cJSON_AddStringToObject(subprgjson,"key", idstr);
			cJSON_AddStringToObject(subprgjson,"icon", "img/channel_in.ico");
			int j=0, k=0;
			//PMT
			Commdes_t *tmpinfo = malloc(sizeof(Commdes_t));
			int offset = 0, flag = 0;
			//unsigned char tmpstr[100] = {0};
			for(j=0; j<ptmpPrgInfo->pmtDesListLen; j++) {
				memcpy(tmpinfo, ptmpPrgInfo->pmtDesList+offset, sizeof(Commdes_t) );
				offset += 1;
			}
			free(tmpinfo);
			//stream
			offset = 0;
			DataStream_t *streaminfo = malloc(sizeof(DataStream_t));
			for(j=0; j<ptmpPrgInfo->pdataStreamListLen; j++) {			
				memcpy(streaminfo, ptmpPrgInfo->pdataStreamList+offset, sizeof(DataStream_t) );
				//printf("-----%x-->>%x\n", streaminfo->inPid, streaminfo->streamType);
				offset += 1;				
				if(2 == streaminfo->streamType){
					//MPEG2 VIDEO
					cJSON_AddItemToArray(subprgsarray,subprgjson = cJSON_CreateObject());
					sprintf(idstr, "id1.%d.%d.%d", inChn, ptmpPrgInfo->index, streaminfo->index);
					sprintf(str,"MPEG2 VIDEO  [PID(0x%x)]",  streaminfo->inPid);
					cJSON_AddStringToObject(subprgjson,"title", str);
					cJSON_AddTrueToObject(subprgjson,"folder");
					cJSON_AddFalseToObject(subprgjson,"expanded");
					cJSON_AddNumberToObject(subprgjson, "index", streaminfo->index);
					cJSON_AddStringToObject(subprgjson,"key", idstr);
					cJSON_AddStringToObject(subprgjson,"icon", "img/favicon.ico");
					
					cJSON_AddItemToObject(subprgjson, "children", streamsarray = cJSON_CreateArray());
					cJSON_AddItemToArray(streamsarray,streamjson = cJSON_CreateObject());
					sprintf(idstr, "id1.%d.%d.%d.1", inChn,  ptmpPrgInfo->index, streaminfo->index);
					cJSON_AddStringToObject(streamjson,"title", "video stream descriptor");
					cJSON_AddTrueToObject(streamjson,"folder");
					cJSON_AddFalseToObject(streamjson,"expanded");
					cJSON_AddStringToObject(streamjson,"key", idstr);
					cJSON_AddStringToObject(streamjson,"icon", "img/channel_in.ico");
					
					cJSON_AddItemToArray(streamsarray,streamjson = cJSON_CreateObject());
					sprintf(idstr, "id1.%d.%d.%d.2", inChn, ptmpPrgInfo->index, streaminfo->index);
					cJSON_AddStringToObject(streamjson,"title", "data stream alignment descriptor");
					cJSON_AddTrueToObject(streamjson,"folder");
					cJSON_AddFalseToObject(streamjson,"expanded");
					cJSON_AddStringToObject(streamjson,"key", idstr);
					cJSON_AddStringToObject(streamjson,"icon", "img/channel_in.ico");
					
					cJSON_AddItemToArray(streamsarray,streamjson = cJSON_CreateObject());
					sprintf(idstr, "id1.%d.%d.%d.3", inChn, ptmpPrgInfo->index, streaminfo->index);
					cJSON_AddStringToObject(streamjson,"title", "maximum bitrate descriptor");
					cJSON_AddTrueToObject(streamjson,"folder");
					cJSON_AddFalseToObject(streamjson,"expanded");
					cJSON_AddStringToObject(streamjson,"key", idstr);
					cJSON_AddStringToObject(streamjson,"icon", "img/channel_in.ico");
					
					cJSON_AddItemToArray(streamsarray,streamjson = cJSON_CreateObject());
					sprintf(idstr, "id1.%d.%d.%d.4", inChn, ptmpPrgInfo->index, streaminfo->index);
					cJSON_AddStringToObject(streamjson,"title", "stream identifier descriptor");
					cJSON_AddTrueToObject(streamjson,"folder");
					cJSON_AddFalseToObject(streamjson,"expanded");
					cJSON_AddStringToObject(streamjson,"key", idstr);
					cJSON_AddStringToObject(streamjson,"icon", "img/channel_in.ico");
					//cJSON_AddItemToObject(subprgjson, "children", streamsarray);
				}else if(4 == streaminfo->streamType){
					//MPEG2 AUDIO
					cJSON_AddItemToArray(subprgsarray,subprgjson = cJSON_CreateObject());
					sprintf(idstr, "id1.%d.%d.%d", inChn, ptmpPrgInfo->index, streaminfo->index);
					sprintf(str,"MPEG2 AUDIO  [PID(0x%x)]",  streaminfo->inPid);
					cJSON_AddStringToObject(subprgjson,"title", str);
					cJSON_AddTrueToObject(subprgjson,"folder");
					cJSON_AddFalseToObject(subprgjson,"expanded");
					cJSON_AddNumberToObject(subprgjson, "index", streaminfo->index);
					cJSON_AddStringToObject(subprgjson,"key", idstr);
					cJSON_AddStringToObject(subprgjson,"icon", "img/audio.ico");
					
					//stream json
					cJSON_AddItemToObject(subprgjson, "children", audiosarray = cJSON_CreateArray());
					cJSON_AddItemToArray(audiosarray,streamjson = cJSON_CreateObject());
					sprintf(idstr, "id1.%d.%d.%d.1", inChn, ptmpPrgInfo->index, streaminfo->index);
					cJSON_AddStringToObject(streamjson,"title", "ISO 639 language descriptor");
					cJSON_AddTrueToObject(streamjson,"folder");
					cJSON_AddFalseToObject(streamjson,"expanded");
					cJSON_AddStringToObject(streamjson,"key", idstr);
					cJSON_AddStringToObject(streamjson,"icon", "img/channel_in.ico");
					
					cJSON_AddItemToArray(audiosarray,streamjson = cJSON_CreateObject());
					sprintf(idstr, "id1.%d.%d.%d.2", inChn, ptmpPrgInfo->index, streaminfo->index);
					cJSON_AddStringToObject(streamjson,"title", "audio stream descriptor");
					cJSON_AddTrueToObject(streamjson,"folder");
					cJSON_AddFalseToObject(streamjson,"expanded");
					cJSON_AddStringToObject(streamjson,"key", idstr);
					cJSON_AddStringToObject(streamjson,"icon", "img/channel_in.ico");
					
					cJSON_AddItemToArray(audiosarray,streamjson = cJSON_CreateObject());
					sprintf(idstr, "id1.%d.%d.%d.3", inChn, ptmpPrgInfo->index, streaminfo->index);
					cJSON_AddStringToObject(streamjson,"title", "maximum bitrate descriptor");
					cJSON_AddTrueToObject(streamjson,"folder");
					cJSON_AddFalseToObject(streamjson,"expanded");
					cJSON_AddStringToObject(streamjson,"key", idstr);
					cJSON_AddStringToObject(streamjson,"icon", "img/channel_in.ico");
				}				
				Commdes_t *desNodeinfo = malloc(sizeof(Commdes_t));
				for(k=0; k<streaminfo->desNodeLen; k++) {
					memcpy(desNodeinfo, streaminfo->desNode+flag, sizeof(Commdes_t) );
					//memset(tmpstr, 0, sizeof(tmpstr));
					//memcpy(tmpstr, desNodeinfo->data, desNodeinfo->dataLen );
					//printf("---desNodeinfo--%d-->>%d::::%d:::%d:::%d\n", streaminfo->desNodeLen, tmpstr[0],tmpstr[1],tmpstr[2],tmpstr[3]);
					flag += 1;
					
				}
				free(desNodeinfo);
			}
			free(streaminfo);
		}
		//添加CAT
		CA_descriptor *cades = NULL;
		cJSON_AddItemToArray(prgsarray,prgjson = cJSON_CreateObject());
        cJSON_AddStringToObject(prgjson,"title", "CAT");
        cJSON_AddTrueToObject(prgjson,"folder");
        cJSON_AddFalseToObject(prgjson,"expanded");
        cJSON_AddNumberToObject(prgjson, "chnid", inChn);
        memset(idstr, 0, sizeof(idstr));
        sprintf(idstr, "id1.%d.cat", inChn);//1.2.1
        cJSON_AddStringToObject(prgjson,"key", idstr);
        cJSON_AddStringToObject(prgjson,"icon", "img/star.ico");
        if(list_len(caIdenList)>0){
            cJSON_AddItemToObject(prgjson, "children", subprgsarray = cJSON_CreateArray());
            for(j=0;j<list_len(caIdenList);j++){
                list_get(caIdenList, j, &cades);
                //subprgjson
                cJSON_AddItemToArray(subprgsarray,subprgjson = cJSON_CreateObject());
                sprintf(idstr, "id1.%d.cat.%d", inChn, cades->index);
                cJSON_AddStringToObject(subprgjson,"key", idstr);
                cJSON_AddNumberToObject(subprgjson,"index", cades->index);
                cJSON_AddNumberToObject(subprgjson, "chnid", cades->inChannel);
                cJSON_AddStringToObject(subprgjson,"icon", "img/circle16_green.ico");
                memset(str, 0, sizeof(str));
                sprintf(str,"CA descriptor SysID=%d(0x%x) PID(0x%x)",cades->inCaSysId, cades->inCaSysId, cades->inCaPid );
                cJSON_AddStringToObject(subprgjson,"title", str);
            }
        }

		//添加通道节点TITLE
		sprintf(str,"通道%d(ASI-%d)  -  原始网络ID=0x%04X,传输流ID=0x%04X",ptmpPrgInfo->chnId, ptmpPrgInfo->chnId, ptmpPrgInfo->networkId, ptmpPrgInfo->streamId );
		cJSON_AddStringToObject(channeljson,"title", str);

		prgjsonstring = cJSON_PrintUnformatted(channelsarray);
		
		memcpy(outprg, prgjsonstring, strlen(prgjsonstring));
		printf("---prgjsonlen--->>>%d\n",strlen(prgjsonstring));
		//释放内存	
		cJSON_Delete(channelsarray);
		free(prgjsonstring);
	}else{
		cJSON *channelsarray,*channeljson;
		char* prgjsonstring;
		channelsarray = cJSON_CreateArray();
		cJSON_AddItemToArray(channelsarray,channeljson = cJSON_CreateObject());
		cJSON_AddTrueToObject(channeljson,"folder");
		cJSON_AddTrueToObject(channeljson,"expanded");
		sprintf(idstr, "id1.%d", inChn);//1.2	
		cJSON_AddStringToObject(channeljson,"key", idstr);
		cJSON_AddStringToObject(channeljson,"icon", "img/channel_in.ico");
		//添加通道节点TITLE
		sprintf(str,"通道%d(ASI-%d)", inChn, inChn);
		cJSON_AddStringToObject(channeljson,"title", str);
		prgjsonstring = cJSON_PrintUnformatted(channelsarray);		
		memcpy(outprg, prgjsonstring, strlen(prgjsonstring));
		
		//释放内存	
		cJSON_Delete(channelsarray);		
		free(prgjsonstring);
	}
	
}

void adduserprgjson(cJSON *basearry, ChannelProgramSt *pst){
    int i = 0, j = 0;
    char str[200] = {0};
    char idstr[100] = {0};
    cJSON *basejson,*prgjson,*subprgjson,*subprgsarray,*streamjson,*streamsarray,*audiosarray,*prgsarray;
    User_prgInfo_t *userprg = NULL;
    for(i=0; i<list_len(&pst->userPrgNodes); i++) {
        list_get(&pst->userPrgNodes, i, &userprg);
        cJSON_AddItemToArray(basearry,basejson = cJSON_CreateObject());
        cJSON_AddNumberToObject(basejson, "ch", 9);
        cJSON_AddNumberToObject(basejson, "index", userprg->index);
        cJSON_AddItemToObject(basejson, "children", prgjson = cJSON_CreateObject());

        //添加节目节点TITLE
        memset(idstr, 0, sizeof(idstr));
        memcpy(idstr, userprg->prgName, userprg->prgNameLen);
        sprintf(str,"节目%d(0X%x):PID(0X%x) PCR_PID(0X%x) - %s",userprg->prgNum, userprg->prgNum, userprg->pmtPid, userprg->newPcrPid, idstr );
        memset(idstr, 0, sizeof(idstr));
        cJSON_AddStringToObject(prgjson,"title", str);
        cJSON_AddTrueToObject(prgjson,"folder");
        cJSON_AddFalseToObject(prgjson,"expanded");
        cJSON_AddStringToObject(prgjson, "extraClasses", "userprg");
        cJSON_AddNumberToObject(prgjson, "index", userprg->index);
        cJSON_AddNumberToObject(prgjson, "chnid", 9);
        cJSON_AddNumberToObject(prgjson, "pmtPid", userprg->pmtPid);
        sprintf(idstr, "id1.%d.%d", 9, userprg->index);//1.2.1
        cJSON_AddStringToObject(prgjson,"key", idstr);
        cJSON_AddStringToObject(prgjson,"icon", "img/notebook.ico");
        cJSON_AddItemToObject(prgjson, "children", subprgsarray = cJSON_CreateArray());
        //subprgjson
        cJSON_AddItemToArray(subprgsarray,subprgjson = cJSON_CreateObject());
        cJSON_AddStringToObject(subprgjson,"title", "maximun bitrate descriptor");
        cJSON_AddTrueToObject(subprgjson,"folder");
        cJSON_AddFalseToObject(subprgjson,"expanded");
        sprintf(idstr, "id1.%d.%d.1", 9, userprg->index);
        cJSON_AddStringToObject(subprgjson,"key", idstr);
        cJSON_AddStringToObject(subprgjson,"icon", "img/channel_in.ico");
        cJSON_AddItemToArray(subprgsarray,subprgjson = cJSON_CreateObject());
        sprintf(idstr, "id1.%d.%d.2", 9, userprg->index);
        cJSON_AddStringToObject(subprgjson,"title", "multiplex buffer utilization descriptor");
        cJSON_AddTrueToObject(subprgjson,"folder");
        cJSON_AddFalseToObject(subprgjson,"expanded");
        cJSON_AddStringToObject(subprgjson,"key", idstr);
        cJSON_AddStringToObject(subprgjson,"icon", "img/channel_in.ico");
        int k=0;
        //PMT
        Commdes_t *tmpinfo = malloc(sizeof(Commdes_t));
        int offset = 0, flag = 0;
        for(j=0; j<userprg->pmtDesListLen; j++) {
            memcpy(tmpinfo, userprg->pmtDesList+offset, sizeof(Commdes_t) );
            offset += 1;
        }
        free(tmpinfo);
        //stream
        offset = 0;
        DataStream_t *streaminfo = malloc(sizeof(DataStream_t));
        for(j=0; j<userprg->pdataStreamListLen; j++) {
            memcpy(streaminfo, userprg->pdataStreamList+offset, sizeof(DataStream_t) );
            offset += 1;
            if(2 == streaminfo->streamType){
                //MPEG2 VIDEO
                cJSON_AddItemToArray(subprgsarray,subprgjson = cJSON_CreateObject());
                sprintf(idstr, "id1.%d.%d.%d", 9, userprg->index, streaminfo->index);
                sprintf(str,"MPEG2 VIDEO  [PID(0x%x)]",  streaminfo->inPid);
                cJSON_AddStringToObject(subprgjson,"title", str);
                cJSON_AddTrueToObject(subprgjson,"folder");
                cJSON_AddFalseToObject(subprgjson,"expanded");
                cJSON_AddNumberToObject(subprgjson, "index", streaminfo->index);
                cJSON_AddStringToObject(subprgjson,"key", idstr);
                cJSON_AddStringToObject(subprgjson,"icon", "img/favicon.ico");

                cJSON_AddItemToObject(subprgjson, "children", streamsarray = cJSON_CreateArray());
                cJSON_AddItemToArray(streamsarray,streamjson = cJSON_CreateObject());
                sprintf(idstr, "id1.%d.%d.%d.1", 9,  userprg->index, streaminfo->index);
                cJSON_AddStringToObject(streamjson,"title", "video stream descriptor");
                cJSON_AddTrueToObject(streamjson,"folder");
                cJSON_AddFalseToObject(streamjson,"expanded");
                cJSON_AddStringToObject(streamjson,"key", idstr);
                cJSON_AddStringToObject(streamjson,"icon", "img/channel_in.ico");

                cJSON_AddItemToArray(streamsarray,streamjson = cJSON_CreateObject());
                sprintf(idstr, "id1.%d.%d.%d.2", 9, userprg->index, streaminfo->index);
                cJSON_AddStringToObject(streamjson,"title", "data stream alignment descriptor");
                cJSON_AddTrueToObject(streamjson,"folder");
                cJSON_AddFalseToObject(streamjson,"expanded");
                cJSON_AddStringToObject(streamjson,"key", idstr);
                cJSON_AddStringToObject(streamjson,"icon", "img/channel_in.ico");

                cJSON_AddItemToArray(streamsarray,streamjson = cJSON_CreateObject());
                sprintf(idstr, "id1.%d.%d.%d.3", 9, userprg->index, streaminfo->index);
                cJSON_AddStringToObject(streamjson,"title", "maximum bitrate descriptor");
                cJSON_AddTrueToObject(streamjson,"folder");
                cJSON_AddFalseToObject(streamjson,"expanded");
                cJSON_AddStringToObject(streamjson,"key", idstr);
                cJSON_AddStringToObject(streamjson,"icon", "img/channel_in.ico");

                cJSON_AddItemToArray(streamsarray,streamjson = cJSON_CreateObject());
                sprintf(idstr, "id1.%d.%d.%d.4", 9, userprg->index, streaminfo->index);
                cJSON_AddStringToObject(streamjson,"title", "stream identifier descriptor");
                cJSON_AddTrueToObject(streamjson,"folder");
                cJSON_AddFalseToObject(streamjson,"expanded");
                cJSON_AddStringToObject(streamjson,"key", idstr);
                cJSON_AddStringToObject(streamjson,"icon", "img/channel_in.ico");

                //cJSON_AddItemToObject(subprgjson, "children", streamsarray);
            }else if(4 == streaminfo->streamType){
                //MPEG2 AUDIO
                cJSON_AddItemToArray(subprgsarray,subprgjson = cJSON_CreateObject());
                sprintf(idstr, "id1.%d.%d.%d", 9, userprg->index, streaminfo->index);
                sprintf(str,"MPEG2 AUDIO  [PID(0x%x)]",  streaminfo->inPid);
                cJSON_AddStringToObject(subprgjson,"title", str);
                cJSON_AddTrueToObject(subprgjson,"folder");
                cJSON_AddFalseToObject(subprgjson,"expanded");
                cJSON_AddNumberToObject(subprgjson, "index", streaminfo->index);
                cJSON_AddStringToObject(subprgjson,"key", idstr);
                cJSON_AddStringToObject(subprgjson,"icon", "img/audio.ico");

                //stream json
                cJSON_AddItemToObject(subprgjson, "children", audiosarray = cJSON_CreateArray());
                cJSON_AddItemToArray(audiosarray,streamjson = cJSON_CreateObject());
                sprintf(idstr, "id1.%d.%d.%d.1", 9, userprg->index, streaminfo->index);
                cJSON_AddStringToObject(streamjson,"title", "ISO 639 language descriptor");
                cJSON_AddTrueToObject(streamjson,"folder");
                cJSON_AddFalseToObject(streamjson,"expanded");
                cJSON_AddStringToObject(streamjson,"key", idstr);
                cJSON_AddStringToObject(streamjson,"icon", "img/channel_in.ico");

                cJSON_AddItemToArray(audiosarray,streamjson = cJSON_CreateObject());
                sprintf(idstr, "id1.%d.%d.%d.2", 9, userprg->index, streaminfo->index);
                cJSON_AddStringToObject(streamjson,"title", "audio stream descriptor");
                cJSON_AddTrueToObject(streamjson,"folder");
                cJSON_AddFalseToObject(streamjson,"expanded");
                cJSON_AddStringToObject(streamjson,"key", idstr);
                cJSON_AddStringToObject(streamjson,"icon", "img/channel_in.ico");

                cJSON_AddItemToArray(audiosarray,streamjson = cJSON_CreateObject());
                sprintf(idstr, "id1.%d.%d.%d.3", 9, userprg->index, streaminfo->index);
                cJSON_AddStringToObject(streamjson,"title", "maximum bitrate descriptor");
                cJSON_AddTrueToObject(streamjson,"folder");
                cJSON_AddFalseToObject(streamjson,"expanded");
                cJSON_AddStringToObject(streamjson,"key", idstr);
                cJSON_AddStringToObject(streamjson,"icon", "img/channel_in.ico");
            }
            Commdes_t *desNodeinfo = malloc(sizeof(Commdes_t));
            for(k=0; k<streaminfo->desNodeLen; k++) {
                memcpy(desNodeinfo, streaminfo->desNode+flag, sizeof(Commdes_t) );
                flag += 1;

            }
            free(desNodeinfo);
        }
        free(streaminfo);
    }
}

void getoutprgsJson(char *ip, int inChn, char *outprg){
	int i = 0;
	char str[200] = {0};
	char idstr[100] = {0};
	Dev_prgInfo_st *ptmpPrgInfo;
	ChannelProgramSt *pst = NULL;		
	list_get(&clsProgram.outPrgList, inChn, &pst);
	cJSON *basearry,*basejson,*prgjson,*subprgjson,*subprgsarray,*streamjson,*streamsarray,*audiosarray,*prgsarray;//*prgsjson,
    char* prgjsonstring;
    basearry = cJSON_CreateArray();
	if(list_len(&pst->prgNodes)>0){
		for(i=0; i<list_len(&pst->prgNodes); i++) {
			list_get(&pst->prgNodes, i, &ptmpPrgInfo);
			cJSON_AddItemToArray(basearry,basejson = cJSON_CreateObject());
			cJSON_AddNumberToObject(basejson, "ch", ptmpPrgInfo->chnId);	
			cJSON_AddNumberToObject(basejson, "index", ptmpPrgInfo->index);
			cJSON_AddItemToObject(basejson, "children", prgjson = cJSON_CreateObject());
			//添加节目节点TITLE					
			memset(idstr, 0, sizeof(idstr));
			memcpy(idstr, ptmpPrgInfo->prgName, ptmpPrgInfo->prgNameLen);
			sprintf(str,"节目%d(0X%x):PID(0X%x) PCR_PID(0X%x) - %s",ptmpPrgInfo->prgNum, ptmpPrgInfo->prgNum, ptmpPrgInfo->pmtPid, ptmpPrgInfo->newPcrPid, idstr );
			memset(idstr, 0, sizeof(idstr));
			cJSON_AddStringToObject(prgjson,"title", str);
			cJSON_AddTrueToObject(prgjson,"folder");
			cJSON_AddFalseToObject(prgjson,"expanded");
			if(ptmpPrgInfo->userNew == 1){
			    cJSON_AddStringToObject(prgjson, "extraClasses", "userprg");
			}
			cJSON_AddNumberToObject(prgjson, "index", ptmpPrgInfo->index);
			cJSON_AddNumberToObject(prgjson, "chnid", ptmpPrgInfo->chnId);
			cJSON_AddNumberToObject(prgjson, "pmtPid", ptmpPrgInfo->pmtPid);
			sprintf(idstr, "id1.%d.%d", ptmpPrgInfo->chnId, ptmpPrgInfo->index);//1.2.1
			cJSON_AddStringToObject(prgjson,"key", idstr);
			cJSON_AddStringToObject(prgjson,"icon", "img/notebook.ico");
			
			cJSON_AddItemToObject(prgjson, "children", subprgsarray = cJSON_CreateArray());
			//subprgjson
			cJSON_AddItemToArray(subprgsarray,subprgjson = cJSON_CreateObject());
			cJSON_AddStringToObject(subprgjson,"title", "maximun bitrate descriptor");
			cJSON_AddTrueToObject(subprgjson,"folder");
			cJSON_AddFalseToObject(subprgjson,"expanded");
			sprintf(idstr, "id1.%d.%d.1", ptmpPrgInfo->chnId, ptmpPrgInfo->index);
			cJSON_AddStringToObject(subprgjson,"key", idstr);
			cJSON_AddStringToObject(subprgjson,"icon", "img/channel_in.ico");
			
			cJSON_AddItemToArray(subprgsarray,subprgjson = cJSON_CreateObject());	
			sprintf(idstr, "id1.%d.%d.2", ptmpPrgInfo->chnId, ptmpPrgInfo->index);
			cJSON_AddStringToObject(subprgjson,"title", "multiplex buffer utilization descriptor");
			cJSON_AddTrueToObject(subprgjson,"folder");
			cJSON_AddFalseToObject(subprgjson,"expanded");
			cJSON_AddStringToObject(subprgjson,"key", idstr);
			cJSON_AddStringToObject(subprgjson,"icon", "img/channel_in.ico");
			int j=0, k=0;
			//PMT
			Commdes_t *tmpinfo = malloc(sizeof(Commdes_t));
			int offset = 0, flag = 0;
			//unsigned char tmpstr[100] = {0};
			for(j=0; j<ptmpPrgInfo->pmtDesListLen; j++) {
				memcpy(tmpinfo, ptmpPrgInfo->pmtDesList+offset, sizeof(Commdes_t) );
				offset += 1;
			}
			free(tmpinfo);
			//stream
			offset = 0;
			DataStream_t *streaminfo = malloc(sizeof(DataStream_t));
			for(j=0; j<ptmpPrgInfo->pdataStreamListLen; j++) {			
				memcpy(streaminfo, ptmpPrgInfo->pdataStreamList+offset, sizeof(DataStream_t) );
				offset += 1;
				if(2 == streaminfo->streamType){
					//MPEG2 VIDEO
					cJSON_AddItemToArray(subprgsarray,subprgjson = cJSON_CreateObject());
					sprintf(idstr, "id1.%d.%d.%d", ptmpPrgInfo->chnId, ptmpPrgInfo->index, streaminfo->index);
					sprintf(str,"MPEG2 VIDEO  [PID(0x%x)]",  streaminfo->outPid);
					cJSON_AddStringToObject(subprgjson,"title", str);
					cJSON_AddTrueToObject(subprgjson,"folder");
					cJSON_AddFalseToObject(subprgjson,"expanded");
					cJSON_AddNumberToObject(subprgjson, "index", streaminfo->index);
					cJSON_AddStringToObject(subprgjson,"key", idstr);
					cJSON_AddStringToObject(subprgjson,"icon", "img/favicon.ico");
					
					cJSON_AddItemToObject(subprgjson, "children", streamsarray = cJSON_CreateArray());
					cJSON_AddItemToArray(streamsarray,streamjson = cJSON_CreateObject());
					sprintf(idstr, "id1.%d.%d.%d.1", ptmpPrgInfo->chnId,  ptmpPrgInfo->index, streaminfo->index);
					cJSON_AddStringToObject(streamjson,"title", "video stream descriptor");
					cJSON_AddTrueToObject(streamjson,"folder");
					cJSON_AddFalseToObject(streamjson,"expanded");
					cJSON_AddStringToObject(streamjson,"key", idstr);
					cJSON_AddStringToObject(streamjson,"icon", "img/channel_in.ico");
					
					cJSON_AddItemToArray(streamsarray,streamjson = cJSON_CreateObject());
					sprintf(idstr, "id1.%d.%d.%d.2", ptmpPrgInfo->chnId, ptmpPrgInfo->index, streaminfo->index);
					cJSON_AddStringToObject(streamjson,"title", "data stream alignment descriptor");
					cJSON_AddTrueToObject(streamjson,"folder");
					cJSON_AddFalseToObject(streamjson,"expanded");
					cJSON_AddStringToObject(streamjson,"key", idstr);
					cJSON_AddStringToObject(streamjson,"icon", "img/channel_in.ico");
					
					cJSON_AddItemToArray(streamsarray,streamjson = cJSON_CreateObject());
					sprintf(idstr, "id1.%d.%d.%d.3", ptmpPrgInfo->chnId, ptmpPrgInfo->index, streaminfo->index);
					cJSON_AddStringToObject(streamjson,"title", "maximum bitrate descriptor");
					cJSON_AddTrueToObject(streamjson,"folder");
					cJSON_AddFalseToObject(streamjson,"expanded");
					cJSON_AddStringToObject(streamjson,"key", idstr);
					cJSON_AddStringToObject(streamjson,"icon", "img/channel_in.ico");
					
					cJSON_AddItemToArray(streamsarray,streamjson = cJSON_CreateObject());
					sprintf(idstr, "id1.%d.%d.%d.4", ptmpPrgInfo->chnId, ptmpPrgInfo->index, streaminfo->index);
					cJSON_AddStringToObject(streamjson,"title", "stream identifier descriptor");
					cJSON_AddTrueToObject(streamjson,"folder");
					cJSON_AddFalseToObject(streamjson,"expanded");
					cJSON_AddStringToObject(streamjson,"key", idstr);
					cJSON_AddStringToObject(streamjson,"icon", "img/channel_in.ico");
				
					//cJSON_AddItemToObject(subprgjson, "children", streamsarray);
				}else if(4 == streaminfo->streamType){
					//MPEG2 AUDIO
					cJSON_AddItemToArray(subprgsarray,subprgjson = cJSON_CreateObject());
					sprintf(idstr, "id1.%d.%d.%d", ptmpPrgInfo->chnId, ptmpPrgInfo->index, streaminfo->index);
					sprintf(str,"MPEG2 AUDIO  [PID(0x%x)]",  streaminfo->outPid);
					cJSON_AddStringToObject(subprgjson,"title", str);
					cJSON_AddTrueToObject(subprgjson,"folder");
					cJSON_AddFalseToObject(subprgjson,"expanded");
					cJSON_AddNumberToObject(subprgjson, "index", streaminfo->index);
					cJSON_AddStringToObject(subprgjson,"key", idstr);
					cJSON_AddStringToObject(subprgjson,"icon", "img/audio.ico");
					
					//stream json
					cJSON_AddItemToObject(subprgjson, "children", audiosarray = cJSON_CreateArray());
					cJSON_AddItemToArray(audiosarray,streamjson = cJSON_CreateObject());
					sprintf(idstr, "id1.%d.%d.%d.1", ptmpPrgInfo->chnId, ptmpPrgInfo->index, streaminfo->index);
					cJSON_AddStringToObject(streamjson,"title", "ISO 639 language descriptor");
					cJSON_AddTrueToObject(streamjson,"folder");
					cJSON_AddFalseToObject(streamjson,"expanded");
					cJSON_AddStringToObject(streamjson,"key", idstr);
					cJSON_AddStringToObject(streamjson,"icon", "img/channel_in.ico");
					
					cJSON_AddItemToArray(audiosarray,streamjson = cJSON_CreateObject());
					sprintf(idstr, "id1.%d.%d.%d.2", ptmpPrgInfo->chnId, ptmpPrgInfo->index, streaminfo->index);
					cJSON_AddStringToObject(streamjson,"title", "audio stream descriptor");
					cJSON_AddTrueToObject(streamjson,"folder");
					cJSON_AddFalseToObject(streamjson,"expanded");
					cJSON_AddStringToObject(streamjson,"key", idstr);
					cJSON_AddStringToObject(streamjson,"icon", "img/channel_in.ico");
					
					cJSON_AddItemToArray(audiosarray,streamjson = cJSON_CreateObject());
					sprintf(idstr, "id1.%d.%d.%d.3", ptmpPrgInfo->chnId, ptmpPrgInfo->index, streaminfo->index);
					cJSON_AddStringToObject(streamjson,"title", "maximum bitrate descriptor");
					cJSON_AddTrueToObject(streamjson,"folder");
					cJSON_AddFalseToObject(streamjson,"expanded");
					cJSON_AddStringToObject(streamjson,"key", idstr);
					cJSON_AddStringToObject(streamjson,"icon", "img/channel_in.ico");				
				}				
				Commdes_t *desNodeinfo = malloc(sizeof(Commdes_t));
				for(k=0; k<streaminfo->desNodeLen; k++) {
					memcpy(desNodeinfo, streaminfo->desNode+flag, sizeof(Commdes_t) );
					flag += 1;
					
				}
				free(desNodeinfo);
			}	
			free(streaminfo);
		}
	}
	if((&pst->userPrgNodes != NULL)&&(list_len(&pst->userPrgNodes)>0)){
        adduserprgjson(basearry, pst);
	}
	list_t *caIdenList = &pst->caNode.caIdenList;
	if(list_len(caIdenList)>0){
	    int j = 0;
        cJSON_AddItemToArray(basearry,basejson = cJSON_CreateObject());
        cJSON_AddNumberToObject(basejson, "ch", inChn + 1);
        cJSON_AddStringToObject(basejson, "index", "cat");
        cJSON_AddItemToObject(basejson, "children", prgjson = cJSON_CreateObject());
        cJSON_AddStringToObject(prgjson,"title", "CAT");
        cJSON_AddTrueToObject(prgjson,"folder");
        cJSON_AddFalseToObject(prgjson,"expanded");
        cJSON_AddNumberToObject(prgjson, "chnid", inChn + 1);
        memset(idstr, 0, sizeof(idstr));
        sprintf(idstr, "id1.%d.cat", inChn + 1);//1.2.1
        cJSON_AddStringToObject(prgjson,"key", idstr);
        cJSON_AddStringToObject(prgjson,"icon", "img/star.ico");
        cJSON_AddItemToObject(prgjson, "children", subprgsarray = cJSON_CreateArray());
        CA_descriptor *cades = NULL;
        for(j=0;j<list_len(caIdenList);j++){
            list_get(caIdenList, j, &cades);
            //subprgjson
            cJSON_AddItemToArray(subprgsarray,subprgjson = cJSON_CreateObject());
            sprintf(idstr, "id1.%d.cat.%d", inChn + 1, cades->index);
            cJSON_AddStringToObject(subprgjson,"key", idstr);
            cJSON_AddNumberToObject(subprgjson,"index", cades->index);
            cJSON_AddNumberToObject(subprgjson, "chnid", cades->inChannel);
            cJSON_AddStringToObject(subprgjson,"icon", "img/circle16_green.ico");
            memset(str, 0, sizeof(str));
            sprintf(str,"CA descriptor SysID=%d(0x%x) PID(0x%x)",cades->inCaSysId, cades->inCaSysId, cades->inCaPid );
            cJSON_AddStringToObject(subprgjson,"title", str);
        }

	}
	prgjsonstring = cJSON_PrintUnformatted(basearry);
	printf("--outprgjsonlen---->>>%d\n",strlen(prgjsonstring));
	if(strlen(prgjsonstring) == 2){
	    cJSON *channeljson;
        char* prgjsonstring;
        channeljson = cJSON_CreateObject();
        cJSON_AddNumberToObject(channeljson, "sts", 1);
        prgjsonstring = cJSON_PrintUnformatted(channeljson);
        memcpy(outprg, prgjsonstring, strlen(prgjsonstring));

        //释放内存
        cJSON_Delete(channeljson);
        cJSON_Delete(basearry);
        free(prgjsonstring);
	}else{
	    memcpy(outprg, prgjsonstring, strlen(prgjsonstring));
        //printf("--outprgjsonlen---->>>%d\n",strlen(prgjsonstring));
        //释放内存
        cJSON_Delete(basearry);
        free(prgjsonstring);
	}

}



