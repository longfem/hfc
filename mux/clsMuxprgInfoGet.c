#include "communicate.h"
#include "clsMuxprgInfoGet.h"


ErrorTypeEm GetPidOffset(char *ip, int outChn, unsigned int *outPid)
{
	unsigned char buf[10];
    int i = 0;
    unsigned char sendbuf[256];
    int slen=0;
  
    //get call channal signal status
    enum ErrorTypeEm res;

    *outPid = 0x100;

    sendbuf[0]=0x77;
    sendbuf[1]=0x6C;
    sendbuf[2]=0x21;
    sendbuf[3]=(unsigned char)outChn;
    sendbuf[4]=0x02;
    sendbuf[5]=0x01;

    memset(buf,0,sizeof(buf));
    communicate(ip, sendbuf, 6, buf, &slen);
    
    //printf("\n####Recive GetOutChnNetID receive nums=[%d]\n", slen );
    if( 8 == slen ){
          // for(i=0;i<slen;i++)
          //   printf("Recive GetOutChnNetID buf[%d]=0x[%02x]\n",i, buf[i]);    
              
        *outPid = ( buf[6]<<8| buf[7]) & 0xffff; 
        
        if (*outPid < 0 || *outPid > 0x1fff)
			*outPid = 0x100;

         res = ok;

    }
    else 
    	res = error;

	return res;
	
}