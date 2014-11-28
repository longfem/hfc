#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "getAllChannelSignal.h"
#include "getOutRate.h"
#include "getMultiMethod.h"


int main(int argc,char *argv[])
{
    char sendbuf[256];
    int len=0;

    if(!argv[1]){
	    perror("please input ip\n");
	    return -1;
    }

    char ip[256];
    memset(ip,0,sizeof(ip));
    memcpy(ip, argv[1], strlen(argv[1]));

    memset(sendbuf,0,sizeof(sendbuf));
    getAllChannelSignal(ip, sendbuf, &len);

    memset(sendbuf,0,sizeof(sendbuf));
    getOutRate(ip, sendbuf, &len);
    
    memset(sendbuf,0,sizeof(sendbuf));
    getMultiMethod(ip);
    
    memset(sendbuf,0,sizeof(sendbuf));
    getMultiProgNum(ip);
    
    memset(sendbuf,0,sizeof(sendbuf));
    getMultiProgNum2(ip);
    
    memset(sendbuf,0,sizeof(sendbuf));
    setMultiProgNum(ip);
    
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
