#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "communicate.h"

int getPidMapSumCnt(char *ip)
{

    char buf[256];
    int i = 0;
    char sendbuf[256];
    int rlen=0;
  
 
    //get call channal signal status
    sendbuf[0]=0x77;
    sendbuf[1]=0x6C;
    sendbuf[2]=0x23;
    sendbuf[3]=0x01;
    sendbuf[4]=0x02;
    sendbuf[5]=0x01;
    sendbuf[6]=0x02;
    sendbuf[7]=0x01;
    sendbuf[8]=0x01;

    communicate(ip, sendbuf, 9, buf, &rlen);
    
    printf("\n####Recive Convert get output multi method nums=[%d]\n",rlen );
    for(i=0;i<rlen;i++)
      printf("Recive Convert get output multi method buf[%d]=0x[%02x]\n",i, buf[i]);

}

