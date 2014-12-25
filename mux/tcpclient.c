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

    printf("ip=%s\n",ip);

    // unsigned char csearch;    
    // getAllChannelSignal(ip, &csearch);
    // printf("searched=[%02x]\n", csearch);


    list_t  prginfolist ;    
    getPrograms(ip, 2, &prginfolist);
    printf("call getPrograms\n");
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
