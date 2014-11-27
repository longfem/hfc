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
    if(!argv[1]){
	    perror("please input ip\n");
	    return -1;
    }

    char ip[256];
    memset(ip,0,sizeof(ip));
    memcpy(ip, argv[1], strlen(argv[1]));


    getAllChannelSignal(ip);
    getOutRate(ip);
    getMultiMethod(ip);
    getMultiProgNum(ip);
    getMultiProgNum2(ip);
    setMultiProgNum(ip);
    getPidMapSumCnt(ip);

    //input
    curCHNSearched(ip);
    return 0;
}
