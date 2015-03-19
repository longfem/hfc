

//#include "EnumDef.h"
#include "communicate.h"
#include "clsMuxOutCh.h"


// 获取输出有效码率
ErrorTypeEm OutChn_validBitrateGet(char *ip, int outChn, unsigned int *outValidBitrate)
{
    unsigned char buf[20];
    int i = 0;
    unsigned char sendbuf[12];
    int slen=0;
  
    //get call channal signal status
    enum ErrorTypeEm res;

    sendbuf[0]=0x77;
    sendbuf[1]=0x6C;
    sendbuf[2]=0x21;
    sendbuf[3]=(unsigned char)outChn;
    sendbuf[4]=0x03;
    

    memset(buf,0,sizeof(buf));
    communicate(ip, sendbuf, 5, buf, &slen);
    
    //printf("\n####Recive GetOutChnTSID receive nums=[%d]\n", slen );
    if( 9 == slen ){
         // for(i=0;i<slen;i++)
         //   printf("Recive GetOutChnTSID buf[%d]=0x[%02x]\n",i, buf[i]);    
              
        *outValidBitrate =  buf[5]<<24| buf[6]<<16| buf[7] << 8 | buf[8];  
         res = ok;

    }else 
        res = error;

    return res;
    
}

ErrorTypeEm GetOutChannelStatus(char *ip, int outChn, unsigned int *outStatus)
{
    unsigned char buf[20];
    int i = 0;
    unsigned char sendbuf[256];
    int slen=0;
  
    //get call channal signal status
    enum ErrorTypeEm res;

    sendbuf[0]=0x77;
    sendbuf[1]=0x6C;
    sendbuf[2]=0x21;
    sendbuf[3]=(unsigned char)outChn;
    sendbuf[4]=0x04;
    

    memset(buf,0,sizeof(buf));
    communicate(ip, sendbuf, 5, buf, &slen);
    
    //printf("\n####Recive GetOutChnNetID receive nums=[%d]\n", slen );
    if( 6 == slen ){
          // for(i=0;i<slen;i++)
          //   printf("Recive GetOutChnNetID buf[%d]=0x[%02x]\n",i, buf[i]);    
              
        *outStatus = buf[5] & 0xff;  
        if(0 == *outStatus){
            res = ok;
        }
        else{
            printf("errrrrrrrrr \n");
            res = error;
        }

    }
    else 
        res = error;

    return res;
    
}


// 获取TS_ID
ErrorTypeEm GetOutChnTSID(char *ip, int outChn,  unsigned int  *outTsId)
{
	// 双字节，从低到高分别为：PAT\SDT\CAT\NIT	
	
	unsigned char buf[20];
    int i = 0;
    unsigned char sendbuf[12];
    int slen=0;
  
    //get call channal signal status
    enum ErrorTypeEm res;

    sendbuf[0]=0x77;
    sendbuf[1]=0x6C;
    sendbuf[2]=0x21;
    sendbuf[3]=(unsigned char)(outChn & 0xFF);
    sendbuf[4]=0x07;
    sendbuf[5]=0x01;

    memset(buf,0,sizeof(buf));
    communicate(ip, sendbuf, 6, buf, &slen);
    
    //printf("\n####Recive GetOutChnTSID receive outChn=%d nums=[%d]\n", outChn, slen );
    if( 8 == slen ){
          // for(i=0;i<slen;i++)
          //   printf("Recive GetOutChnTSID buf[%d]=0x[%02x]\n",i, buf[i]);    
              
        *outTsId = ( buf[7]<<8| buf[6]) & 0xffff;  
         res = ok;

    }else 
    	res = error;

	return res;
	
}

