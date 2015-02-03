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

#include "clsParams.h"
#include "getOutPrograms.h"


ClsProgram_st clsProgram;


int main(int argc,char *argv[])
{
    char ip[20] = {"192.168.1.134"}; 
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
    getOutPrograms(ip, 0);
	
    return 0;
}
