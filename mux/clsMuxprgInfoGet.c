#include <stdio.h>
#include <stdlib.h>
#include "list.h"
#include "datastructdef.h"
#include "communicate.h"
#include "clsMuxprgInfoGet.h"

extern ClsParams_st *pdb;
extern ClsProgram_st clsProgram;

ErrorTypeEm GetPidOffset(char *ip, int outChn, unsigned int *outPid)
{
	unsigned char buf[10];
    int i = 0;
    unsigned char sendbuf[20];
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
              
        *outPid = ( buf[7]<<8| buf[6]) & 0xffff;       
        
        if (*outPid < 0 || *outPid > 0x1fff)
			*outPid = 0x100;

         res = ok;

    }
    else 
    	res = error;

	return res;
	
}


int GetChannelOutputMaxRate(char *ip, int outChannel, unsigned int *outRate)
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
    sendbuf[3]=(unsigned char)outChannel;
    sendbuf[4]=0x01;
    sendbuf[5]=0x01;

    memset(buf,0,sizeof(buf));
    communicate(ip, sendbuf, 6, buf, &slen);
    
    //printf("\n####Recive GetChannelOutputMaxRate receive nums=[%d]\n", slen );
    if( 9 == slen ){
          // for(i=0;i<slen;i++)
          //   printf("Recive GetChannelOutputMaxRate buf[%d]=0x[%02x]\n",i, buf[i]);    
              
        *outRate = ( buf[8]<<24| buf[7]<<8 | buf[6]) & 0xffffff;       

         res = ok;

    }
    else{
        *outRate = 0;
        res = error;
    } 
        

    return res;    
}

ErrorTypeEm GetOutProgramMuxMap(int outChannel, list_t *muxPrgInfoList) // MuxPrgInfo_st
{
   
    int i = 0;

    unsigned char buf[256];
    
    unsigned char sendbuf[256];
    int rlen=0;
    int prgCnt =0;
   
    muxPrgInfoList = malloc(sizeof(list_t));  
    //get call channal signal status
    memset(sendbuf,0,sizeof(sendbuf));
    sendbuf[0]=0x77;
    sendbuf[1]=0x6C;
    sendbuf[2]=0x23;
    sendbuf[3]=(unsigned char)(outChannel & 0xFF);
    sendbuf[4]=0x01;
    sendbuf[5]=0x01;
    sendbuf[6]=0x01;

    memset(buf, 0, sizeof(buf));
    communicate(ip, sendbuf, 7, buf, &rlen);
    
    if( 9 == slen ){
          // for(i=0;i<slen;i++)
          //   printf("Recive GetChannelOutputMaxRate buf[%d]=0x[%02x]\n",i, buf[i]);    
              
        prgCnt = ( buf[7]<<8 | buf[6]) & 0xffff;       
        res = ok;

    }
    else{        
        return error;
    } 
        
//////////////////////////////////////////////
    MuxPrgInfoGet_st *muxPrgInfo = NULL;

    int getCnt = prgCnt / 200 + ((prgCnt % 200 > 0) ? 1 : 0);

    int nowCnt = 0;
    int cmdStringAddr = 0;
    for (int i = 0; i < getCnt; i++)
    {
        memset(sendbuf,0,sizeof(sendbuf));
        sendbuf[0]=0x77;
        sendbuf[1]=0x6C;
        sendbuf[2]=0x23;
        sendbuf[3]=(unsigned char)(outChannel & 0xFF);
        sendbuf[4]=0x01;
        sendbuf[5]=0x01;
        sendbuf[6]=0x02;
        sendbuf[7]=(unsigned char) (getCnt & 0xFF);
        sendbuf[8]=(unsigned char) ((i + 1)& 0xFF);
        /////////////////////

        memset(buf, 0, sizeof(buf));
        communicate(ip, sendbuf, 9, buf, &rlen);
    

        if(rlen <= 11) {
            return error;
        }

        nowCnt = (buf[10] << 8 | buf[9]) & 0xFFFF;
        
        cmdStringAddr = 11;
        for (int j = 0; j < nowCnt; j++)
        {
            muxPrgInfo = NULL;
            muxPrgInfo = malloc(sizeof(MuxPrgInfoGet_st));
            muxPrgInfo.inChannel = buf[cmdStringAddr++];
            muxPrgInfo.prgIndex = buf[cmdStringAddr++];
            muxPrgInfo.prgNum = (buf[cmdStringAddr+1] << 8 | buf[cmdStringAddr]) & 0xFFFF;
            cmdStringAddr += 2;
            muxPrgInfo.prgPid = (buf[cmdStringAddr+1] << 8 | buf[cmdStringAddr]) & 0xFFFF;
            cmdStringAddr += 2;
            list_append(muxPrgInfoList, muxPrgInfo);
        }
    }
    return ok;
}