// 获取TS_ID
ErrorTypeEm SetOutChnTSID(char *ip, int outChn,  unsigned int  outTsId)
{
    // 双字节，从低到高分别为：PAT\SDT\CAT\NIT  
    
    unsigned char buf[20];
    int i = 0;
    unsigned char sendbuf[12];
    int slen=0;
  
    //get call channal signal status
    enum ErrorTypeEm res;

    sendbuf[0]=0x77;
    sendbuf[1]=0x6C;
    sendbuf[2]=0x21;
    sendbuf[3]=(unsigned char)outChn;
    sendbuf[4]=0x07;
    sendbuf[5]=0x02;
    sendbuf[6]=(unsigned char) (outTsId & 0xff);
    sendbuf[7]=(unsigned char) ((outTsId & 0xff00) >> 8);

    memset(buf,0,sizeof(buf));
    communicate(ip, sendbuf, 8, buf, &slen);
    
    //printf("\n####Recive GetOutChnTSID receive nums=[%d]\n", slen );
    if( 9 == slen ){
          for(i=0;i<slen;i++)
            printf("Recive SetOutChnTSID buf[%d]=0x[%02x]\n",i, buf[i]);    
        if(0 == buf[8])              
            res = ok;
        else
            res = error;

    }else 
        res = error;

    return res;
    
}



// 获取网络ID
ErrorTypeEm GetOutChnNetID(char *ip, int outChn, unsigned int  *outNetId)
{
	unsigned char buf[20];
    int i = 0;
    unsigned char sendbuf[12];
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
              
        *outNetId = ( buf[7]<<8| buf[6]) & 0xffff;  
         res = ok;

    }
    else 
    	res = error;

	return res;
}

// 获取网络ID
ErrorTypeEm SetOutChnNetID(char *ip, int outChn, unsigned int  outNetId)
{
    unsigned char buf[20];
    int i = 0;
    unsigned char sendbuf[12];
    int slen=0;
  
    //get call channal signal status
    enum ErrorTypeEm res;

    sendbuf[0]=0x77;
    sendbuf[1]=0x6C;
    sendbuf[2]=0x21;
    sendbuf[3]=(unsigned char)outChn;
    sendbuf[4]=0x08;
    sendbuf[5]=0x02;
    sendbuf[6]=(unsigned char) (outNetId & 0xff);
    sendbuf[7]=(unsigned char) ((outNetId & 0xff00) >> 8);

    memset(buf,0,sizeof(buf));
    communicate(ip, sendbuf, 8, buf, &slen);
    
    //printf("\n####Recive GetOutChnNetID receive nums=[%d]\n", slen );
    if( 9 == slen ){
           for(i=0;i<slen;i++)
             printf("Recive SetOutChnNetID buf[%d]=0x[%02x]\n",i, buf[i]);    
              
        
         res = ok;

    }
    else 
        res = error;

    return res;
}


// 获取原始网络ID
ErrorTypeEm GetOutChnOrgNetID(char *ip, int outChn, unsigned int *outOrgNetId)
{

    unsigned char buf[20];
    int i = 0;
    unsigned char sendbuf[12];
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
              
        *outOrgNetId = ( buf[7]<<8| buf[6]) & 0xffff;  
         res = ok;

    }
    else 
        res = error;

    return res;
    
}

// 获取原始网络ID
ErrorTypeEm SetOutChnOrgNetID(char *ip, int outChn, unsigned int outOrgNetId)
{

    unsigned char buf[20];
    int i = 0;
    unsigned char sendbuf[20];
    int slen=0;
  
    //get call channal signal status
    enum ErrorTypeEm res;

    sendbuf[0]=0x77;
    sendbuf[1]=0x6C;
    sendbuf[2]=0x21;
    sendbuf[3]=(unsigned char)outChn;
    sendbuf[4]=0x09;
    sendbuf[5]=0x02;
    sendbuf[6]=(unsigned char) (outOrgNetId & 0xff);
    sendbuf[7]=(unsigned char) ((outOrgNetId & 0xff00) >> 8);

    memset(buf,0,sizeof(buf));
    communicate(ip, sendbuf, 8, buf, &slen);
    
    //printf("\n####Recive GetOutChnNetID receive nums=[%d]\n", slen );
    if( 9 == slen ){
           for(i=0;i<slen;i++)
             printf("Recive GetOutChnNetID buf[%d]=0x[%02x]\n",i, buf[i]);    
                  
         res = ok;

    }
    else 
        res = error;

    return res;
    
}


