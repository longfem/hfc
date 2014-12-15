#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "communicate.h"

int getPrgramInfo(char *ip, int inChn, int prgNo, unsigned char * result, int * reslen)
{

    char buf[256];
    int i = 0;
    char sendbuf[6];
    int rlen=0;


  
 
    //get call 0 auto 1 manual signal status
    sendbuf[0]=0x77;
    sendbuf[1]=0x6C;
    sendbuf[2]=0x11;
    sendbuf[3]=0x02;
    sendbuf[4]=(unsigned char)inChn;
    sendbuf[5]=(unsigned char)prgNo;

    for(i=0;i<6;i++)
    printf("%x ", sendbuf[i]);
    memset(buf,0,sizeof(buf));
    communicate(ip, sendbuf, 6, buf, &rlen);
    
    printf("\n####Recive Convert getPrgramInfo nums=[%d]\n",rlen );
   
    if( rlen > 6 ){
       
        //0 auto 1 manual              
        memcpy(result, buf, rlen); 
        *reslen =  rlen;
        return *reslen;
    }
    else if(rlen >=0 && rlen < 6){
        return *reslen;
    }

    return -1;
}

