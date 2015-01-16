

#include "communicate.h"
#include "clsMuxOutCh.h"


// 获取TS_ID
ErrorTypeEm GetOutChnTSID(char *ip, int outChn,  unsigned int  *outTsId)
{
	// 双字节，从低到高分别为：PAT\SDT\CAT\NIT	
	
	unsigned char buf[10];
    int i = 0;
    unsigned char sendbuf[256];
    int slen=0;
  
    //get call channal signal status
    enum ErrorTypeEm res;

    sendbuf[0]=0x77;
    sendbuf[1]=0x6C;
    sendbuf[2]=0x21;
    sendbuf[3]=(unsigned char)outChn;
    sendbuf[4]=0x07;
    sendbuf[5]=0x01;

    memset(buf,0,sizeof(buf));
    communicate(ip, sendbuf, 6, buf, &slen);
    
    //printf("\n####Recive GetOutChnTSID receive nums=[%d]\n", slen );
    if( 8 == slen ){
         // for(i=0;i<slen;i++)
         //   printf("Recive GetOutChnTSID buf[%d]=0x[%02x]\n",i, buf[i]);    
              
        *outTsId = ( buf[6]<<8| buf[7]) & 0xffff;  
         res = ok;

    }else 
    	res = error;

	return res;
	
}



// 获取网络ID
ErrorTypeEm GetOutChnNetID(char *ip, int outChn, unsigned int  *outNetId)
{
	unsigned char buf[10];
    int i = 0;
    unsigned char sendbuf[256];
    int slen=0;
  
    //get call channal signal status
    enum ErrorTypeEm res;

    sendbuf[0]=0x77;
    sendbuf[1]=0x6C;
    sendbuf[2]=0x21;
    sendbuf[3]=(unsigned char)outChn;
    sendbuf[4]=0x08;
    sendbuf[5]=0x01;

    memset(buf,0,sizeof(buf));
    communicate(ip, sendbuf, 6, buf, &slen);
    
    //printf("\n####Recive GetOutChnNetID receive nums=[%d]\n", slen );
    if( 8 == slen ){
          // for(i=0;i<slen;i++)
          //   printf("Recive GetOutChnNetID buf[%d]=0x[%02x]\n",i, buf[i]);    
              
        *outNetId = ( buf[6]<<8| buf[7]) & 0xffff;  
         res = ok;

    }
    else 
    	res = error;

	return res;
}


// 获取原始网络ID
ErrorTypeEm GetOutChnOrgNetID(char *ip, int outChn, unsigned int *outOrgNetId)
{

    unsigned char buf[10];
    int i = 0;
    unsigned char sendbuf[256];
    int slen=0;
  
    //get call channal signal status
    enum ErrorTypeEm res;

    sendbuf[0]=0x77;
    sendbuf[1]=0x6C;
    sendbuf[2]=0x21;
    sendbuf[3]=(unsigned char)outChn;
    sendbuf[4]=0x09;
    sendbuf[5]=0x01;

    memset(buf,0,sizeof(buf));
    communicate(ip, sendbuf, 6, buf, &slen);
    
    //printf("\n####Recive GetOutChnNetID receive nums=[%d]\n", slen );
    if( 8 == slen ){
          // for(i=0;i<slen;i++)
          //   printf("Recive GetOutChnNetID buf[%d]=0x[%02x]\n",i, buf[i]);    
              
        *outOrgNetId = ( buf[6]<<8| buf[7]) & 0xffff;  
         res = ok;

    }
    else 
        res = error;

    return res;
    
}


// 获取表版本号
ErrorTypeEm GetOutChnTableVer(char *ip, int outChn, unsigned int *outTableVer)
{
    unsigned char buf[10];
    int i = 0;
    unsigned char sendbuf[256];
    int slen=0;
  
    //get call channal signal status
    enum ErrorTypeEm res;

    sendbuf[0]=0x77;
    sendbuf[1]=0x6C;
    sendbuf[2]=0x21;
    sendbuf[3]=(unsigned char)outChn;
    sendbuf[4]=0x0a;
    sendbuf[5]=0x01;

    memset(buf,0,sizeof(buf));
    communicate(ip, sendbuf, 6, buf, &slen);
    
    //printf("\n####Recive GetOutChnNetID receive nums=[%d]\n", slen );
    if( 8 == slen ){
          // for(i=0;i<slen;i++)
          //   printf("Recive GetOutChnNetID buf[%d]=0x[%02x]\n",i, buf[i]);    
              
        *outTableVer = buf[6] & 0xff;  
         res = ok;

    }
    else 
        res = error;

    return res;
    
    
}