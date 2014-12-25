#include <string.h>

#include "getPrograms.h"
#include "json.h"

void getprgsJson(char *ip, int inChn, char *outprg){
	int i=0;	
	char str[200];
	char idstr[20] = {0};
	list_t  prginfolist;    
	program_info_t *ptmpPrgInfo;
	json_object *prgjson;
	json_object *subprgjson;
	json_object *subprgsarray;
	json_object *streamjson;
	json_object *streamsarray;
	
    getPrograms(ip, inChn, &prginfolist);
	json_object *prgsjson = json_object_new_object();
	json_object_object_add(prgsjson, "title", json_object_new_string("输入通道"));
	json_object_object_add(prgsjson, "folder", json_object_new_boolean(1));
	json_object_object_add(prgsjson, "expanded", json_object_new_boolean(1));
	json_object_object_add(prgsjson, "key", json_object_new_string("id1.0"));
	json_object_object_add(prgsjson, "icon", json_object_new_string("img/book.ico"));
	json_object *channelsarray = json_object_new_array();
	json_object *prgsarray = json_object_new_array();	
	json_object *channeljson = json_object_new_object();
	json_object_object_add(channeljson, "key", json_object_new_string("id1.1"));
	json_object_object_add(channeljson, "folder", json_object_new_boolean(1));
	json_object_object_add(channeljson, "expanded", json_object_new_boolean(1));
	json_object_object_add(channeljson, "icon", json_object_new_string("img/channel_in.ico"));	
	for(i=0; i<list_len(&prginfolist); i++) {
		list_get(&prginfolist, i, &ptmpPrgInfo);
		//添加节目节点TITLE		
		subprgsarray = json_object_new_array();	
		streamsarray = json_object_new_array();
		sprintf(idstr, "id1.2.%d", (i+1));//1.2.1		
		sprintf(str,"节目%d(0X%x):PID(0X%x) PCR_PID(0X%x) - %s",ptmpPrgInfo->prgNum, ptmpPrgInfo->prgNum, ptmpPrgInfo->pmtPid, ptmpPrgInfo->newPcrPid, ptmpPrgInfo->prgName );
		prgjson = json_object_new_object();
		
		json_object_object_add(prgjson, "title", json_object_new_string((char*)str));
		json_object_object_add(prgjson, "key", json_object_new_string((char*)idstr));
		json_object_object_add(prgjson, "folder", json_object_new_boolean(1));
		json_object_object_add(prgjson, "icon", json_object_new_string("img/notebook.ico"));		
		//subprgjson
		subprgjson = json_object_new_object();
		sprintf(idstr, "id1.2.%d.1", (i+1));	
		json_object_object_add(subprgjson, "title", json_object_new_string("maximun bitrate descriptor"));
		json_object_object_add(subprgjson, "key", json_object_new_string((char*)idstr));
		json_object_object_add(subprgjson, "folder", json_object_new_boolean(1));
		json_object_object_add(subprgjson, "icon", json_object_new_string("img/channel_in.ico"));
		json_object_array_add(subprgsarray, subprgjson);		
		subprgjson = json_object_new_object();
		sprintf(idstr, "id1.2.%d.2", (i+1));
		json_object_object_add(subprgjson, "title", json_object_new_string("multiplex buffer utilization descriptor"));
		json_object_object_add(subprgjson, "key", json_object_new_string((char*)idstr));
		json_object_object_add(subprgjson, "folder", json_object_new_boolean(1));
		json_object_object_add(subprgjson, "icon", json_object_new_string("img/channel_in.ico"));
		json_object_array_add(subprgsarray, subprgjson);	
		
		int j=0, k=0;
		//PMT
		Commdes_t *tmpinfo = malloc(sizeof(Commdes_t));
		int offset = 0, flag = 0;
		for(j=0; j<ptmpPrgInfo->pmtDesListLen; j++) {
			memcpy(tmpinfo, ptmpPrgInfo->pmtDesList+offset, sizeof(Commdes_t) );
			//printf("-----%d-->>%s\n", j, tmpinfo->data);
			offset += sizeof(Commdes_t);
		}
		free(tmpinfo);
		//stream
		offset = 0;
		DataStream_t *streaminfo = malloc(sizeof(DataStream_t));
		for(j=0; j<ptmpPrgInfo->pdataStreamListLen; j++) {			
			memcpy(streaminfo, ptmpPrgInfo->pdataStreamList+offset, sizeof(DataStream_t) );
			printf("-----%x-->>%x\n", streaminfo->inPid, streaminfo->streamType);
			offset += sizeof(DataStream_t);				
			if(2 == streaminfo->streamType){
				//MPEG2 VIDEO
				subprgjson = json_object_new_object();
				sprintf(idstr, "id1.2.%d.3", (i+1));
				json_object_object_add(subprgjson, "icon", json_object_new_string("img/favicon.ico"));
				sprintf(str,"MPEG2 VIDEO  [PID(0x%x)]",  streaminfo->inPid);
				json_object_object_add(subprgjson, "title", json_object_new_string((char*)str));
				json_object_object_add(subprgjson, "key", json_object_new_string((char*)idstr));
				json_object_object_add(subprgjson, "folder", json_object_new_boolean(1));	

				//stream json
				streamjson = json_object_new_object();
				json_object_object_add(streamjson, "icon", json_object_new_string("img/channel_in.ico"));
				json_object_object_add(streamjson, "title", json_object_new_string("video stream descriptor"));
				sprintf(idstr, "id1.2.%d.3.1", (i+1));
				json_object_object_add(streamjson, "key", json_object_new_string((char*)idstr));
				json_object_object_add(streamjson, "folder", json_object_new_boolean(1));
				json_object_array_add(streamsarray, streamjson);
				
				streamjson = json_object_new_object();
				json_object_object_add(streamjson, "icon", json_object_new_string("img/channel_in.ico"));
				json_object_object_add(streamjson, "title", json_object_new_string("data stream alignment descriptor"));
				sprintf(idstr, "id1.2.%d.3.2", (i+1));
				json_object_object_add(streamjson, "key", json_object_new_string((char*)idstr));
				json_object_object_add(streamjson, "folder", json_object_new_boolean(1));
				json_object_array_add(streamsarray, streamjson);
				
				streamjson = json_object_new_object();
				json_object_object_add(streamjson, "icon", json_object_new_string("img/channel_in.ico"));
				json_object_object_add(streamjson, "title", json_object_new_string("maximum bitrate descriptor"));
				sprintf(idstr, "id1.2.%d.3.3", (i+1));
				json_object_object_add(streamjson, "key", json_object_new_string((char*)idstr));
				json_object_object_add(streamjson, "folder", json_object_new_boolean(1));
				json_object_array_add(streamsarray, streamjson);
				
				streamjson = json_object_new_object();
				json_object_object_add(streamjson, "icon", json_object_new_string("img/channel_in.ico"));
				json_object_object_add(streamjson, "title", json_object_new_string("stream identifier descriptor"));
				sprintf(idstr, "id1.2.%d.3.4", (i+1));
				json_object_object_add(streamjson, "key", json_object_new_string((char*)idstr));
				json_object_object_add(streamjson, "folder", json_object_new_boolean(1));
				json_object_array_add(streamsarray, streamjson);
				
				json_object_object_add(subprgjson, "children", streamsarray);
				
				json_object_array_add(subprgsarray, subprgjson);
			}else if(3 == streaminfo->streamType){
				//MPEG2 AUDIO
			}				
			
			Commdes_t *desNodeinfo = malloc(sizeof(Commdes_t));
			for(k=0; k<streaminfo->desNodeLen; k++) {
				memcpy(desNodeinfo, streaminfo->desNode+flag, sizeof(Commdes_t) );
				//printf("-----%d-->>%s\n", k, desNodeinfo->data);
				flag += sizeof(Commdes_t);
				
			}
			free(desNodeinfo);
		}	
		free(streaminfo);		
		json_object_object_add(prgjson, "children", subprgsarray);
		json_object_array_add(prgsarray, prgjson);			
	}
	
	//添加通道节点TITLE
	sprintf(str,"通道%d(ASI-%d)  -  原始网络ID=0x%04X,传输流ID=0x%04X",ptmpPrgInfo->chnId, ptmpPrgInfo->chnId, ptmpPrgInfo->networkId, ptmpPrgInfo->streamId );
	json_object_object_add(channeljson, "title", json_object_new_string((char*)str));
	//printf("json_string1=%s\n", json_object_to_json_string(prgsarray));
	json_object_object_add(channeljson, "children", prgsarray);
	json_object_array_add(channelsarray, channeljson);	
	json_object_object_add(prgsjson, "children", channelsarray);	
	//printf("json_string1=%s\n", json_object_to_json_string(prgsjson));
	printf("length=%d\n", sizeof(char)* strlen(json_object_to_json_string(prgsjson)));
	memcpy(outprg, json_object_to_json_string(prgsjson), sizeof(char)* strlen(json_object_to_json_string(prgsjson)));
	json_object_put(streamjson);
	json_object_put(streamsarray);	
	json_object_put(subprgjson);
	json_object_put(subprgsarray);
	json_object_put(prgjson);
	json_object_put(prgsarray);
	json_object_put(channeljson);
	json_object_put(channelsarray);
	json_object_put(prgsjson);
	
	

}

