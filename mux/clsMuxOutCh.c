

#include "communicate.h"
#include "clsMuxOutCh.h"

ErrorTypeEm SetOutChnTSID(char *ip, int outChn, unsigned short tsId)
{

	 enum ErrorTypeEm res = ok;
	// 双字节，从低到高分别为：PAT\SDT\CAT\NIT
	// byte[] headCmd = new byte[20];
	// int iAddr = 0;
	// headCmd[iAddr++] = 0x77;
	// headCmd[iAddr++] = 0x6c;
	// headCmd[iAddr++] = 0x21;
	// headCmd[iAddr++] = (byte)outChn;
	// headCmd[iAddr++] = 0x07;
	// headCmd[iAddr++] = 0x02;

	// int CmdStringAddr = iAddr;
	// iAddr += ClsDataOper.LittleFormat_intToBytes(tsId, headCmd, iAddr, 2);

	// Array.Copy(headCmd, _buf, iAddr);
	// int readLen = netConn.WriteAndRead(_buf, iAddr);
	// ErrorTypeEm checkRslt = CheckReturnBytes(headCmd, CmdStringAddr, _buf, readLen);
	// if (checkRslt != ErrorTypeEm.ok)
	// 	return ErrorTypeEm.cmd;
	// if (_buf[iAddr] == 0)
	// 	return ErrorTypeEm.ok;
	// else
	// 	return ErrorTypeEm.error;

	return res;
}


// 获取TS_ID
ErrorTypeEm GetOutChnTSID(char *ip, int outChn,  unsigned short  *outTsId)
{
	// 双字节，从低到高分别为：PAT\SDT\CAT\NIT	
	
	char buf[10];
    int i = 0;
    char sendbuf[256];
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
              
        *outTsId = ( buf[7]<<8| buf[6]) & 0xffff;  
         res = ok;

    }else 
    	res = error;

	return res;
	
}



// 获取网络ID
ErrorTypeEm GetOutChnNetID(char *ip, int outChn, unsigned short  *outNetId)
{
	char buf[10];
    int i = 0;
    char sendbuf[256];
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