ErrorTypeEm GetOutPidMuxMap(int outChannel, list_t *muxPidInfoList) // MuxPidInfo_st
{
    muxPidInfoList = malloc(sizeof(list_t));

    int pidCnt  = 0, i=0, j =0;
    memset(sendbuf,0,sizeof(sendbuf));
    sendbuf[0]=0x77;
    sendbuf[1]=0x6C;
    sendbuf[2]=0x23;
    sendbuf[3]=(unsigned char)(outChannel & 0xFF);
    sendbuf[4]=0x02;
    sendbuf[5]=0x01;
    sendbuf[6]=0x01;

    memset(buf, 0, sizeof(buf));
    communicate(ip, sendbuf, 7, buf, &rlen);


     if( 9 == slen ){
          // for(i=0;i<slen;i++)
          //   printf("Recive GetChannelOutputMaxRate buf[%d]=0x[%02x]\n",i, buf[i]);    
              
        pidCnt = ( buf[7]<<8 | buf[6]) & 0xffff;       
        res = ok;

    }
    else{        
        return error;
    } 
    /////////////////////////////////////////////////
    int getCnt = pidCnt / pidMap_eachTransmit_numberMax + ((pidCnt % pidMap_eachTransmit_numberMax > 0) ? 1 : 0);
    int nowCnt = 0;
    int cmdStringAddr = 0;
    for (i = 0; i < getCnt; i++)
    {
        memset(sendbuf,0,sizeof(sendbuf));
        sendbuf[0]=0x77;
        sendbuf[1]=0x6C;
        sendbuf[2]=0x23;
        sendbuf[3]=(unsigned char)(outChannel & 0xFF);
        sendbuf[4]=0x02;
        sendbuf[5]=0x01;
        sendbuf[6]=0x02;
        sendbuf[7]=(unsigned char)(getCnt & 0xff);
        sendbuf[8]=(unsigned char)((i + 1) & 0xff);
        memset(buf, 0, sizeof(buf));
        communicate(ip, sendbuf, 9, buf, &rlen);
        if( 11 == slen ){
          // for(i=0;i<slen;i++)
          //   printf("Recive GetChannelOutputMaxRate buf[%d]=0x[%02x]\n",i, buf[i]);    
              
            nowCnt = ( buf[10]<<8 | buf[9]) & 0xffff;       
            res = ok;
        }
        else{        
            return error;
        } 
        ////////////////////
        cmdStringAddr= 11;
        for (j = 0; j < nowCnt; j++)
        {
            MuxPidInfo_st *muxPidInfo = malloc(sizeof(MuxPidInfo_st));
            muxPidInfo->inChannel = buf[cmdStringAddr++];
            muxPidInfo->oldPid = buf[cmdStringAddr+1] << 8 | buf[cmdStringAddr];
            cmdStringAddr += 2;
            muxPidInfo->newPid = buf[cmdStringAddr+1] << 8 | buf[cmdStringAddr];
            cmdStringAddr += 2;
            list_append(muxPidInfoList, muxPidInfo);            
        }
    }
    return ok;
}
ErrorTypeEm SendOutPrgMuxMap(char *ip, int outChannel, list_t *pmuxPrgInfoList)
{

/////////////////////////////send count/////////////
	unsigned char buf[256];
    int i = 0, j=0;
    unsigned char sendbuf[256];
    int slen=0;

    int dataAddr, iAddr;
  
    //get call channal signal status
    enum ErrorTypeEm res;
    
    sendbuf[0]=0x77;
    sendbuf[1]=0x6C;
    sendbuf[2]=0x23;
    sendbuf[3]=(unsigned char)outChannel;
    sendbuf[4]=0x01;
    sendbuf[5]=0x02;
    sendbuf[6]=0x01;

    unsigned int muxPrgInfoListLen = list_len(pmuxPrgInfoList);


    sendbuf[7]=(unsigned char)(muxPrgInfoListLen & 0xFF);
    sendbuf[8]=(unsigned char)(muxPrgInfoListLen & 0xFF00 >>8);

    memset(buf,0,sizeof(buf));
    communicate(ip, sendbuf, 9, buf, &slen);
    
    printf("\n####Recive GetOutChnNetID receive nums=[%d]\n", slen );
    if( slen ==10 ){
          // for(i=0;i<slen;i++)
          //   printf("Recive GetOutChnNetID buf[%d]=0x[%02x]\n",i, buf[i]);           
         res = ok;
    }
    else{
    	res = error;
    	return res;
    } 

/////////////////////////////send count end/////////////	
/////////////////////////////// -------- send muxed prg info ---------
    memset(sendbuf,0,sizeof(sendbuf));
    sendbuf[0]=0x77;
    sendbuf[1]=0x6C;
    sendbuf[2]=0x23;
    sendbuf[3]=(unsigned char)outChannel;
    sendbuf[4]=0x01;
    sendbuf[5]=0x02;
    sendbuf[6]=0x02;

    iAddr = 7;
    

	int eachSendNumMax = 200;
	int iSendTry = muxPrgInfoListLen / eachSendNumMax + (muxPrgInfoListLen % eachSendNumMax > 0 ? 1 : 0);

	int sendedCnt = 0;
	int thisNeedSend = eachSendNumMax;

	MuxPrgInfoGet_st *pcurPrg = NULL;
	for (i = 0; i < iSendTry; i++)
	{	
		dataAddr = iAddr;

		if (i + 1 == iSendTry) // the last
		{
			thisNeedSend = muxPrgInfoListLen - i * eachSendNumMax;
		}

		sendbuf[dataAddr++] = (unsigned char)iSendTry;

		
		
		sendbuf[dataAddr++] = (unsigned char)((i + 1)&0xFF);
		
		sendbuf[dataAddr++] = (unsigned char)thisNeedSend;
		for (j = 0; j < thisNeedSend; j++)
		{
			list_get(pmuxPrgInfoList, j + sendedCnt, &pcurPrg);

			if(pcurPrg){
				sendbuf[dataAddr++] = (unsigned char)pcurPrg->inChannel & 0xFF;
				sendbuf[dataAddr++] = (unsigned char)pcurPrg->prgIndex & 0xFF;	
			}
			
		}

		memset(buf,0,sizeof(buf));
    	communicate(ip, sendbuf, dataAddr, buf, &slen);

    	printf("\n####Recive SendOutPrgMuxMap j=%d receive nums=[%d]\n", j, slen );

    	if(slen < dataAddr){
    		printf("errorl..........muxPrgInfoList \n");
    		return error;
    	}
		

		// rslt = _buf[iAddr++];
		// if (rslt != 0)
		// 	return ErrorTypeEm.error;

		sendedCnt += thisNeedSend;
	}

	return ok;
}