// 获取表版本号
ErrorTypeEm GetOutChnTableVer(char *ip, int outChn, unsigned char *outTableVer)
{
    unsigned char buf[20];
    int i = 0;
    unsigned char sendbuf[20];
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
    if( 7 == slen ){
          // for(i=0;i<slen;i++)
          //   printf("Recive GetOutChnNetID buf[%d]=0x[%02x]\n",i, buf[i]);    
              
        *outTableVer = buf[6] & 0xff;  
         res = ok;

    }
    else 
        res = error;

    return res;
    
    
}

// 获取表版本号
ErrorTypeEm SetOutChnTableVer(char *ip, int outChn, unsigned char outTableVer)
{
    unsigned char buf[20];
    int i = 0;
    unsigned char sendbuf[20];
    int slen=0;
  
    //get call channal signal status
    enum ErrorTypeEm res;

    sendbuf[0]=0x77;
    sendbuf[1]=0x6C;
    sendbuf[2]=0x21;
    sendbuf[3]=(unsigned char)outChn;
    sendbuf[4]=0x0a;
    sendbuf[5]=0x02;
    sendbuf[6]= outTableVer ;

    memset(buf,0,sizeof(buf));
    communicate(ip, sendbuf, 7, buf, &slen);
    
    //printf("\n####Recive SetOutChnTableVer receive nums=[%d]\n", slen );
    if( 8 == slen ){
           for(i=0;i<slen;i++)
             printf("Recive SetOutChnTableVer buf[%d]=0x[%02x]\n",i, buf[i]);    
                      
         res = ok;

    }
    else 
        res = error;

    return res;
    
    
}


// 获取表使能标志位
ErrorTypeEm GetTableEnableFlag(char *ip, int outChn, unsigned int *outStatus)
{
    unsigned char buf[20];
    int i = 0;
    unsigned char sendbuf[20];
    int slen=0;
  
    //get call channal signal status
    enum ErrorTypeEm res;

    sendbuf[0]=0x77;
    sendbuf[1]=0x6C;
    sendbuf[2]=0x21;
    sendbuf[3]=(unsigned char)outChn & 0xFF;
    sendbuf[4]=0x05;
    sendbuf[5]=0x01;

    memset(buf,0,sizeof(buf));
    communicate(ip, sendbuf, 6, buf, &slen);
    
    //printf("\n####Recive GetTableEnableFlag receive nums=[%d]\n", slen );
    if( 8 == slen ){
           // for(i=0;i<slen;i++)
           //   printf("Recive GetTableEnableFlag buf[%d]=0x[%02x]\n",i, buf[i]);    
              
        *outStatus = ( buf[7]<<8| buf[6]) & 0xffff;  
                  
         res = ok;

    }
    else 
        res = error;

    return res;
        
}

// 获取表使能标志位
ErrorTypeEm SetTableEnableFlag(char *ip, int outChn, unsigned int outStatus)
{
    unsigned char buf[10];
    int i = 0;
    unsigned char sendbuf[12];
    int slen=0;
  
    //get call channal signal status
    enum ErrorTypeEm res;

    sendbuf[0]=0x77;
    sendbuf[1]=0x6C;
    sendbuf[2]=0x21;
    sendbuf[3]=(unsigned char)outChn;
    sendbuf[4]=0x05;
    sendbuf[5]=0x02;
    sendbuf[6]=(unsigned char) (outStatus & 0xff);
    sendbuf[7]=(unsigned char) ((outStatus & 0xff00) >> 8);

    memset(buf,0,sizeof(buf));
    communicate(ip, sendbuf, 8, buf, &slen);
    
    //printf("\n####Recive SetTableEnableFlag receive nums=[%d]\n", slen );
    if( 9 == slen ){
           for(i=0;i<slen;i++)
             printf("Recive SetTableEnableFlag buf[%d]=0x[%02x]\n",i, buf[i]);    
         
         res = ok;

    }
    else 
        res = error;

    return res;
        
}