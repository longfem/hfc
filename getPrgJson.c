#include <string.h>

#include "getPrograms.h"
#include "json.h"

void getprgsJson(char *ip, int inChn, char *outprg){
	int i=0;
	list_t  prginfolist ;    
    getPrograms(ip, 2, &prginfolist);
	json_object *prgsjson = json_object_new_object();
	json_object_object_add(prgsjson, "title", json_object_new_string("输入通道"));
	json_object_object_add(prgsjson, "folder", json_object_new_boolean(1));
	json_object_object_add(prgsjson, "expanded", json_object_new_boolean(1));
	json_object_object_add(prgsjson, "key", json_object_new_string("id1.0"));
	json_object_object_add(prgsjson, "icon", json_object_new_string("img/book.ico"));
	json_object *channelsarray = json_object_new_array();
	json_object *prgsarray = json_object_new_array();	
	json_object *channeljson = json_object_new_object();
	//json_object_object_add(channeljson, "title", json_object_new_string("通道2"));
	json_object_object_add(channeljson, "key", json_object_new_string("id1.1"));
	json_object_object_add(channeljson, "folder", json_object_new_boolean(1));
	json_object_object_add(channeljson, "expanded", json_object_new_boolean(1));
	json_object_object_add(channeljson, "icon", json_object_new_string("img/channel_in.ico"));
	program_info_t *ptmpPrgInfo = malloc(sizeof(program_info_t));
	for(i=0; i<list_len(&prginfolist); i++) {
		memset(ptmpPrgInfo, 0, sizeof(program_info_t));
		list_get(&prginfolist, i, ptmpPrgInfo);
		
	
		//添加节目节点TITLE
		char *str = (char *)malloc(200*sizeof(char));
		memset(str, 0, 200);
		prg_title(ptmpPrgInfo->prgNum, ptmpPrgInfo->pmtPid, ptmpPrgInfo->newPcrPid, ptmpPrgInfo->prgName,  str);
			
		json_object *prgjson = json_object_new_object();
		json_object_object_add(prgjson, "title", json_object_new_string(str));
		json_object_object_add(prgjson, "key", json_object_new_string("id1.3"));
		json_object_object_add(prgjson, "folder", json_object_new_boolean(1));
		json_object_object_add(prgjson, "icon", json_object_new_string("img/notebook.ico"));
		json_object_get(prgjson);
		json_object_array_add(prgsarray, prgjson);					
		json_object_put(prgjson);
					
		free(str);
	}
	
	//添加通道节点TITLE
	char *str = (char *)malloc(200*sizeof(char));
	memset(str, 0, 200);
	channel_title(ptmpPrgInfo->chnId, ptmpPrgInfo->networkId, ptmpPrgInfo->streamId, str);
	json_object_object_add(channeljson, "title", json_object_new_string(str));
	//printf("json_string1=%s\n", json_object_to_json_string(prgsarray));
	json_object_object_add(channeljson, "children", prgsarray);
	json_object_array_add(channelsarray, channeljson);	
	json_object_object_add(prgsjson, "children", channelsarray);	
	printf("json_string1=%s\n", json_object_to_json_string(prgsjson));
	memset(outprg, 0, 2048);
	memcpy(outprg, json_object_to_json_string(prgsjson), 2048);
	json_object_put(prgsarray);
	json_object_put(channeljson);
	json_object_put(channelsarray);
	json_object_put(prgsjson);
	free(str);
	free(ptmpPrgInfo);
	
	
	
	
	
}

void prg_title(int prgnum, int pid, int pcr_pid, char *prgname, char *str)
{	
	sprintf(str,"节目%d(0X%x):PID(0X%x) PCR_PID(0X%x) - %s",prgnum, prgnum, pid, pcr_pid, prgname );
}

void channel_title(int inChn, int networkId, int streamId, char *str)
{
	sprintf(str,"通道%d(ASI-%d)  -  原始网络ID=0x%04X,传输流ID=0x%04X",inChn, inChn, networkId, streamId );
}