ErrorTypeEm SendOutPrgMuxMap_finish(int outChannel)
{

    unsigned char buf[20];
    int i = 0, j=0;
    unsigned char sendbuf[20];
    int slen=0;

    int dataAddr, iAddr;
  

    memset(sendbuf, 0, sizeof(sendbuf));
    sendbuf[0]=0x77;
    sendbuf[1]=0x6C;
    sendbuf[2]=0x23;
    sendbuf[3]=(unsigned char)outChannel;
    sendbuf[4]=0x01;
    sendbuf[5]=0x03;

    memset(buf,0,sizeof(buf));
    communicate(ip, sendbuf, 6, buf, &slen);

    if( slen >=6 ){        
        res = ok;                        
    }
    else{            
        return error;          
    }

    return ok;    
}

ErrorTypeEm SendEnableMuxTable(int outChannel)
{


    unsigned char buf[20];
    int i = 0, j=0;
    unsigned char sendbuf[20];
    int slen=0;

    int dataAddr, iAddr;
  

    memset(sendbuf, 0, sizeof(sendbuf));
    sendbuf[0]=0x77;
    sendbuf[1]=0x6C;
    sendbuf[2]=0x23;
    sendbuf[3]=(unsigned char)outChannel;
    sendbuf[4]=0x04;

    memset(buf,0,sizeof(buf));
    communicate(ip, sendbuf, 5, buf, &slen);

    if( slen >=5 ){        
        res = ok;                        
    }
    else{            
        return error;          
    }

    return ok;

}

list_t * MaketPaketSection(unsigned char *table, int length)
{
    list_t *outList = malloc(sizeof(list_t));

    BufferUn_st *pPacket = malloc(sizeof(BufferUn_st));
    pPacket->pbuf = malloc(188);
    unsigned char *tmpBytes = pPacket->pbuf;
    int iAdded = 0, i=0;

    if (length <= 188)
    {
        memcpy(tmpBytes, table , length);

        for (i = length; i < 188; i++)
            tmpBytes[i] = 0xff;

        pPacket->bufLen = length;
        list_append(outList, pPacket);        
        return outList;
    }
    else
    {
        memcpy(tmpBytes, table , 188);
        pPacket->bufLen = 188;
        list_append(outList, pPacket);        
        length -= 188;
        iAdded += 188;

        while (length > 0)
        {
            pPacket = NULL;
            pPacket = malloc(sizeof(BufferUn_st));
            pPacket->pbuf = malloc(188);
            tmpBytes = pPacket->pbuf;
            
            memcpy(tmpBytes, table , 4);            
            tmpBytes[1] = (unsigned char)(tmpBytes[1] & 0xbf); // 清除起始包标志
            tmpBytes[3] = (unsigned char)((tmpBytes[3] & 0xf0) | ((tmpBytes[3]++) & 0x0f)); // 连续计数器加1

            if (length <= 184)
            {
                pPacket->bufLen = length;
                memcpy(tmpBytes+4, table+iAdded, length);                
                for (i = length; i < 184; i++)
                {
                    tmpBytes[i + 4] = 0xff;
                }
                iAdded += length;
                length = 0;

            }
            else
            {
                pPacket->bufLen = 184;
                memcpy(tmpBytes+4, table+iAdded, length);
                iAdded += 184;
                length -= 184;
            }
            list_append(outList, tmpBytes);        
        }
    }
    return outList;
}

