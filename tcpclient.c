#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>


#include "getAllChannelSignal.h"
#include "getOutRate.h"
#include "getMultiMethod.h"
#include "getMultiProgNum2.h"

int main(int argc,char *argv[])
{
    char sendbuf[256];
    int len=0,i;

    if(!argv[1]){
	    perror("please input ip\n");
	    return -1;
    }

    int oo = 0x12345678;
    char *p =(char *)&oo;

    printf("=====%x==\n", oo);
    for(i=0;i<4;i++){
        printf("buf[%d]=[%02x]\n",i, *p++);
    }

    char ip[256];
    memset(ip,0,sizeof(ip));
    memcpy(ip, argv[1], strlen(argv[1]));

    unsigned char csearch;    
    getAllChannelSignal(ip, &csearch);
    printf("searched=[%c]\n", csearch);

    int rate = 0;//4 bytes
    memset(sendbuf,0,sizeof(sendbuf));
    getOutRate(ip, &rate);
    printf("output rate = [%d]\n", rate);
    
    unsigned char cmutexauto;   
    getMultiMethod(ip, &cmutexauto);
    printf("cmutexauto=[%c]\n", cmutexauto);
    
    memset(sendbuf,0,sizeof(sendbuf));
    getMultiProgNum(ip);
    

    out_program_num_t outprg[8];
    memset(outprg,0,sizeof(out_program_num_t)*8);
    getMultiProgNum2(ip, outprg);
    
    
    memset(sendbuf,0,sizeof(sendbuf));
    getPidMapSumCnt(ip);

    //input
    memset(sendbuf,0,sizeof(sendbuf));
    curCHNSearched(ip);

    //input
    memset(sendbuf,0,sizeof(sendbuf));
    curCHNSearched(ip);

    //input
    memset(sendbuf,0,sizeof(sendbuf));
    curCHNSearched(ip);
    return 0;
}
