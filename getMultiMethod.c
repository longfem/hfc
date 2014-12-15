#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "communicate.h"
#include "getMultiMethod.h"

int getMultiMethod(char *ip, unsigned char * result)
{

    char buf[7];
    int i = 0;
    char sendbuf[256];
    int rlen=0;


  
 
    //get call 0 auto 1 manual signal status
    sendbuf[0]=0x77;
    sendbuf[1]=0x6C;
    sendbuf[2]=0x23;
    sendbuf[3]=0x01;
    sendbuf[4]=0x05;

    communicate(ip, sendbuf, 5, buf, &rlen);
    
    printf("\n####Recive Convert get output multi method nums=[%d]\n",rlen );
   
    if( 6 == rlen ){
       
        //0 auto 1 manual      
        *result = buf[5];  
        return *result;
    }

    return -1;
}

