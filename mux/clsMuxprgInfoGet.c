#include <stdio.h>
#include <stdlib.h>


#include "datastructdef.h"
#include "communicate.h"
#include "clsPsiTable.h"

#include "clsMuxprgInfoGet.h"

extern ClsParams_st *pdb;
extern ClsProgram_st clsProgram;

ErrorTypeEm SetOutRate(char *ip, int outChannel, int outputRate)
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
    sendbuf[5]=0x02;
    sendbuf[6]=(unsigned char) outputRate & 0xff;
    sendbuf[7]=(unsigned char) (outputRate & 0xff00)>>8;
    sendbuf[8]=(unsigned char) (outputRate & 0xff0000>>16);

    memset(buf,0,sizeof(buf));
    communicate(ip, sendbuf, 9, buf, &slen);
    
    //printf("\n####Recive GetOutChnNetID receive nums=[%d]\n", slen );
    if( 10 == slen ){
          // for(i=0;i<slen;i++)
          //   printf("Recive GetOutChnNetID buf[%d]=0x[%02x]\n",i, buf[i]);    
        
        if (buf[9] == 0)
            res = ok;
        else 
            res = error;
    }
    else 
        res = error;

    return res;

}


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

ErrorTypeEm GetOutProgramMuxMap(char *ip, int outChannel, list_t *muxPrgInfoList) // MuxPrgInfo_st
{
   
    int i = 0,j =0;

    unsigned char buf[256];
    
    unsigned char sendbuf[256];
    int rlen=0;
    int prgCnt =0;

    ErrorTypeEm res;
   
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
    
    if( 9 == rlen ){
          // for(i=0;i<rlen;i++)
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
    for (i = 0; i < getCnt; i++)
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
        for (j = 0; j < nowCnt; j++)
        {
            muxPrgInfo = NULL;
            muxPrgInfo = malloc(sizeof(MuxPrgInfoGet_st));
            muxPrgInfo->inChannel = buf[cmdStringAddr++];
            muxPrgInfo->prgIndex = buf[cmdStringAddr++];
            muxPrgInfo->prgNum = (buf[cmdStringAddr+1] << 8 | buf[cmdStringAddr]) & 0xFFFF;
            cmdStringAddr += 2;
            muxPrgInfo->prgPid = (buf[cmdStringAddr+1] << 8 | buf[cmdStringAddr]) & 0xFFFF;
            cmdStringAddr += 2;
            list_append(muxPrgInfoList, muxPrgInfo);
        }
    }
    return ok;
}

