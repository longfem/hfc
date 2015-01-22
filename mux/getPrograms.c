#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "communicate.h"
#include "list.h"
#include "getPrograms.h"

  

int getPrograms(char *ip, int inChn, list_t *prginfolist)
{
    unsigned char buf[256];
    int i = 0, j=0, k=0, p=0, o=0, iPrg=0;
    char sendbuf[256];
    int rlen=0;
    unsigned char *ptemp = NULL;
    int ibuflen = 0;
    int iPmtCntIndex = 1;
    unsigned short itmp;
	int iAddr = 0;
    Dev_prgInfo_st *ptmpPrgInfo = NULL;

    //getProCnt
    int result=0;
    int prgCnt = getPrgCnt(ip, inChn, &result);

   // printf("prgCnt=%d\n", prgCnt);

    if (prgCnt > 0)
    {
        list_init(prginfolist);

        for (iPrg = 0; iPrg < prgCnt; iPrg++){
            
            

           // printf("iPrg=%d\n", iPrg);
            iAddr = 0;
            memset(buf,0,sizeof(buf));
            rlen = getPrgramInfo(ip, inChn, iPrg+1, buf, &ibuflen);
            //invalid program
        //    printf("\n###11333#R\n");
            if(rlen >=0 && rlen <6) {
        //        printf("\n###1ipmt1#R\n");
             iPmtCntIndex++; continue;
             }
			if(rlen < 0){return rlen;}

		//	printf("\n###1befoer rlen=%d1#R\n", rlen);
            if(rlen > 0){


      
				ptmpPrgInfo = malloc(sizeof(Dev_prgInfo_st));

             //   printf("\n###11555555555#R\n");
                ptmpPrgInfo->index = iPmtCntIndex++;                
				iAddr += 4; //skip 6 header to data pointer				

                //ptemp = (unsigned char *)&ptmpPrgInfo->prgNum;
                //*ptemp++ = buf[iAddr]; *ptemp = buf[iAddr+1];
				iAddr += 2; //n
				ptmpPrgInfo->prgNum = buf[iAddr + 1]<<8 | buf[iAddr];//n
                iAddr += 2;
                
				//ptmpPrgInfo->chnId = buf[iAddr];
				ptmpPrgInfo->chnId = buf[iAddr]; //n
                iAddr += 1; 
				ptmpPrgInfo->streamId = buf[iAddr + 1]<<8 | buf[iAddr];//n
                //ptemp = (unsigned char *)&ptmpPrgInfo->streamId;
                //*ptemp++ = buf[iAddr]; *ptemp = buf[iAddr+1]; 
                iAddr += 2; 
				ptmpPrgInfo->networkId = buf[iAddr + 1]<<8 | buf[iAddr];//n
               // ptemp = (unsigned char *)&ptmpPrgInfo->networkId;
               // *ptemp++ = buf[iAddr]; *ptemp = buf[iAddr+1]; 

                iAddr += 2; 
				ptmpPrgInfo->pmtPid = buf[iAddr + 1]<<8 | buf[iAddr];//n
               // ptemp = (unsigned char *)&ptmpPrgInfo->pmtPid;
               // *ptemp++ = buf[iAddr]; *ptemp = buf[iAddr+1];

                iAddr += 2;               
				if (ptmpPrgInfo->pmtPid == 0xffff) continue;
				ptmpPrgInfo->oldPcrPid = buf[iAddr + 1]<<8 | buf[iAddr];//n
                //ptemp = (unsigned char *)&ptmpPrgInfo->oldPcrPid;
               // *ptemp++ = buf[iAddr]; *ptemp = buf[iAddr+1];
                ptmpPrgInfo->newPcrPid = ptmpPrgInfo->oldPcrPid;
                iAddr += 2;                        
                int pmtDesCnt = buf[iAddr];
                iAddr += 1; 

             //    printf("\n###6666666#R\n");
                //pmt
				ptmpPrgInfo->pmtDesListLen = pmtDesCnt;
                ptmpPrgInfo->pmtDesList = malloc(pmtDesCnt * sizeof(Commdes_t) );
                int desCntIndex = 1;
                Commdes_t *pmtDesInfo = ptmpPrgInfo->pmtDesList;
                for (i = 0; i < pmtDesCnt; i++)
                {                    
                    pmtDesInfo->index = desCntIndex++;
                    pmtDesInfo->tag = buf[iAddr];
                    iAddr += 1; 
                    int pmtDesDataLen = buf[iAddr];
					iAddr += 1; 
                    pmtDesInfo->dataLen = pmtDesDataLen;                    
                    pmtDesInfo->data = malloc(pmtDesDataLen);                    
                    iAddr += pmtDesDataLen;
					memcpy(pmtDesInfo->data, buf+iAddr, pmtDesDataLen);                    
                    pmtDesInfo++;
                }

			// printf("\n###7777777777776666#R\n");
                //stream data
                int dataStreamCnt = buf[iAddr];
                iAddr += 1;
				ptmpPrgInfo->pdataStreamListLen = dataStreamCnt;
                ptmpPrgInfo->pdataStreamList = malloc(dataStreamCnt * sizeof(DataStream_t));
                DataStream_t *pdataStreamInfo = ptmpPrgInfo->pdataStreamList;
                for (i = 0; i < dataStreamCnt; i++)
                {                    
                    pdataStreamInfo->inChn = ptmpPrgInfo->chnId;                    
                    pdataStreamInfo->index = desCntIndex++;
                    pdataStreamInfo->streamType = buf[iAddr]; 
					iAddr += 1;				
					pdataStreamInfo->inPid = buf[iAddr + 1]<<8 | buf[iAddr];//n
                    //ptemp = (unsigned char *)&pdataStreamInfo->inPid;
                    //*ptemp++ = buf[iAddr]; *ptemp = buf[iAddr+1];
                    pdataStreamInfo->outPid = pdataStreamInfo->inPid;

                    iAddr += 2;

                    int dataStreamDesCnt = buf[iAddr];
					iAddr += 1;
					pdataStreamInfo->desNodeLen = dataStreamDesCnt;
                    pdataStreamInfo->desNode = malloc(dataStreamDesCnt * sizeof(Commdes_t));                    
                    Commdes_t *pdataStreamDesInfo = pdataStreamInfo->desNode;
					int subDesCntIndex = 1;
                    for (j = 0; j < dataStreamDesCnt; j++)
                    {                        
                        pdataStreamDesInfo->index = subDesCntIndex++;
                        pdataStreamDesInfo->tag = buf[iAddr];
						iAddr += 1;
                        int dataStreamDesDataLen = buf[iAddr];
						iAddr += 1;
						pdataStreamDesInfo->dataLen = dataStreamDesDataLen;
                        pdataStreamDesInfo->data = malloc(dataStreamDesDataLen);                        
                        memcpy(pdataStreamDesInfo->data, buf+iAddr, dataStreamDesDataLen); 
						iAddr += dataStreamDesDataLen;
						pdataStreamDesInfo++;
                    }                    
                    pdataStreamInfo++;
                }         

			//	 printf("\n###88888888#R\n");
                // --- 节目和提供商名字 ---
                int prgNameLen = buf[iAddr];
                iAddr += 1;  
				ptmpPrgInfo->prgNameLen = prgNameLen;				
                //stan bug bug
                ptmpPrgInfo->prgName = (unsigned char *)malloc(prgNameLen);
				memset(ptmpPrgInfo->prgName, 0,  prgNameLen);
                memcpy(ptmpPrgInfo->prgName, (unsigned char *)(buf+iAddr),  prgNameLen); 	
				
                iAddr += prgNameLen;                           
                int providerNameLen = buf[iAddr];
                iAddr += 1;     
				ptmpPrgInfo->providerNameLen = providerNameLen;
                ptmpPrgInfo->providerName = (unsigned char *)malloc(ptmpPrgInfo->providerNameLen);
                memcpy(ptmpPrgInfo->providerName, buf+iAddr, providerNameLen);
                
                iAddr += providerNameLen;
                ptmpPrgInfo->isCrypto = buf[iAddr] > 0 ? 1 : 0;    
                iAddr += 1;

                //SDT 描述符
                int sdtDesCnt = buf[iAddr];
                iAddr += 1;
				ptmpPrgInfo->psdtDesListLen = sdtDesCnt;
                ptmpPrgInfo->psdtDesList = malloc(sdtDesCnt * sizeof(Commdes_t));
                Commdes_t *psdtDesInfo = ptmpPrgInfo->psdtDesList;

                for (i = 0; i < sdtDesCnt; i++)
                {    
                    psdtDesInfo->index = desCntIndex++;
                    psdtDesInfo->tag = buf[iAddr];
                    iAddr += 1;
                    int sdtDesDataLen = buf[iAddr];
                    iAddr += 1;
					psdtDesInfo->dataLen = sdtDesDataLen;
                    psdtDesInfo->data = malloc(sdtDesDataLen);                    
                    memcpy(psdtDesInfo->data, buf+iAddr, sdtDesDataLen);
					iAddr += sdtDesDataLen;                   
                    if (psdtDesInfo->tag == 0x48)
                        ptmpPrgInfo->serviceType = psdtDesInfo->data[0];

                    psdtDesInfo++;
                }

            //     printf("\n###999999999666#R\n");
                list_append(prginfolist, ptmpPrgInfo);

             //    printf("\n###666610101010110#R\n");
			}
        } 
		return 1;
    }

     //printf("\n###6oooookkkkkkkkkkkkkkk#R\n");
    return 0;
}