ErrorTypeEm SendTable_pmt(char *ip, int outChannel, list_t *pmtList)
{
    unsigned char buf[256];
    int i = 0, j=0;
    unsigned char sendbuf[20];
    int slen=0;

    int dataAddr=0, iAddr=0;
  
    //get call channal signal status
    enum ErrorTypeEm res;

    if(pmtList==NULL ) return error;

    unsigned int pmtListLen = list_len(pmtList);
    if(pmtListLen <=0) return error;
    
    sendbuf[0]=0x77;
    sendbuf[1]=0x6C;
    sendbuf[2]=0x22;
    sendbuf[3]=(unsigned char)outChannel;
    sendbuf[4]=0x02; //type pmt
    sendbuf[5]=0x02;
    sendbuf[6]=0x00;

    

    memset(buf,0,sizeof(buf));
    communicate(ip, sendbuf, 7, buf, &slen);
    
    printf("\n####Recive SendTable_pmt receive nums=[%d]\n", slen );
    if( slen ==8 ){
          // for(i=0;i<slen;i++)
          //   printf("Recive GetOutChnNetID buf[%d]=0x[%02x]\n",i, buf[i]);
        if(0 != buf[7]){
            res = error;
            return res;
        }                    
    }
    else{
        res = error;
        return res;
    } 
    res = ok;


    table_pmtList_st  *pmt_tablelist = NULL;
    BufferUn_st *pbuff = NULL;
    int paketListLen = 0;
    ////////////////////////////////////////////////////////////////////////
	unsigned char iPmtCnt = 1;
	for (i = 0; i < pmtListLen; i++)
	{
        pmt_tablelist = NULL;
        list_get(pmtList, i, &pbuff); 

        if(pbuff== NULL) continue;

		list_t *paketList = NULL;
		paketList = MaketPaketSection(pbuff->pbuf, pbuff->bufLen);

		if (NULL == paketList)
			return error;

        paketListLen = list_len(pmtList);

        memset(buf,0,sizeof(buf));
        memcpy(sendbuf, 0 ,sizeof(sendbuf));
		iAddr = 0;
        sendbuf[0]=0x77;
        sendbuf[1]=0x6C;
        sendbuf[2]=0x22;
        sendbuf[3]=(unsigned char)(outChannel & 0xFF);
        sendbuf[4]=0x02; //type pmt
        sendbuf[5]=0x02;
        sendbuf[6]=(unsigned char)(paketListLen & 0xFF);

        communicate(ip, sendbuf, 7, buf, &slen);
    
        printf("\n####Recive SendTable_pmt receive nums=[%d]\n", slen );
        if( slen ==8 ){
              // for(i=0;i<slen;i++)
              //   printf("Recive GetOutChnNetID buf[%d]=0x[%02x]\n",i, buf[i]);
            if(0 != buf[7]){
                res = error;
                return res;
            }                    
        }
        else{
            res = error;
            return res;
        }
        res = ok; 

		for (j = 0; j < paketListLen; j++)
		{
			iAddr = 0;

            memset(buf,0,sizeof(buf));
            memcpy(sendbuf, 0 ,sizeof(sendbuf));
            iAddr = 0;
            sendbuf[0]=0x77;
            sendbuf[1]=0x6C;
            sendbuf[2]=0x22;
            sendbuf[3]=(unsigned char)(outChannel & 0xFF);
            sendbuf[4]=0x02; //type pmt
            sendbuf[5]=0x03;
            sendbuf[6]=(unsigned char)iPmtCnt++;
            sendbuf[7]=(unsigned char)(paketListLen & 0xFF);
            sendbuf[8]=(unsigned char)(i + 1);

            pbuff = NULL;
            list_get(paketList, j, &pbuff);
            memcpy(buf+9, pbuff->pbuf, pbuff->bufLen);

            communicate(ip, sendbuf, 9 + pbuff->bufLen, buf, &slen);
            if( slen > 9 ){
              // for(i=0;i<slen;i++)
              //   printf("Recive GetOutChnNetID buf[%d]=0x[%02x]\n",i, buf[i]);
                res = ok;                  
            }
            else{
                res = error;
                return res;
            }
            

		}
	}
	return ok;
}