ErrorTypeEm GetOutPidMuxMap(char *ip, int outChannel, list_t *muxPidInfoList) // MuxPidInfo_st
{

    unsigned char buf[256];
    
    unsigned char sendbuf[256];
    int rlen=0;
    int prgCnt =0;
    enum ErrorTypeEm res;
    

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


     if( 9 == rlen ){
          // for(i=0;i<slen;i++)
          //   printf("Recive GetChannelOutputMaxRate buf[%d]=0x[%02x]\n",i, buf[i]);    
              
        pidCnt = ( buf[7]<<8 | buf[6]) & 0xffff;       
        res = ok;

    }
    else{        
        return error;
    } 
    /////////////////////////////////////////////////
    muxPidInfoList = malloc(sizeof(list_t));

    int getCnt = pidCnt / clsProgram.pidMap_eachTransmit_numberMax + ((pidCnt % clsProgram.pidMap_eachTransmit_numberMax > 0) ? 1 : 0);
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
        if( 11 == rlen ){
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

ErrorTypeEm SendOutPrgMuxMap_finish(char *ip, int outChannel)
{

    unsigned char buf[20];
    int i = 0, j=0;
    unsigned char sendbuf[20];
    int slen=0;

    int dataAddr, iAddr;
    enum ErrorTypeEm res;
  

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

ErrorTypeEm SendEnableMuxTable(char *ip, int outChannel)
{


    unsigned char buf[20];
    int i = 0, j=0;
    unsigned char sendbuf[20];
    int slen=0;

    int dataAddr, iAddr;
    enum ErrorTypeEm res;

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

ErrorTypeEm SendTable_psi_finish(char *ip, int outChannel)
{

    unsigned char buf[20];
    int i = 0, j=0;
    unsigned char sendbuf[20];
    int slen=0;

    int dataAddr, iAddr;
    enum ErrorTypeEm res;
  

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

ErrorTypeEm SendTable_PidMap(char *ip, int outChannel, list_t *pidMapListArray)
{
	 unsigned char buf[1024];
    int i = 0, j=0;
    unsigned char sendbuf[1024];
    int slen=0;

    int dataAddr, iAddr=0;
    list_t *pidMapList = NULL;
    int pidMapListLen = 0;
    int pidMapListArrayLen = 0;
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
    ErrorTypeEm res;
    iAddr = 0;    


    MuxPidInfo_st * pidMapTmp = NULL;
	
	
	unsigned char *pidMapBytes = malloc(pidMapListLen * 5);
    int pidMapBytesLen = 0;
	iAddr = 0;
    for(i=0; i< pidMapListLen; i++){
        list_get(pidMapList, i, &pidMapTmp);        
       
        pidMapBytes[iAddr++] = (unsigned char)pidMapTmp->inChannel & 0xFF;
        pidMapBytes[iAddr++] = (unsigned char)(pidMapTmp->oldPid & 0xFF);    
        pidMapBytes[iAddr++] = (unsigned char)((pidMapTmp->oldPid & 0xFF00)>>8);    
        pidMapBytes[iAddr++] = (unsigned char)(pidMapTmp->newPid & 0xFF);    
        pidMapBytes[iAddr++] = (unsigned char)((pidMapTmp->newPid & 0xFF00)>>8);    
        pidMapBytesLen+=5;        
    }

	

	int sendCnt = pidMapListLen / clsProgram.pidMap_eachTransmit_numberMax + ((pidMapListLen % clsProgram.pidMap_eachTransmit_numberMax) == 0 ? 0 : 1);


//////////////////////////////////////
	// 发送个数
    memset(sendbuf, 0, sizeof(sendbuf));
    sendbuf[0]=0x77;
    sendbuf[1]=0x6C;
    sendbuf[2]=0x23;
    sendbuf[3]=(unsigned char)outChannel;
    sendbuf[4]=0x02;
    sendbuf[5]=0x02;
    sendbuf[6]=0x01;

    sendbuf[7]=(unsigned char) (pidMapListLen & 0xFF);
    sendbuf[8]=(unsigned char) (pidMapListLen & 0xFF00)>>8;

    memset(buf,0,sizeof(buf));
    communicate(ip, sendbuf, 9, buf, &slen);
    if( slen < 9 ){
        return error;
    }
	

	// 发送表
	int iSendedBytes = 0;
	for (i = 0; i < sendCnt; i++)
	{
        memset(sendbuf, 0, sizeof(sendbuf));
        sendbuf[0]=0x77;
        sendbuf[1]=0x6C;
        sendbuf[2]=0x23;
        sendbuf[3]=(unsigned char)outChannel;
        sendbuf[4]=0x02;
        sendbuf[5]=0x02;
        sendbuf[6]=0x02;
        sendbuf[7]=(unsigned char)sendCnt & 0xFF;
        sendbuf[8]=(unsigned char)(i + 1);
        sendbuf[9]=(unsigned char) (pidMapListLen & 0xFF);
        sendbuf[10]=(unsigned char) (pidMapListLen & 0xFF00)>>8;
		
		int cmdFinishAddr = 9;

		iAddr = 11;
		if (pidMapBytesLen < iSendedBytes + clsProgram.pidMap_eachTransmit_numberMax * 5)
		{
            memcpy(sendbuf + iAddr, pidMapBytes + (i * clsProgram.pidMap_eachTransmit_numberMax * 5), pidMapBytesLen - iSendedBytes);			
			iAddr += (pidMapBytesLen - iSendedBytes);
			iSendedBytes = pidMapBytesLen;
		}
		else
		{
            memcpy(sendbuf + iAddr, pidMapBytes + (i * clsProgram.pidMap_eachTransmit_numberMax * 5), clsProgram.pidMap_eachTransmit_numberMax);			
			iSendedBytes += clsProgram.pidMap_eachTransmit_numberMax * 5;
			iAddr += clsProgram.pidMap_eachTransmit_numberMax * 5;
		}

        memset(buf,0,sizeof(buf));
        communicate(ip, sendbuf, iAddr, buf, &slen);

        if(slen == iAddr+1){
            if(buf[iAddr] == 0)
                res = ok;
            else{
                printf("send pidmap error\n");
                return error;
            }
        }		
	}
	return ok;
}

ErrorTypeEm DirectlyTransmit_sendMap(char *ip, int outChannel, list_t *pidMapList)
{

    unsigned char buf[1024];
    int i = 0, j=0;
    unsigned char sendbuf[20];
    int slen=0;

    int dataAddr, iAddr=0;
    ErrorTypeEm res;

    int pidMapListLen = list_len(pidMapList);

	int readLen;	
	if (pidMapList == NULL || pidMapListLen == 0)
	{
        memset(sendbuf, 0, sizeof(sendbuf));
        sendbuf[0]=0x77;
        sendbuf[1]=0x6C;
        sendbuf[2]=0x23;
        sendbuf[3]=(unsigned char)outChannel;
        sendbuf[4]=0x03;
        sendbuf[5]=0x02;
        sendbuf[6]=0x01;

        sendbuf[7]=0x00;
        sendbuf[8]=0x00;

        memset(buf,0,sizeof(buf));
        communicate(ip, sendbuf, 9, buf, &slen);
        if( slen < 9 ){
            return error;
        }

        return ok;
	}

	unsigned char *pidMapBytes = malloc(pidMapListLen * 5);

    int pidMapBytesLen = 0;
    iAddr = 0;
    MuxPidInfo_st * pidMapTmp = NULL;

    for(i=0; i< pidMapListLen; i++){
        list_get(pidMapList, i, &pidMapTmp);        
       
        pidMapBytes[iAddr++] = (unsigned char)pidMapTmp->inChannel & 0xFF;
        pidMapBytes[iAddr++] = (unsigned char)(pidMapTmp->oldPid & 0xFF);    
        pidMapBytes[iAddr++] = (unsigned char)((pidMapTmp->oldPid & 0xFF00)>>8);    
        pidMapBytes[iAddr++] = (unsigned char)(pidMapTmp->newPid & 0xFF);    
        pidMapBytes[iAddr++] = (unsigned char)((pidMapTmp->newPid & 0xFF00)>>8);    
        pidMapBytesLen+=5;        
    }

    int sendCnt = pidMapListLen / clsProgram.pidMap_eachTransmit_numberMax + ((pidMapListLen % clsProgram.pidMap_eachTransmit_numberMax) == 0 ? 0 : 1);


    // 发送个数
    memset(sendbuf, 0, sizeof(sendbuf));
    sendbuf[0]=0x77;
    sendbuf[1]=0x6C;
    sendbuf[2]=0x23;
    sendbuf[3]=(unsigned char)outChannel;
    sendbuf[4]=0x03;
    sendbuf[5]=0x02;
    sendbuf[6]=0x01;

    sendbuf[7]=(unsigned char) (pidMapListLen & 0xFF);
    sendbuf[8]=(unsigned char) (pidMapListLen & 0xFF00)>>8;

    memset(buf,0,sizeof(buf));
    communicate(ip, sendbuf, 9, buf, &slen);
    if( slen < 9 ){
        return error;
    }

    int iSendedBytes = 0;
    for (i = 0; i < sendCnt; i++)
    {
        memset(sendbuf, 0, sizeof(sendbuf));
        sendbuf[0]=0x77;
        sendbuf[1]=0x6C;
        sendbuf[2]=0x23;
        sendbuf[3]=(unsigned char)outChannel;
        sendbuf[4]=0x03;
        sendbuf[5]=0x02;
        sendbuf[6]=0x02;
        sendbuf[7]=(unsigned char)sendCnt & 0xFF;
        sendbuf[8]=(unsigned char)(i + 1);
        sendbuf[9]=(unsigned char) (pidMapListLen & 0xFF);
        sendbuf[10]=(unsigned char) (pidMapListLen & 0xFF00)>>8;
        
        iAddr = 11;
        if (pidMapBytesLen < iSendedBytes + clsProgram.pidMap_eachTransmit_numberMax * 5)
        {
            memcpy(sendbuf + iAddr, pidMapBytes + (i * clsProgram.pidMap_eachTransmit_numberMax * 5), pidMapBytesLen - iSendedBytes);          
            iAddr += (pidMapBytesLen - iSendedBytes);
            iSendedBytes = pidMapBytesLen;
        }
        else
        {
            memcpy(sendbuf + iAddr, pidMapBytes + (i * clsProgram.pidMap_eachTransmit_numberMax * 5), clsProgram.pidMap_eachTransmit_numberMax);          
            iSendedBytes += clsProgram.pidMap_eachTransmit_numberMax * 5;
            iAddr += clsProgram.pidMap_eachTransmit_numberMax * 5;
        }

        memset(buf,0,sizeof(buf));
        communicate(ip, sendbuf, iAddr, buf, &slen);

        if(slen == iAddr+1){
            if(buf[iAddr] == 0)
                res = ok;
            else{
                printf("send pidmap error\n");
                return error;
            }
        }       
    }
    return ok;

}


unsigned char SendOutputPrgInfo(char *ip, int outChn, unsigned char *muxInfoBytes, int length)
{
    unsigned char buf[1024];
    int i = 0, j=0;
    unsigned char sendbuf[200];
    int slen=0;

    int dataAddr, iAddr=0;
    ErrorTypeEm res;

    //length include crs 4 bytes
    if(muxInfoBytes || length <=0){
        printf("muxInfoBytes==null or length <0\n");
        return -1;
    }

    int eachSendLeng = 500;
    
    
    int sendCnt = length / eachSendLeng + ((length % eachSendLeng) > 0 ? 1 : 0);
    int sendedInfoAddr = 0;
    
    unsigned int crcRslt = CrcBytes(muxInfoBytes, 0, length - 4);
    muxInfoBytes[length-4] = (unsigned char) crcRslt & 0xFF000000;
    muxInfoBytes[length-4] = (unsigned char) crcRslt & 0xFF0000;
    muxInfoBytes[length-4] = (unsigned char) crcRslt & 0xFF00;
    muxInfoBytes[length-4] = (unsigned char) crcRslt & 0xFF;
    
    
    memset(sendbuf, 0, sizeof(sendbuf));
    sendbuf[0]=0x77;
    sendbuf[1]=0x6C;
    sendbuf[2]=0x23;
    sendbuf[3]=(unsigned char)outChn;
    sendbuf[4]=0x06;
    sendbuf[5]=0x02;
    sendbuf[6]=0x01;
    sendbuf[7]=(unsigned char)iAddr & 0xFF;
    sendbuf[8]=(unsigned char)(sendCnt & 0xFF);
    sendbuf[9]=(unsigned char) (length & 0xFF);
    sendbuf[10]=(unsigned char) (length & 0xFF00)>>8;

    memset(buf,0,sizeof(buf));
    communicate(ip, sendbuf, iAddr, buf, &slen);

    if(slen <= iAddr){
        printf("send SendOutputPrgInfo error \n");
        return -1;
    }
    

////////////////////////////////


    for ( i = 0; i < sendCnt; i++)
    {
        iAddr = 0;
        memset(sendbuf, 0, sizeof(sendbuf));
        sendbuf[iAddr++]=0x77;
        sendbuf[iAddr++]=0x6C;
        sendbuf[iAddr++]=0x23;
        sendbuf[iAddr++]=(unsigned char)outChn;
        sendbuf[iAddr++]=0x06;
        sendbuf[iAddr++]=0x02;
        sendbuf[iAddr++]=0x02;        
        sendbuf[iAddr++]=(unsigned char)(sendCnt & 0xFF);
        sendbuf[iAddr++]=(unsigned char) (i+1)& 0xFF;
        
        //////////////////////////////
       

        int needSendNum = length - sendedInfoAddr;
        int thisTimeSendNum = (needSendNum > eachSendLeng) ? eachSendLeng : needSendNum;
        sendbuf[iAddr++]=(unsigned char)(thisTimeSendNum & 0xFF);
        sendbuf[iAddr++]=(unsigned char)(thisTimeSendNum & 0xFF00)>>8;
        
        memcpy(sendbuf+iAddr, muxInfoBytes + sendedInfoAddr, thisTimeSendNum);
        sendedInfoAddr += thisTimeSendNum;
        iAddr += thisTimeSendNum;
        
        memset(buf,0,sizeof(buf));
        communicate(ip, sendbuf, iAddr, buf, &slen);

        if(slen <= iAddr){
            printf("send SendOutputPrgInfo error \n");
            if(buf[iAddr]!=0)            
                return 0;
        }
        
    }
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
