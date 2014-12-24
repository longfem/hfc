#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "communicate.h"
#include "getOutRate.h"

int getOutRate(char *ip, int* result)
{
    char buf[4];
    int i = 0;
    char sendbuf[256];
    int slen=0;
    char *p;

    //get output rate  return 4 byte len
    sendbuf[0]=0x77;
    sendbuf[1]=0x6C;
    sendbuf[2]=0x21;
    sendbuf[3]=0x01;
    sendbuf[4]=0x03;

    communicate(ip, sendbuf, 5, buf, &slen);
    
    printf("\n####Recive Convert get getOutputRate receive nums=[%d]\n",slen );
    if(9 == slen){
         for(i=0;i<slen;i++)
           printf("Recive data buf[%d]=0x[%02x]\n",i, buf[i]);    

        p = buf+5;
        memcpy((char *)result, p, 4);
           

        return slen;
    }
    
    //error
    return -1;


}

