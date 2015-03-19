
#include <stdio.h>


#include "datastructdef.h"
#include "list.h"
#include "EnumDef.h"
#include "sendPrograms.h"

#include "clsMux.h"



extern ClsProgram_st clsProgram;

int ChnBypass_write(int outChnId)
{
// 	if (clsProgram.chnBypass2 == NULL || clsProgram.chnBypassEnable == NULL)
// 		return 0;
// 	for (unsigned char  iOutChn = 1; iOutChn <= _outMaxNum; iOutChn++)
// 	{
// 		if (outChnId == 0 || outChnId == iOutChn)
// 		{
// 			if (!clsProgram.chnBypassEnable[iOutChn - 1])
// 				continue;
// 			int iAddr = 0;
// 			char buf[256];
// 		    int i = 0;
// 		    char sendbuf[256];
// 		    int rlen=0;
		  
		 
// 		    //get call channal signal status
// 		    sendbuf[0]=0x77;
// 		    sendbuf[1]=0x6C;
// 		    sendbuf[2]=0x22;
// 		    sendbuf[3]=(unsigned char)iOutChn;
// 		    sendbuf[4]=0x0f;
// 		    sendbuf[5]=0x2;
// 		    sendbuf[6]=(unsigned char)clsProgram.chnBypass2[iOutChn - 1];
		    
		    

// 		    communicate(ip, sendbuf, 5, buf, &rlen);
		    
// 		    printf("\n####Recive curCHNSearched  nums=[%d]\n",rlen );
// 		    for(i=0;i<rlen;i++)
// 		      printf("Recive curCHNSearched method buf[%d]=0x[%02x]\n",i, buf[i]);

// /////////////////////////////////////////////////////////////////////
// 			byte[] cmdBytes = new byte[20];
// 			cmdBytes[iAddr++] = _startBytes[0];
// 			cmdBytes[iAddr++] = _startBytes[1];
// 			cmdBytes[iAddr++] = 0x22;
// 			cmdBytes[iAddr++] = (unsigned char)iOutChn;
// 			cmdBytes[iAddr++] = 0xf;
// 			cmdBytes[iAddr++] = 0x2;
// 			int CmdStringAddr = iAddr;
// 			cmdBytes[iAddr++] = (byte)clsProgram.chnBypass2[iOutChn - 1];

// 			Array.Copy(cmdBytes, _buf, iAddr);
// 			int readLen = netConn.WriteAndRead(_buf, iAddr);
// 			ErrorTypeEm checkRslt = CheckReturnBytes(cmdBytes, CmdStringAddr, _buf, readLen);
// 			if (checkRslt != ErrorTypeEm.ok || _buf[CmdStringAddr] != 0)
// 			{
// 				clsProgram.chnBypass2 = null;
// 				return 0;
// 			}
// 		}
// 	}
	return 1;
}



int sendPrograms(char *ip, int outChnId)
{
	
	if(CreateTable(outChnId))
	{		
		sendOutPutMuxInfo(ip, outChnId);
		printf("2\n");
		sendOutPutOption(ip, outChnId);

		printf("sendPrograms finish\n");
	}
	return 0;	
}