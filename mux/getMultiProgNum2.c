#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>


#include "communicate.h"
#include "getMultiProgNum2.h"

int getMultiProgNum2(char *ip, out_program_num_t * outprg)
{

    char buf[256];
    int i = 0;
    char sendbuf[256];
    int rlen=0;
  
    out_program_num_t *pstr = outprg;
 
    //get call channal signal status
    sendbuf[0]=0x77;
    sendbuf[1]=0x6C;
    sendbuf[2]=0x23;
    sendbuf[3]=0x01;
    sendbuf[4]=0x01;
    sendbuf[5]=0x01;
    sendbuf[6]=0x02;
    sendbuf[7]=0x01;
    sendbuf[8]=0x01;

    communicate(ip, sendbuf, 9, buf, &rlen);
    
    printf("\n####Recive Convert get output multi method nums=[%d]\n",rlen );

    if(rlen > 12){
    //  for(i=0;i<rlen;i++)
    //  printf("Recive Convert get output multi method buf[%d]=0x[%02x]\n",i, buf[i]);
        int pronums=0;
        char *p = &pronums;

        *p++ = buf[9];
        *p++ = buf[10];
        
        p = buf+11;

        //n*{ inChn+prgIndex+newPrgId+newPid }
        if(pronums > 0){            
            for(i=0; i< pronums; i++){
                    pstr->inChn  = buf[11+6*i]; 
                    pstr->prgIndex = buf[12+6*i];
                    p = &pstr->newPrgId;
                    *p++ = buf[13+6*i];
                    *p++ = buf[14+6*i];
                    p = &pstr->newPid;
                    *p++ = buf[15+6*i];
                    *p++ = buf[16+6*i]; 
                    pstr++;                   
            }
        }

        return rlen;

    }
    
    return -1;
}

