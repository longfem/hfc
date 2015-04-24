
#include <stdio.h>


#include "datastructdef.h"
#include "list.h"
#include "EnumDef.h"
#include "sendPrograms.h"
#include "clsMuxprgInfoGet.h"
#include "clsMux.h"



extern ClsProgram_st clsProgram;

int ChnBypass_read(char *ip, int outChnId)
{
    int iOutChn = 0;
    if (clsProgram.chnBypass2 == NULL || clsProgram.chnBypassEnable == NULL)
        return 1;
    for (iOutChn = 1; iOutChn <= clsProgram._outChannelCntMax; iOutChn++)
    {
        if(outChnId == 0 || outChnId == iOutChn)
        {
            if (!clsProgram.chnBypassEnable[iOutChn-1])
                continue;
            int iAddr = 0;
            char buf[256];
            int i = 0;
            char sendbuf[20];
            int rlen=0;
            sendbuf[iAddr++] = 0x77;
            sendbuf[iAddr++] = 0x6C;
            sendbuf[iAddr++] = 0x22;
            sendbuf[iAddr++] = iOutChn;
            sendbuf[iAddr++] = 0xf;
            sendbuf[iAddr++] = 0x1;

            communicate(ip, sendbuf, iAddr, buf, &rlen);

            int checkRslt = CheckReturnBytes(sendbuf, iAddr, buf, rlen);
            if (checkRslt != 1)
            {
                clsProgram.chnBypass2 = NULL;
                return checkRslt;
            }
            clsProgram.chnBypass2[iOutChn - 1] = buf[iAddr];
        }
    }
    return 0;
}

int ChnBypass_write(char *ip, int outChnId)
{
    int iOutChn = 0;
 	if (clsProgram.chnBypass2 == NULL || clsProgram.chnBypassEnable == NULL)
 		return 0;
 	for (iOutChn = 1; iOutChn <= clsProgram._outChannelCntMax; iOutChn++)
 	{
 		if (outChnId == 0 || outChnId == iOutChn)
 		{
 			if (!clsProgram.chnBypassEnable[iOutChn - 1])
 				continue;
 			int iAddr = 0;
			char buf[256];
		    int i = 0;
		    char sendbuf[20];
		    int rlen=0;
		  
		 
// 		    //get call channal signal status
		    sendbuf[iAddr++]=0x77;
		    sendbuf[iAddr++]=0x6C;
		    sendbuf[iAddr++]=0x22;
		    sendbuf[iAddr++]=iOutChn;
		    sendbuf[iAddr++]=0x0f;
		    sendbuf[iAddr++]=0x2;
		    sendbuf[iAddr++]=clsProgram.chnBypass2[iOutChn - 1];

 		    communicate(ip, sendbuf, iAddr, buf, &rlen);

 			int checkRslt = CheckReturnBytes(sendbuf, iAddr, buf, rlen);
 			if (checkRslt != 0 || buf[iAddr] != 0)
 			{
 			    printf("====ChnBypass_write error-=-=");
 				clsProgram.chnBypass2 = NULL;
 				return 0;
 			}
 		}
 	}
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