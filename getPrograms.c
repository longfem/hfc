#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>


#include "communicate.h"
#include "getPrograms.h"

  

int getPrograms(char *ip, int inChn, out_program_num_t * outprg)
{

    unsigned char buf[256];
    int i = 0, j=0, k=0, p=0, o=0, iPrg=0;
    char sendbuf[256];
    int rlen=0;
    unsigned char *pbuf = buf;
    int ibuflen = 0;
    int iPmtCntIndex = 1;
    unsigned short itmp;
	int iAddr = 0;
    program_info_t *ptmpPrgInfo = NULL;

    //getProCnt
    int result=0;
    int prgCnt = getPrgCnt(ip, inChn, &result);

    printf("prgCnt=%d\n", prgCnt);

    if (prgCnt > 0)
    {
        for (iPrg = 0; iPrg < prgCnt; iPrg++){
            
            printf("iPrg=%d\n", iPrg);
            iAddr = 0;
            memset(buf,0,sizeof(buf));
            rlen = getPrgramInfo(ip, inChn, iPrg+1, buf, &ibuflen);
            //invalid program
            if(rlen >=0 && rlen <6) { iPmtCntIndex++; continue;}
			if(rlen < 0){return rlen;}

            if(rlen > 0){			
				
                ptmpPrgInfo = malloc(sizeof(program_info_t));
                ptmpPrgInfo->index = iPmtCntIndex++;                
				iAddr += 6; pbuf = buf+iAddr;//skip 6 header to data pointer				
                
				ptmpPrgInfo->prgNum =  cdr_int16_to(*pbuf); 
                iAddr += 2; pbuf +=iAddr;
                
				ptmpPrgInfo->chnId = buf[iAddr];
                iAddr += 1; pbuf +=iAddr;
                ptmpPrgInfo->streamId = cdr_int16_to(*pbuf); 
                iAddr += 2; pbuf +=iAddr;
                ptmpPrgInfo->networkId = cdr_int16_to(*pbuf); 
                iAddr += 2; pbuf +=iAddr;
                ptmpPrgInfo->pmtPid = cdr_int16_to(*pbuf);                
                iAddr += 2; pbuf +=iAddr;                
				if (ptmpPrgInfo->pmtPid == 0xffff) continue;
                ptmpPrgInfo->newPcrPid = ptmpPrgInfo->oldPcrPid = cdr_int16_to(*pbuf); 
                iAddr += 2; pbuf +=iAddr;                        
                int pmtDesCnt = buf[iAddr];
                iAddr += 1; pbuf +=iAddr;

                //pmt
				ptmpPrgInfo->pmtDesListLen = pmtDesCnt;
                ptmpPrgInfo->pmtDesList = malloc(pmtDesCnt * sizeof(Commdes_t) );
                int desCntIndex = 1;
                Commdes_t *pmtDesInfo = ptmpPrgInfo->pmtDesList;
                for (i = 0; i < pmtDesCnt; i++)
                {                    
                    pmtDesInfo->index = desCntIndex++;
                    pmtDesInfo->tag = buf[iAddr];
                    iAddr += 1; pbuf +=iAddr;
                    int pmtDesDataLen = buf[iAddr];
					iAddr += 1; pbuf +=iAddr;
                    pmtDesInfo->dataLen = pmtDesDataLen;                    
                    pmtDesInfo->data = malloc(pmtDesDataLen);                    
                    iAddr += pmtDesDataLen; pbuf +=iAddr;
					memcpy(pmtDesInfo->data, pbuf, pmtDesDataLen);                    
                    pmtDesInfo++;
                }

                //stream data
                int dataStreamCnt = buf[iAddr];
                iAddr += 1; pbuf +=iAddr;
				ptmpPrgInfo->pdataStreamListLen = dataStreamCnt;
                ptmpPrgInfo->pdataStreamList = malloc(dataStreamCnt * sizeof(DataStream_t));
                DataStream_t *pdataStreamInfo = ptmpPrgInfo->pdataStreamList;
                for (i = 0; i < dataStreamCnt; i++)
                {                    
                    pdataStreamInfo->inChn = ptmpPrgInfo->chnId;                    
                    pdataStreamInfo->index = desCntIndex++;
                    pdataStreamInfo->streamType = buf[iAddr]; 
					iAddr += 1;pbuf +=iAddr;					
                    pdataStreamInfo->outPid = pdataStreamInfo->inPid = cdr_int16_to(*pbuf);
                    iAddr += 2;pbuf +=iAddr;

                    int dataStreamDesCnt = buf[iAddr];
					iAddr += 1;pbuf +=iAddr;
					pdataStreamInfo->desNodeLen = dataStreamDesCnt;
                    pdataStreamInfo->desNode = malloc(dataStreamDesCnt * sizeof(Commdes_t));                    
                    Commdes_t *pdataStreamDesInfo = pdataStreamInfo->desNode;
					int subDesCntIndex = 1;
                    for (j = 0; j < dataStreamDesCnt; j++)
                    {                        
                        pdataStreamDesInfo->index = subDesCntIndex++;
                        pdataStreamDesInfo->tag = buf[iAddr];
						iAddr += 1;pbuf +=iAddr;
                        int dataStreamDesDataLen = buf[iAddr];
						iAddr += 1;pbuf +=iAddr;
						pdataStreamDesInfo->dataLen = dataStreamDesDataLen;
                        pdataStreamDesInfo->data = malloc(dataStreamDesDataLen);                        
                        memcpy(pdataStreamDesInfo->data, pbuf, dataStreamDesDataLen); 
						iAddr += dataStreamDesDataLen;pbuf +=iAddr;
						pdataStreamDesInfo++;
                    }                    
                    pdataStreamInfo++;
                }         
				
                // --- 节目和提供商名字 ---
                int prgNameLen = buf[iAddr];
                iAddr += 1;pbuf +=iAddr;   
				ptmpPrgInfo->prgNameLen = prgNameLen;				
                //stan bug bug
                memcpy(ptmpPrgInfo->prgName, pbuf, prgNameLen);                    
                
                iAddr += prgNameLen;pbuf +=iAddr;                            
                int providerNameLen = buf[iAddr];
                iAddr += 1;pbuf +=iAddr;     
				ptmpPrgInfo->providerNameLen = providerNameLen;
                //memcpy(ptmpPrgInfo->providerName, pbuf, providerNameLen);
                
                iAddr += providerNameLen;pbuf +=iAddr;
                ptmpPrgInfo->isCrypto = buf[iAddr] > 0 ? 1 : 0;    
                iAddr += 1;pbuf +=iAddr; 

                //SDT 描述符
                int sdtDesCnt = buf[iAddr];
                iAddr += 1;pbuf +=iAddr;
				ptmpPrgInfo->psdtDesListLen = sdtDesCnt;
                ptmpPrgInfo->psdtDesList = malloc(sdtDesCnt * sizeof(Commdes_t));
                Commdes_t *psdtDesInfo = ptmpPrgInfo->psdtDesList;

                for (i = 0; i < sdtDesCnt; i++)
                {    
                    psdtDesInfo->index = desCntIndex++;
                    psdtDesInfo->tag = buf[iAddr];
                    iAddr += 1;pbuf +=iAddr;
                    int sdtDesDataLen = buf[iAddr];
                    iAddr += 1;pbuf +=iAddr;
					psdtDesInfo->dataLen = sdtDesDataLen;
                    psdtDesInfo->data = malloc(sdtDesDataLen);                    
                    memcpy(psdtDesInfo->data, pbuf, sdtDesDataLen);
					iAddr += sdtDesDataLen;pbuf +=iAddr;                   
                    if (psdtDesInfo->tag == 0x48)
                        ptmpPrgInfo->serviceType = psdtDesInfo->data[0];

                    psdtDesInfo++;
                }
            }

            printf("iPrg=%d\n", iPrg);

        }

        return 0;
    }
   
}

