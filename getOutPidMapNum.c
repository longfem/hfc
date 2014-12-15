#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "communicate.h"

int getOutPidMapNum(char *ip, unsigned short *result)
{

    char buf[256];
    int i = 0;
    char sendbuf[256];
    int rlen=0;
    unsigned char *p = result;
  
 
    //get call channal signal status
    sendbuf[0]=0x77;
    sendbuf[1]=0x6C;
    sendbuf[2]=0x23;
    sendbuf[3]=0x01;
    sendbuf[4]=0x02;
    sendbuf[5]=0x01;
    sendbuf[6]=0x01;

    communicate(ip, sendbuf, 7, buf, &rlen);
    
    printf("\n####Recive Convert set output multi method nums=[%d]\n",rlen );
    if(9 == rlen){
        for(i=0;i<rlen;i++)
        printf("Recive Convert set output multi method buf[%d]=0x[%02x]\n",i, buf[i]);

        *p++ = buf[8];
        *p++ = buf[9];

        printf("get out getOutPidMapNum=[%d]\n", *result );

        return rlen;
    }

    return -1;
}

