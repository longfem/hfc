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

    char buf[256];
    int i = 0, j=0, k=0, p=0, o=0;
    char sendbuf[256];
    int rlen=0;
    char *pbuf = buf;
    int ibuflen = 0;
    int iPmtCntIndex = 1;
    unsigned short itmp;
    program_info_t *ptmpPrgInfo = NULL;

    //getProCnt
    int prgCnt = getPrgCnt(ip, inChn);


    if (prgCnt > 0)
    {
        for (i = 0; i < prgCnt; i++){
            
            int iAddr = 0;
            rlen = getPrgramInfo(ip, 1, i, buf, &ibuflen);
            //invalid program
            if(rlen <0) { iPmtCntIndex++; continue;}

            if(rlen > 0){
                ptmpPrgInfo = malloc(sizeof(program_info_t));
                ptmpPrgInfo->index = iPmtCntIndex++;
                pbuf = buf;                
                ptmpPrgInfo->prgNum =  cdr_int16_to(*pbuf); 
                iAddr += 2; pbuf +=iAddr;
                ptmpPrgInfo->chnId = buf[iAddr];
                iAddr += 1; pbuf +=iAddr;
                ptmpPrgInfo->streamId = cdr_int16_to(*pbuf); 
                iAddr += 2; pbuf +=iAddr;
                ptmpPrgInfo->networkId = cdr_int16_to(*pbuf); 
                iAddr += 2; pbuf +=iAddr;
                ptmpPrgInfo->pmtPid = cdr_int16_to(*pbuf); 
                if (ptmpPrgInfo->pmtPid == 0xffff) continue;
                iAddr += 2; pbuf +=iAddr;                
                ptmpPrgInfo->newPcrPid = ptmpPrgInfo->oldPcrPid = cdr_int16_to(*pbuf); 
                iAddr += 2; pbuf +=iAddr;                        
                int pmtDesCnt = buf[iAddr];
                iAddr += 1; pbuf +=iAddr;

                //pmt
                ptmpPrgInfo->pmtDesList = malloc(pmtDesCnt * sizeof(Commdes_t) );
                int desCntIndex = 1;
                Commdes_t *pmtDesInfo = ptmpPrgInfo->pmtDesList;
                for (j = 0; j < pmtDesCnt; j++)
                {                    
                    pmtDesInfo->index = desCntIndex++;
                    pmtDesInfo->tag = buf[iAddr];
                    iAddr += 1; pbuf +=iAddr;
                    int pmtDesDataLen = buf[iAddr];
                    pmtDesInfo->datalen = pmtDesDataLen;
                    iAddr += 1; pbuf +=iAddr;
                    pmtDesInfo->data = malloc(pmtDesDataLen);
                    memcpy(pmtDesInfo->data, pbuf, pmtDesDataLen);                    
                    iAddr += pmtDesDataLen; pbuf +=iAddr;
                    pmtDesInfo++;
                }

                //stream data
                int dataStreamCnt = buf[iAddr];
                iAddr += 1; pbuf +=iAddr;
                ptmpPrgInfo->pdataStreamList = malloc(dataStreamCnt * sizeof(DataStream_t));
                DataStream_t *pdataStreamInfo = ptmpPrgInfo->pdataStreamList;
                for (k = 0; k < dataStreamCnt; k++)
                {                    
                    pdataStreamInfo->inChn = ptmpPrgInfo->chnId;                    
                    pdataStreamInfo->index = desCntIndex++;
                    pdataStreamInfo->streamType = buf[iAddr];                    
                    pdataStreamInfo->outPid = pdataStreamInfo->inPid = cdr_int16_to(*pbuf);
                    iAddr += 2;pbuf +=iAddr;

                    int dataStreamDesCnt = buf[iAddr];
                    pdataStreamInfo->desNode = malloc(dataStreamDesCnt);
                    int subDesCntIndex = 1;
                    Commdes_t *pdataStreamDesInfo = malloc(dataStreamDesCnt * sizeof(Commdes_t));
                    for (p = 0; p < dataStreamDesCnt; p++)
                    {                        
                        pdataStreamDesInfo->index = subDesCntIndex++;
                        pdataStreamDesInfo->tag = buf[iAddr];
                        int dataStreamDesDataLen = buf[iAddr];
                        pdataStreamDesInfo->data = malloc(dataStreamDesDataLen);                        
                        memcpy(pdataStreamDesInfo->data, pbuf, dataStreamDesDataLen);                                            
                        
                        pdataStreamDesInfo++;
                        iAddr += dataStreamDesDataLen;pbuf +=iAddr;
                    }                    
                    pdataStreamInfo++;
                }             
                // --- 节目和提供商名字 ---
                int prgNameLen = buf[iAddr];
                iAddr += 1;pbuf +=iAddr;                            
                memcpy(ptmpPrgInfo->prgName, pbuf, prgNameLen);                    
                iAddr += prgNameLen;pbuf +=iAddr;                            
                int providerNameLen = buf[iAddr];
                iAddr += 1;pbuf +=iAddr;                
                memcpy(ptmpPrgInfo->providerName, pbuf, providerNameLen);
                iAddr += providerNameLen;pbuf +=iAddr;
                ptmpPrgInfo->isCrypto = buf[iAddr] > 0 ? 1 : 0;    
                iAddr += 1;pbuf +=iAddr; 

                //SDT 描述符
                int sdtDesCnt = buf[iAddr];
                iAddr += 1;pbuf +=iAddr;
                ptmpPrgInfo->psdtDesList = malloc(sdtDesCnt * sizeof(Commdes_t));
                Commdes_t *psdtDesInfo = ptmpPrgInfo->psdtDesList;

                for (o = 0; o < sdtDesCnt; o++)
                {    
                    psdtDesInfo->index = desCntIndex++;
                    psdtDesInfo->tag = buf[iAddr];
                    iAddr += 1;pbuf +=iAddr;
                    int sdtDesDataLen = buf[iAddr];
                    iAddr += 1;pbuf +=iAddr;
                    psdtDesInfo->data = malloc(sdtDesDataLen);                    
                    memcpy(psdtDesInfo->data, pbuf, sdtDesDataLen);
                    
                    psdtDesInfo++;
                    iAddr += sdtDesDataLen;

                    if (psdtDesInfo->tag == 0x48)
                        ptmpPrgInfo->serviceType = psdtDesInfo->data[0];

                    psdtDesInfo++;
                }
            }

        }

        return 0;
    }
   
}