ErrorTypeEm SendTable_psi(char *ip, int outChannel, PsiTableType tableType, unsigned char *ptableBytes, int length)
{
    unsigned char buf[256];
    int i = 0, j=0;
    unsigned char sendbuf[256];
    int slen=0;

    int dataAddr, iAddr;
  
    //get call channal signal status
    enum ErrorTypeEm res;
    
    if (ptableBytes == NULL || length < 0)
    {
        sendbuf[0]=0x77;
        sendbuf[1]=0x6C;
        sendbuf[2]=0x22;
        sendbuf[3]=(unsigned char)outChannel;
        sendbuf[4]=(unsigned char)tableType;
        sendbuf[5]=0x02;
        sendbuf[6]=0x00;
        sendbuf[7]=0x00;

      
        memset(buf,0,sizeof(buf));
        communicate(ip, sendbuf, 8, buf, &slen);
        
        printf("\n####Recive SendTable_psi pat receive nums=[%d]\n", slen );
        if( slen ==9 ){
              // for(i=0;i<slen;i++)
              //   printf("Recive SendTable_psi buf[%d]=0x[%02x]\n",i, buf[i]);
              
              if(buf[8]==0) 
                res = ok;
              else{
                res = error;                
              }              
        }
        else{
            res = error;            
        } 
        return ok;
    }
    /////////////////////////////////////////////////////////send first
	list_t *paketList = NULL;
	
	paketList = MaketPaketSection(ptableBytes, length);
    
    int paketListLen = list_len(paketList); 
	if (paketListLen == 0)
		return 0;


    BufferUn_st *pPacket = NULL;
	for (i = 0; i < paketListLen; i++)
	{
		iAddr = 0;

        memset(sendbuf, 0 , sizeof(sendbuf));
        sendbuf[0]=0x77;
        sendbuf[1]=0x6C;
        sendbuf[2]=0x22;
        sendbuf[3]=(unsigned char)outChannel;
        sendbuf[4]=(unsigned char)tableType;
        sendbuf[5]=0x02;

      
		if (length == 0)
		{			
            sendbuf[6]=0x00; // 没有该表
            sendbuf[7]=0x00;
		}
		else
		{
			sendbuf[6] = (unsigned char)paketListLen;
			sendbuf[7] = (unsigned char)(i + 1);
		}

        pPacket = NULL;
        list_get(paketList, i, &pPacket);

        memcpy(sendbuf + 8, (unsigned char *)pPacket->pbuf, pPacket->bufLen);

        memset(buf,0,sizeof(buf));
        communicate(ip, sendbuf, 8 + pPacket->bufLen, buf, &slen);
		
		if( slen > 8 ){
              res = ok;                        
        }
        else{            
            return error;          
        } 

	}
	return ok;
}

ErrorTypeEm SendTable_psi_finish(int outChannel)
{

    unsigned char buf[20];
    int i = 0, j=0;
    unsigned char sendbuf[20];
    int slen=0;

    int dataAddr, iAddr;
  

    memset(sendbuf, 0, sizeof(sendbuf));
    sendbuf[0]=0x77;
    sendbuf[1]=0x6C;
    sendbuf[2]=0x22;
    sendbuf[3]=(unsigned char)outChannel;
    sendbuf[4]=0x00;

    memset(buf,0,sizeof(buf));
    communicate(ip, sendbuf, 5, buf, &slen);

    if( slen ==6 ){
        printf("resul= %d\n", buf[5]);
        res = ok;                        
    }
    else{            
        return error;          
    }

    return ok;
   
}

