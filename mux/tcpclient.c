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
#include "sendPrograms.h"
#include "clsMuxOutCh.h"
#include "cJSON.h"
#include "devinfo.h"



ClsProgram_st clsProgram;



int main(int argc,char *argv[])
{
    char sendbuf[256];
    int len=0,i,ret=0;

    if(!argv[1]){
	    perror("please input ip\n");
	    return -1;
    }


    char ip[256];
    memset(ip,0,sizeof(ip));
    memcpy(ip, argv[1], strlen(argv[1]));	
    printf("ip=%s\n",ip);

    // unsigned char csearch;    
    // getAllChannelSignal(ip, &csearch);
    // printf("searched=[%02x]\n", csearch);

/////////////////////case programs///////////
    // list_t  prginfolist ;    
        
    // getPrograms(ip, 2, &prginfolist);
    // if(ret < 0){
    //     printf("getPrograms error\n");
    //     return -1;
    // }
    // printf("call getPrograms  prgNum = [%0x]\n",  ((program_info_t *)(prginfolist.head->data))->prgNum);
    // freePrograms(&prginfolist);
    // printf("call freePrograms\n");

////////////////////case end programs//////////






///////////////////////////case devinfo////////////////////////

unsigned int  outType=0;
ret = getDeviceType(ip,  &outType);
if(ret < 0){
        printf("getDeviceType error\n");
        return -1;
}
   
printf("get device type= %x\n", outType);

unsigned int  outHardVer=0;
ret = getHardVersion(ip,  &outHardVer);
if(ret < 0){
        printf("getHardVersion error\n");
        return -1;
}

printf("get device Hard version=%x\n", outHardVer);


unsigned int  outSoftVer=0;
ret = getSoftVersion(ip,  &outSoftVer);
if(ret < 0){
        printf("getSoftVersion error\n");
        return -1;
}else
   printf("get device Soft version=%x\n", outSoftVer);

unsigned int outFPGAVer = 0;
ret = getFpgaVersion(ip,  &outFPGAVer);
if(ret < 0){
        printf("getFpgaVersion error\n");
        return -1;
}else
    printf("get device Fpga version=%x\n", outFPGAVer);

unsigned int outIP=0;
ret = getIp(ip,  &outIP);
if(ret < 0){
        printf("getIp error\n");
        return -1;
}else
   printf("get device Out Ip =%4x\n", outIP);

unsigned int outSubMask=0;
ret = getSubMask(ip,  &outSubMask);
if(ret < 0){
        printf("getSubMask error\n");
        return -1;
}else
   printf("get device Out outSubMask =%4x\n", outSubMask);

unsigned int outGateway=0;
ret = getGateway(ip,  &outGateway);
if(ret < 0){
        printf("getGateway error\n");
        return -1;
}else
   printf("get device Out outGateway =%4x\n", outGateway);  
    
unsigned char outMac[6];
ret = getMac(ip, outMac, 6);
if(ret < 0){
        printf("getMac error\n");
        return -1;
}
for(i=0;i<6;i++)
    printf("outMac[%d]=[%02x]",i,outMac[i]);

unsigned int outPort = 0;
ret = getPort(ip,  &outPort);
if(ret < 0){
        printf("getPort error\n");
        return -1;
}
printf("get getPort  =%x\n", outPort);

rebootDevice(ip);

/*
unsigned int devip = ((unsigned char)192 <<32) & ((unsigned char)168 <<16) & ((unsigned char)1 <<8) & ((unsigned char)18);
ret = setIp(ip,  devip);
if(ret < 0){
        printf("setIp error\n");
        return -1;
}

ret = setMac(ip,  outMac);
if(ret < 0){
        printf("setMac error\n");
        return -1;
}


int getSubMask(char *ip,  unsigned int  *getSubMask);
int setSubMask(char *ip,  unsigned int  inSubMask);
int getGateway(char *ip,  unsigned int  *outGateway);
int setGateway(char *ip,  unsigned int  inGateway);


ret = setPort(ip,  outPort);
if(ret < 0){
        printf("setPort error\n");
        return -1;
}

*/

//restoreFactory(ip);






////////////////////////////////////////////case devinfo end /////////////////////
    //unsigned short  outTsId;
    //ErrorTypeEm res;
    //res =  GetOutChnTSID(ip, 1, &outTsId);
    //res =  GetOutChnNetID(ip, 1, &outTsId);


    //printf("outTsid = %d \n", outTsId);

    //write to device
    //ret = sendPrograms(1);
    //printf("call sendPrograms\n");
    //

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
