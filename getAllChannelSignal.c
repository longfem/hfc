#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>

int getAllChannelSignal(char *ip, unsigned char * result)
{


    char buf[10];
    int i = 0;
    char sendbuf[256];
    int slen=0;
  
 
    //get call channal signal status
    

    sendbuf[0]=0x77;
    sendbuf[1]=0x6C;
    sendbuf[2]=0x11;
    sendbuf[3]=0x04;
    
    memset(buf,0,sizeof(buf));
    communicate(ip, sendbuf, 4, buf, &slen);
    
    printf("\n####Recive Convert get getAllChannelSignal receive nums=[%d]\n", slen );
    if( 5 == slen ){
        // for(i=0;i<slen;i++)
        //   printf("Recive Convert get output multi method buf[%d]=0x[%02x]\n",i, buf[i]);    
              
        *result = buf[4];  
        return 0;
    }
    
    //error
    return -1;
}