ErrorTypeEm SendTable_PidMap(int outChannel, list_t *pidMapListArray)
{
	
    list_t *pidMapList = NULL;
    pidMapListLen = 0;
    int pidMapListArrayLen = 0
    if(pidMapListArray != NULL) {
        pidMapListArrayLen = list_len(pidMapListArray);
        list_get(&pidMapListArray, outChannel - 1, &pidMapList);
        if(pidMapList)
            pidMapListLen = list_len(pidMapList);
    }
    if (pidMapListArray ==NULL || pidMapList == NULL || pidMapListLen == 0)
    {
        // 发送个数
        memset(sendbuf, 0, sizeof(sendbuf));
        sendbuf[0]=0x77;
        sendbuf[1]=0x6C;
        sendbuf[2]=0x23;
        sendbuf[3]=(unsigned char)outChannel;
        sendbuf[4]=0x02;
        sendbuf[5]=0x02;
        sendbuf[6]=0x01;

        sendbuf[7]=0x00;
        sendbuf[8]=0x00;

        memset(buf,0,sizeof(buf));
        communicate(ip, sendbuf, 9, buf, &slen);
        if( slen >=10 ){
            printf("resul= %d\n", buf[10]);
            if(0 == buf[10])
            return  ok;                        
        }
        else{            
            return error;          
        }
        return ok;
    }
    //////////////////////////////////////////////////////////////

    int readLen;
    ErrorTypeEm checkRslt;
    int iAddr = 0;
    int i=0;


    MuxPidInfo_st * pidMapTmp = NULL;
	if (pidMapListArray ==NULL || pidMapList == NULL || pidMapListLen == 0)
	{
		
	}
	
	unsigned char *pidMapBytes = malloc(pidMapListLen * 5);
	iAddr = 0;
    for(i=0; i< pidMapListLen; i++){
        list_get(pidMapList, i, &pidMapTmp);        
       
        pidMapBytes[iAddr++] = (unsigned char)pidMapTmp->inChannel & 0xFF;
        //pidMapBytes[iAddr++] = (unsigned char)pidMapTmp->oldPid & 0x    
                
    }

	foreach (MuxPidInfo_st pidMapTmp in pidMapList)
	{
		pidMapBytes[iAddr++] = (byte)(pidMapTmp.inChannel);
		iAddr += ClsDataOper.LittleFormat_intToBytes(pidMapTmp.oldPid, pidMapBytes, iAddr, 2);
		iAddr += ClsDataOper.LittleFormat_intToBytes(pidMapTmp.newPid, pidMapBytes, iAddr, 2);
	}

	int sendCnt = pidMapList.Count / pidMap_eachTransmit_numberMax + ((pidMapList.Count % pidMap_eachTransmit_numberMax) == 0 ? 0 : 1);

	// 发送个数
	iAddr = 0;
	cmdBytes[iAddr++] = _startBytes[0];
	cmdBytes[iAddr++] = _startBytes[1];
	cmdBytes[iAddr++] = 0x23;
	cmdBytes[iAddr++] = (byte)outChannel;
	cmdBytes[iAddr++] = 2;
	cmdBytes[iAddr++] = 2;
	cmdBytes[iAddr++] = 1;
	int cmdStringAddr = iAddr;
	iAddr += ClsDataOper.LittleFormat_intToBytes(pidMapList.Count, cmdBytes, iAddr, 2);
	Array.Copy(cmdBytes, _buf, iAddr);
	readLen = netConn.WriteAndRead(_buf, iAddr);
	checkRslt = CheckReturnBytes(cmdBytes, cmdStringAddr, _buf, readLen);
	if (checkRslt != ErrorTypeEm.ok)
		return checkRslt;
	if (_buf[cmdStringAddr] != 0)
		return ErrorTypeEm.error;

	// 发送表
	int iSendedBytes = 0;
	for (int i = 0; i < sendCnt; i++)
	{
		iAddr = 0;
		cmdBytes[iAddr++] = _startBytes[0];
		cmdBytes[iAddr++] = _startBytes[1];
		cmdBytes[iAddr++] = 0x23;
		cmdBytes[iAddr++] = (byte)outChannel;
		cmdBytes[iAddr++] = 2;
		cmdBytes[iAddr++] = 2;
		cmdBytes[iAddr++] = 2;
		cmdBytes[iAddr++] = (byte)sendCnt;
		cmdBytes[iAddr++] = (byte)(i + 1);
		int cmdFinishAddr = iAddr;

		iAddr += ClsDataOper.LittleFormat_intToBytes(pidMapList.Count, cmdBytes, iAddr, 2);
		Array.Copy(cmdBytes, _buf, iAddr);
		if (pidMapBytes.Length < iSendedBytes + pidMap_eachTransmit_numberMax * 5)
		{
			Array.Copy(pidMapBytes, (i * pidMap_eachTransmit_numberMax * 5), _buf, iAddr, pidMapBytes.Length - iSendedBytes);
			iAddr += (pidMapBytes.Length - iSendedBytes);
			iSendedBytes = pidMapBytes.Length;
		}
		else
		{
			Array.Copy(pidMapBytes, (i * pidMap_eachTransmit_numberMax * 5), _buf, iAddr, pidMap_eachTransmit_numberMax);
			iSendedBytes += pidMap_eachTransmit_numberMax * 5;
			iAddr += pidMap_eachTransmit_numberMax * 5;
		}

		readLen = netConn.WriteAndRead(_buf, iAddr);
		checkRslt = CheckReturnBytes(cmdBytes, cmdFinishAddr, _buf, readLen);
		if (checkRslt != ErrorTypeEm.ok)
			return checkRslt;
		if (_buf[cmdFinishAddr] != 0)
			return ErrorTypeEm.error;
	}
	return ErrorTypeEm.ok;
}

// 		internal ErrorTypeEm DirectlyTransmit_sendMap(int outChannel, ArrayList pidMapList)
// 		{
// 			int readLen;
// 			byte[] cmdBytes = new byte[20];
// 			int iAddr = 0;
// 			if (pidMapList == null || pidMapList.Count == 0)
// 			{
// 				cmdBytes[iAddr++] = _startBytes[0];
// 				cmdBytes[iAddr++] = _startBytes[1];
// 				cmdBytes[iAddr++] = 0x23;
// 				cmdBytes[iAddr++] = (byte)outChannel;
// 				cmdBytes[iAddr++] = 3;
// 				cmdBytes[iAddr++] = 2;
// 				cmdBytes[iAddr++] = 1;
// 				int cmdStringAddr = iAddr;
// 				cmdBytes[iAddr++] = cmdBytes[iAddr++] = 0;
// 				Array.Copy(cmdBytes, _buf, iAddr);
// 				readLen = netConn.WriteAndRead(_buf, iAddr);
// 				ErrorTypeEm checkRslt = CheckReturnBytes(cmdBytes, cmdStringAddr, _buf, readLen);
// 				if (checkRslt != ErrorTypeEm.ok)
// 					return checkRslt;
// 				if (_buf[cmdStringAddr] != 0)
// 					return ErrorTypeEm.error;
// 				return ErrorTypeEm.ok;
// 			}

// 			byte[] pidMapBytes = new byte[pidMapList.Count * 5];

