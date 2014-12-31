#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>


#include "list.h"
#include "communicate.h"

#include "getAllChannelSignal.h"
#include "getOutRate.h"
#include "getMultiMethod.h"
#include "getMultiProgNum2.h"
#include "getPrograms.h"
#include "freePrograms.h"
#include "cJSON.h"
#include "json.h"

int main(int argc,char *argv[])
{
    char sendbuf[256];
    int len=0,i;

    if(!argv[1]){
	    perror("please input ip\n");
	    return -1;
    }

    // int oo = 0x12345678;
    // char *p =(char *)&oo;

    // printf("=====%x==\n", oo);
    // for(i=0;i<4;i++){
    //     printf("buf[%d]=[%02x]\n",i, *p++);
    // }

    char ip[256];
    memset(ip,0,sizeof(ip));
    memcpy(ip, argv[1], strlen(argv[1]));
	program_info_t *ptmpPrgInfo;
    printf("ip=%s\n",ip);

    // unsigned char csearch;    
    // getAllChannelSignal(ip, &csearch);
    // printf("searched=[%02x]\n", csearch);
	Commdes_t *teststruct = malloc(sizeof(Commdes_t) );
	memset(teststruct, 0, sizeof(Commdes_t));
	teststruct->userNew = 0x116;
	teststruct->index = 0x117;
	teststruct->dataLen = 0x118;
    list_t  prginfolist ;    
    getPrograms(ip, 2, &prginfolist);
    //printf("call getPrograms\n");
	for(i=0; i<list_len(&prginfolist); i++) {
		list_get(&prginfolist, i, &ptmpPrgInfo);
		printf("------prginfo--->>>%x\n", ptmpPrgInfo->prgNum);
	}
	
	json_object *prgsjson = json_object_new_object();
	json_object_object_add(prgsjson, "title", json_object_new_string("输入通道"));
	json_object_object_add(prgsjson, "folder", json_object_new_boolean(1));
	json_object_object_add(prgsjson, "expanded", json_object_new_boolean(1));
	json_object_object_add(prgsjson, "key", json_object_new_string("id1.0"));
	json_object_object_add(prgsjson, "icon", json_object_new_string("img/book.ico"));
	printf("json_string1=%s\n", json_object_to_json_string(prgsjson));
	//printf("------prginfo--->>>%s\n", &prgjsonstring);
	//cJSON_Delete(prgsjson);	
	json_object_put(prgsjson);
	//free(prgjsonstring);
    freePrograms(&prginfolist);
    printf("call freePrograms\n");	
	 
	getPrograms(ip, 2, &prginfolist);
    printf("==========%x,%x,%x\n", teststruct->userNew, teststruct->index, teststruct->dataLen);
	for(i=0; i<list_len(&prginfolist); i++) {
		list_get(&prginfolist, i, &ptmpPrgInfo);
		printf("------prginfo--->>>%x\n", ptmpPrgInfo->prgNum);
	}
    freePrograms(&prginfolist);
    printf("call freePrograms\n");
	
    // int rate = 0;//4 bytes    
    // getOutRate(ip, &rate);
    // printf("output rate = [%04x]\n", rate);
    
    // unsigned char cmutexauto;   
    // getMultiMethod(ip, &cmutexauto);
    // printf("cmutexauto=[%x]\n", cmutexauto);
    
    
    printf("ip=%s\n",ip);
    // int prgnum=0;
    // getMultiProgNum(ip, &prgnum);
    

    // out_program_num_t outprg[8];
    // memset(outprg,0,sizeof(out_program_num_t)*8);
    // getMultiProgNum2(ip, outprg);
    
    
    // memset(sendbuf,0,sizeof(sendbuf));
    // getPidMapSumCnt(ip);

    // //input
    // memset(sendbuf,0,sizeof(sendbuf));
    // curCHNSearched(ip);

    // //input
    // memset(sendbuf,0,sizeof(sendbuf));
    // curCHNSearched(ip);

    // //input
    // memset(sendbuf,0,sizeof(sendbuf));
    // curCHNSearched(ip);


    return 0;
}
