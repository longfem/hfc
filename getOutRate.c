#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>

int getOutRate(char *ip, char* result, int* rlen)
{
    char buf[256];
    int i = 0;
    char sendbuf[256];
    int slen=0;

    //get output rate  return 4 byte len
    sendbuf[0]=0x77;
    sendbuf[1]=0x6C;
    sendbuf[2]=0x21;
    sendbuf[3]=0x01;
    sendbuf[4]=0x03;

    communicate(ip, sendbuf, 5, buf, &slen);
    
    printf("\n####Recive Convert get getOutputRate receive nums=[%d]\n",slen );
    if(slen >0){
         for(i=0;i<slen;i++)
           printf("Recive data buf[%d]=0x[%02x]\n",i, buf[i]);    

        *rlen = slen;        
        memcpy(result, buf, slen);  

        return slen;
    }
    
    //error
    *rlen = -1;
    return -1;


}