// 			foreach (MuxPidInfo_st pidMapTmp in pidMapList)
// 			{
// 				pidMapBytes[iAddr++] = (byte)(pidMapTmp.inChannel);
// 				iAddr += ClsDataOper.LittleFormat_intToBytes(pidMapTmp.oldPid, pidMapBytes, iAddr, 2);
// 				iAddr += ClsDataOper.LittleFormat_intToBytes(pidMapTmp.newPid, pidMapBytes, iAddr, 2);
// 			}

// 			int sendCnt = pidMapList.Count / pidMap_eachTransmit_numberMax + ((pidMapList.Count % pidMap_eachTransmit_numberMax) == 0 ? 0 : 1);

// 			// 发送个数
// 			iAddr = 0;
// 			cmdBytes[iAddr++] = _startBytes[0];
// 			cmdBytes[iAddr++] = _startBytes[1];
// 			cmdBytes[iAddr++] = 0x23;
// 			cmdBytes[iAddr++] = (byte)outChannel;
// 			cmdBytes[iAddr++] = 3;
// 			cmdBytes[iAddr++] = 2;
// 			cmdBytes[iAddr++] = 1;
// 			iAddr += ClsDataOper.LittleFormat_intToBytes(pidMapList.Count, cmdBytes, iAddr, 2);
// 			//netConn.Write(cmdBytes, iAddr, DeviceRead_callBack);
// 			//readEndSem.WaitOne();
// 			Array.Copy(cmdBytes, _buf, iAddr);
// 			readLen = netConn.WriteAndRead(_buf, iAddr);
// 			iAddr -= 2;
// 			if (readLen <= iAddr)
// 				return ErrorTypeEm.length;
// 			for (int j = 0; j < iAddr; j++)
// 			{
// 				if (cmdBytes[j] != _buf[j])
// 					return ErrorTypeEm.cmd;
// 			}
// 			if (_buf[iAddr] != 0)
// 				return ErrorTypeEm.error;

// 			// 发送表
// 			int iSendedBytes = 0;
// 			for (int i = 0; i < sendCnt; i++)
// 			{
// 				iAddr = 0;
// 				cmdBytes[iAddr++] = _startBytes[0];
// 				cmdBytes[iAddr++] = _startBytes[1];
// 				cmdBytes[iAddr++] = 0x23;
// 				cmdBytes[iAddr++] = (byte)outChannel;
// 				cmdBytes[iAddr++] = 3;
// 				cmdBytes[iAddr++] = 2;
// 				cmdBytes[iAddr++] = 2;
// 				int cmdFinishAddr = iAddr;
// 				cmdBytes[iAddr++] = (byte)sendCnt;
// 				cmdBytes[iAddr++] = (byte)(i + 1);


// 				iAddr += ClsDataOper.LittleFormat_intToBytes(pidMapList.Count, cmdBytes, iAddr, 2);
// 				Array.Copy(cmdBytes, _buf, iAddr);
// 				if (pidMapBytes.Length < iSendedBytes + pidMap_eachTransmit_numberMax * 5)
// 				{
// 					Array.Copy(pidMapBytes, (i * pidMap_eachTransmit_numberMax * 5), _buf, iAddr, pidMapBytes.Length - iSendedBytes);
// 					iAddr += (pidMapBytes.Length - iSendedBytes);
// 					iSendedBytes = pidMapBytes.Length;
// 				}
// 				else
// 				{
// 					Array.Copy(pidMapBytes, (i * pidMap_eachTransmit_numberMax * 5), _buf, iAddr, pidMap_eachTransmit_numberMax);
// 					iSendedBytes += pidMap_eachTransmit_numberMax * 5;
// 					iAddr += pidMap_eachTransmit_numberMax * 5;
// 				}

// 				//netConn.Write(_buf, iAddr, DeviceRead_callBack);
// 				//readEndSem.WaitOne();
// 				readLen = netConn.WriteAndRead(_buf, iAddr);
// 				if (readLen <= cmdFinishAddr)
// 					return ErrorTypeEm.length;
// 				for (int j = 0; j < cmdFinishAddr; j++)
// 				{
// 					if (cmdBytes[j] != _buf[j])
// 						return ErrorTypeEm.cmd;
// 				}
// 				if (_buf[cmdFinishAddr] != 0)
// 					return ErrorTypeEm.error;
// 			}
// 			ErrorTypeEm rtnRslt = ErrorTypeEm.ok;
// 			//	DirectlyTransmit_mapSendFinish(outChannel);
// 			return rtnRslt;
// 		}


unsigned char SendOutputPrgInfo(int outChn, unsigned char *muxInfoBytes, int length)
{
    // unsigned char  *cmdBytes = new byte[20];
    // int eachSendLeng = 500;
    // Array.Resize(ref muxInfoBytes, muxInfoBytes.Length + 4);
    // length += 4; // crc bytes
    // int sendCnt = length / eachSendLeng + ((length % eachSendLeng) > 0 ? 1 : 0);
    // int sendedInfoAddr = 0;
    // ClsCrc32 clsCrc = new ClsCrc32();
    // clsCrc.Key = "Hangzhou WanLong Chengdu Studio";
    // uint crcRslt = clsCrc.CrcBytes(muxInfoBytes, 0, length - 4);
    // ClsDataOper.BigFormat_intToBytes((int)crcRslt, muxInfoBytes, length - 4, 4);

    // int iAddr = 0;
    // cmdBytes[iAddr++] = _startBytes[0];
    // cmdBytes[iAddr++] = _startBytes[1];
    // cmdBytes[iAddr++] = 0x23;
    // cmdBytes[iAddr++] = (byte)outChn;
    // cmdBytes[iAddr++] = 6;
    // cmdBytes[iAddr++] = 2;
    // cmdBytes[iAddr++] = 1;
    // int CmdStringAddr = iAddr;
    // cmdBytes[iAddr++] = (byte)sendCnt;

    // ClsDataOper.LittleFormat_intToBytes(length, cmdBytes, iAddr, 2);
    // iAddr += 2;

    // Array.Copy(cmdBytes, _buf, iAddr);

    // int readLen = netConn.WriteAndRead(_buf, iAddr);
    // if (readLen <= CmdStringAddr)
    //     return false;
    // for (int j = 0; j < CmdStringAddr; j++)
    // {
    //     if (cmdBytes[j] != _buf[j])
    //         return false;
    // }
    // if (_buf[CmdStringAddr] != 0)
    //     return false;



    // for (byte i = 0; i < sendCnt; i++)
    // {
    //     iAddr = 0;
    //     cmdBytes[iAddr++] = _startBytes[0];
    //     cmdBytes[iAddr++] = _startBytes[1];
    //     cmdBytes[iAddr++] = 0x23;
    //     cmdBytes[iAddr++] = (byte)outChn;
    //     cmdBytes[iAddr++] = 6;
    //     cmdBytes[iAddr++] = 2;
    //     cmdBytes[iAddr++] = 2;
    //     CmdStringAddr = iAddr;
    //     cmdBytes[iAddr++] = (byte)sendCnt;
    //     cmdBytes[iAddr++] = (byte)(i + 1);

    //     int needSendNum = length - sendedInfoAddr;
    //     int thisTimeSendNum = (needSendNum > eachSendLeng) ? eachSendLeng : needSendNum;
    //     ClsDataOper.LittleFormat_intToBytes(thisTimeSendNum, cmdBytes, iAddr, 2);
    //     iAddr += 2;
    //     Array.Copy(cmdBytes, _buf, iAddr);
    //     //if (i + 1 == sendCnt)
    //     //{
    //     //    Array.Copy(muxInfoBytes, sendedInfoAddr, _buf, iAddr, thisTimeSendNum - 4);
    //     //    iAddr += thisTimeSendNum - 4;
    //     //    ClsDataOper.LittleFormat_intToBytes((int)crcRslt, _buf, iAddr, 4);
    //     //    iAddr += 4;
    //     //}
    //     //else
    //     {
    //         Array.Copy(muxInfoBytes, sendedInfoAddr, _buf, iAddr, thisTimeSendNum);
    //         sendedInfoAddr += thisTimeSendNum;
    //         iAddr += thisTimeSendNum;
    //     }

    //     readLen = netConn.WriteAndRead(_buf, iAddr);
    //     if (readLen <= CmdStringAddr)
    //         return false;
    //     for (int j = 0; j < CmdStringAddr; j++)
    //     {
    //         if (cmdBytes[j] != _buf[j])
    //             return false;
    //     }
    //     if (_buf[CmdStringAddr] != 0)
    //         return false;
    // }
    return 1;
}


// 获取并记录码率、表示能到数据库
void LoadBitrateAndTableEnable(char *ip, int iChn)
{
    if(NULL == pdb){
        printf("pdb not init id prggetinfo!\n");
        return ;
    }

    unsigned int outrate = 0;
    GetChannelOutputMaxRate(ip, iChn + 1, &outrate);
    
    unsigned int rate = pdb->pvalueTree->poutChnArray[iChn].outputRate = outrate;


    unsigned int tableEnableFlag = 0;
    if (GetTableEnableFlag(ip, iChn + 1, &tableEnableFlag) != ok)
    {
        printf("error\n" );
        tableEnableFlag = 0;
    }

    pdb->pvalueTree->poutChnArray[iChn].isNeedSend_pat = (tableEnableFlag & 0x1) > 0;
    pdb->pvalueTree->poutChnArray[iChn].isNeedSend_pmt = (tableEnableFlag & 0x2) > 0;
    pdb->pvalueTree->poutChnArray[iChn].isNeedSend_sdt = (tableEnableFlag & 0x4) > 0;
    pdb->pvalueTree->poutChnArray[iChn].isNeedSend_cat = (tableEnableFlag & 0x8) > 0;
    pdb->pvalueTree->poutChnArray[iChn].isNeedSend_nit = (tableEnableFlag & 0x10) > 0;
}
