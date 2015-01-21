#include "communicate.h"
#include "clsMuxprgInfoGet.h"


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
              
        *outPid = ( buf[6]<<8| buf[7]) & 0xffff; 
        
        if (*outPid < 0 || *outPid > 0x1fff)
			*outPid = 0x100;

         res = ok;

    }
    else 
    	res = error;

	return res;
	
}

ErrorTypeEm SendOutPrgMuxMap(int outChannel, list_t *muxPrgInfoList)
{

/////////////////////////////send count/////////////
	unsigned char buf[10];
    int i = 0;
    unsigned char sendbuf[12];
    int slen=0;
  
    //get call channal signal status
    enum ErrorTypeEm res;
    
    sendbuf[0]=0x77;
    sendbuf[1]=0x6C;
    sendbuf[2]=0x23;
    sendbuf[3]=(unsigned char)outChannel;
    sendbuf[4]=0x01;
    sendbuf[5]=0x02;
    sendbuf[6]=0x01;

    unsigned int muxPrgInfoListLen = list_len(muxPrgInfoList);


    sendbuf[7]=(unsigned char)(muxPrgInfoListLen & 0xFF);
    sendbuf[8]=(unsigned char)(muxPrgInfoListLen & 0xFF00 >>8);

    memset(buf,0,sizeof(buf));
    communicate(ip, sendbuf, 9, buf, &slen);
    
    //printf("\n####Recive GetOutChnNetID receive nums=[%d]\n", slen );
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

    unsigned int muxPrgInfoListLen = list_len(muxPrgInfoList);


    sendbuf[7]=(unsigned char)(muxPrgInfoListLen & 0xFF);
    sendbuf[8]=(unsigned char)(muxPrgInfoListLen & 0xFF00 >>8);

    memset(buf,0,sizeof(buf));
    communicate(ip, sendbuf, 9, buf, &slen);
    
/////////////////////////////// -------- send muxed prg info end---------    

	// -------- send muxed prg info ---------
	iAddr = 0;
	headCmd[iAddr++] = 0x77;
	headCmd[iAddr++] = 0x6c;
	headCmd[iAddr++] = 0x23;
	headCmd[iAddr++] = (byte)outChannel;
	headCmd[iAddr++] = 1;
	headCmd[iAddr++] = 2;
	headCmd[iAddr++] = 2;

	int eachSendNumMax = 200;
	int iSendTry = muxPrgInfoList.Count / eachSendNumMax + (muxPrgInfoList.Count % eachSendNumMax > 0 ? 1 : 0);

	int sendedCnt = 0;
	int thisNeedSend = eachSendNumMax;
	for (int i = 0; i < iSendTry; i++)
	{
		Array.Copy(headCmd, _buf, iAddr);
		dataAddr = iAddr;

		if (i + 1 == iSendTry) // the last
		{
			thisNeedSend = muxPrgInfoList.Count - i * eachSendNumMax;
		}

		_buf[dataAddr++] = (byte)iSendTry;
		MuxPrgInfoGet_st curPrg = (MuxPrgInfoGet_st)muxPrgInfoList[i];
		_buf[dataAddr++] = (byte)(i + 1);
		//_buf[dataAddr++] = (byte)curPrg.prgIndex;
		_buf[dataAddr++] = (byte)thisNeedSend;
		for (int j = 0; j < thisNeedSend; j++)
		{
			_buf[dataAddr++] = (byte)(((MuxPrgInfoGet_st)(muxPrgInfoList[j + sendedCnt])).inChannel);
			_buf[dataAddr++] = (byte)(((MuxPrgInfoGet_st)(muxPrgInfoList[j + sendedCnt])).prgIndex);
		}

		readLen = netConn.WriteAndRead(_buf, dataAddr);
		checkRslt = CheckReturnBytes(headCmd, iAddr, _buf, readLen);
		if (checkRslt != ErrorTypeEm.ok)
			return checkRslt;


		rslt = _buf[iAddr++];
		if (rslt != 0)
			return ErrorTypeEm.error;

		sendedCnt += thisNeedSend;
	}

	return ErrorTypeEm.ok;
}


ErrorTypeEm SendTable_pmt(int outChannel, ArrayList pmtList)
		{
			byte[] cmdBytes = new byte[20];
			int iAddr;

			if (pmtList == null || pmtList.Count == 0)
			{

				iAddr = 0;
				cmdBytes[iAddr++] = _startBytes[0];
				cmdBytes[iAddr++] = _startBytes[1];
				cmdBytes[iAddr++] = 0x22;
				cmdBytes[iAddr++] = (byte)outChannel;
				cmdBytes[iAddr++] = (byte)PsiTableType.pmt;
				cmdBytes[iAddr++] = 2;
				int CmdStringAddr = iAddr;
				cmdBytes[iAddr++] = 0;
				Array.Copy(cmdBytes, _buf, iAddr);
				int readLen = netConn.WriteAndRead(_buf, iAddr);
				ErrorTypeEm checkRslt = CheckReturnBytes(cmdBytes, CmdStringAddr, _buf, readLen);
				if (checkRslt != ErrorTypeEm.ok)
					return checkRslt;
				if (_buf[CmdStringAddr] != 0)
					return ErrorTypeEm.error;

				return ErrorTypeEm.ok;
			}

			byte iPmtCnt = 1;
			foreach (byte[] tableBytes in pmtList)
			{
				ArrayList paketList = null;
				paketList = MaketPaketSection(tableBytes, tableBytes.Length);

				if (paketList.Count == 0)
					return ErrorTypeEm.length;

				iAddr = 0;
				cmdBytes[iAddr++] = _startBytes[0];
				cmdBytes[iAddr++] = _startBytes[1];
				cmdBytes[iAddr++] = 0x22;
				cmdBytes[iAddr++] = (byte)outChannel;
				cmdBytes[iAddr++] = (byte)PsiTableType.pmt;
				cmdBytes[iAddr++] = 2;
				int CmdStringAddr = iAddr;
				cmdBytes[iAddr++] = (byte)pmtList.Count;

				Array.Copy(cmdBytes, _buf, iAddr);
				int readLen = netConn.WriteAndRead(_buf, iAddr);
				ErrorTypeEm checkRslt = CheckReturnBytes(cmdBytes, CmdStringAddr, _buf, readLen);
				if (checkRslt != ErrorTypeEm.ok)
					return checkRslt;
				if (_buf[CmdStringAddr] != 0)
					return ErrorTypeEm.error;


				for (byte i = 0; i < paketList.Count; i++)
				{
					iAddr = 0;
					cmdBytes[iAddr++] = _startBytes[0];
					cmdBytes[iAddr++] = _startBytes[1];
					cmdBytes[iAddr++] = 0x22;
					cmdBytes[iAddr++] = (byte)outChannel;
					cmdBytes[iAddr++] = (byte)PsiTableType.pmt;
					cmdBytes[iAddr++] = 3;
					cmdBytes[iAddr++] = iPmtCnt++;
					cmdBytes[iAddr++] = (byte)paketList.Count;
					cmdBytes[iAddr++] = (byte)(i + 1);
					CmdStringAddr = iAddr;

					Array.Copy(cmdBytes, 0, _buf, 0, CmdStringAddr);
					byte[] sendBytes = (byte[])(paketList[i]);
					Array.Copy(sendBytes, 0, _buf, CmdStringAddr, sendBytes.Length);
					iAddr += sendBytes.Length;
					readLen = netConn.WriteAndRead(_buf, iAddr);
					checkRslt = CheckReturnBytes(cmdBytes, CmdStringAddr, _buf, readLen);
					if (checkRslt != ErrorTypeEm.ok)
						return checkRslt;
					if (_buf[CmdStringAddr] != 0)
						return ErrorTypeEm.error;
				}
			}
			return ErrorTypeEm.ok;
		}

		internal ErrorTypeEm SendTable_psi(int outChannel, PsiTableType tableType, byte[] tableBytes, int length)
		{
			ArrayList paketList = null;
			byte[] cmdBytes = new byte[20];

			if (tableBytes == null || length < 0)
			{
				int iAddr = 0;
				cmdBytes[iAddr++] = _startBytes[0];
				cmdBytes[iAddr++] = _startBytes[1];
				cmdBytes[iAddr++] = 0x22;
				cmdBytes[iAddr++] = (byte)outChannel;
				cmdBytes[iAddr++] = (byte)tableType;
				cmdBytes[iAddr++] = 2;
				cmdBytes[iAddr++] = cmdBytes[iAddr++] = 0; // 没有该表

				Array.Copy(cmdBytes, _buf, iAddr);
				int readLen = netConn.WriteAndRead(_buf, iAddr);
				ErrorTypeEm checkRslt = CheckReturnBytes(cmdBytes, iAddr, _buf, readLen);
				if (checkRslt != ErrorTypeEm.ok)
					return checkRslt;
				if (_buf[iAddr] != 0)
					return ErrorTypeEm.error;
				return ErrorTypeEm.ok;
			}


			paketList = MaketPaketSection(tableBytes, length);
			if (paketList.Count == 0)
				return ErrorTypeEm.length;


			for (byte i = 0; i < paketList.Count; i++)
			{
				int iAddr = 0;
				cmdBytes[iAddr++] = _startBytes[0];
				cmdBytes[iAddr++] = _startBytes[1];
				cmdBytes[iAddr++] = 0x22;
				cmdBytes[iAddr++] = (byte)outChannel;
				cmdBytes[iAddr++] = (byte)tableType;
				cmdBytes[iAddr++] = 2;	
				if (length == 0)
				{
					cmdBytes[iAddr++] = cmdBytes[iAddr++] = 0; // 没有该表
				}
				else
				{
					cmdBytes[iAddr++] = (byte)paketList.Count;
					cmdBytes[iAddr++] = (byte)(i + 1);
				}
				int cmdStringLen = iAddr;

				Array.Copy(cmdBytes, _buf, iAddr);

				byte[] sendBytes = (byte[])(paketList[i]);
				Array.Copy(sendBytes, 0, _buf, iAddr, sendBytes.Length);
				iAddr += sendBytes.Length;

				int readLen = netConn.WriteAndRead(_buf, iAddr);
				ErrorTypeEm checkRslt = CheckReturnBytes(cmdBytes, cmdStringLen, _buf, readLen);
				if (checkRslt != ErrorTypeEm.ok)
					return checkRslt;
				if (_buf[cmdStringLen] != 0)
					return ErrorTypeEm.error;
			}
			return ErrorTypeEm.ok;
		}

		internal ErrorTypeEm SendTable_psi_finish(int outChannel)
		{
			int iAddr = 0;
			byte[] cmdBytes = new byte[20];

			cmdBytes[iAddr++] = _startBytes[0];
			cmdBytes[iAddr++] = _startBytes[1];
			cmdBytes[iAddr++] = 0x22;
			cmdBytes[iAddr++] = (byte)outChannel;
			cmdBytes[iAddr++] = 0;

			Array.Copy(cmdBytes, _buf, iAddr);
			int readLen = netConn.WriteAndRead(_buf, iAddr);
			ErrorTypeEm checkRslt = CheckReturnBytes(cmdBytes, iAddr, _buf, readLen);
			if (checkRslt != ErrorTypeEm.ok)
				return checkRslt;
			if (_buf[iAddr] != 0)
				return ErrorTypeEm.error;
			return ErrorTypeEm.ok;
		}

		internal ErrorTypeEm SendTable_PidMap(int outChannel, ArrayList[] pidMapListArray)
		{
			int readLen;
			ErrorTypeEm checkRslt;
			int iAddr = 0;
			byte[] cmdBytes = new byte[20];

			if (pidMapListArray == null || pidMapListArray[outChannel - 1] == null || pidMapListArray[outChannel - 1].Count == 0)
			{
				// 发送个数
				iAddr = 0;
				cmdBytes[iAddr++] = _startBytes[0];
				cmdBytes[iAddr++] = _startBytes[1];
				cmdBytes[iAddr++] = 0x23;
				cmdBytes[iAddr++] = (byte)outChannel;
				cmdBytes[iAddr++] = 2;
				cmdBytes[iAddr++] = 2;
				cmdBytes[iAddr++] = 1;
				int cmdRebackLen = iAddr;
				cmdBytes[iAddr++] = cmdBytes[iAddr++] = 0;
				Array.Copy(cmdBytes, _buf, iAddr);
				readLen = netConn.WriteAndRead(_buf, iAddr);
				checkRslt = CheckReturnBytes(cmdBytes, cmdRebackLen, _buf, readLen);
				if (checkRslt != ErrorTypeEm.ok)
					return checkRslt;
				if (_buf[cmdRebackLen] != 0)
					return ErrorTypeEm.error;

				return ErrorTypeEm.ok;
			}
			ArrayList pidMapList = pidMapListArray[outChannel - 1];
			byte[] pidMapBytes = new byte[pidMapList.Count * 5];
			iAddr = 0;
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

		internal ErrorTypeEm DirectlyTransmit_sendMap(int outChannel, ArrayList pidMapList)
		{
			int readLen;
			byte[] cmdBytes = new byte[20];
			int iAddr = 0;
			if (pidMapList == null || pidMapList.Count == 0)
			{
				cmdBytes[iAddr++] = _startBytes[0];
				cmdBytes[iAddr++] = _startBytes[1];
				cmdBytes[iAddr++] = 0x23;
				cmdBytes[iAddr++] = (byte)outChannel;
				cmdBytes[iAddr++] = 3;
				cmdBytes[iAddr++] = 2;
				cmdBytes[iAddr++] = 1;
				int cmdStringAddr = iAddr;
				cmdBytes[iAddr++] = cmdBytes[iAddr++] = 0;
				Array.Copy(cmdBytes, _buf, iAddr);
				readLen = netConn.WriteAndRead(_buf, iAddr);
				ErrorTypeEm checkRslt = CheckReturnBytes(cmdBytes, cmdStringAddr, _buf, readLen);
				if (checkRslt != ErrorTypeEm.ok)
					return checkRslt;
				if (_buf[cmdStringAddr] != 0)
					return ErrorTypeEm.error;
				return ErrorTypeEm.ok;
			}

			byte[] pidMapBytes = new byte[pidMapList.Count * 5];

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
			cmdBytes[iAddr++] = 3;
			cmdBytes[iAddr++] = 2;
			cmdBytes[iAddr++] = 1;
			iAddr += ClsDataOper.LittleFormat_intToBytes(pidMapList.Count, cmdBytes, iAddr, 2);
			//netConn.Write(cmdBytes, iAddr, DeviceRead_callBack);
			//readEndSem.WaitOne();
			Array.Copy(cmdBytes, _buf, iAddr);
			readLen = netConn.WriteAndRead(_buf, iAddr);
			iAddr -= 2;
			if (readLen <= iAddr)
				return ErrorTypeEm.length;
			for (int j = 0; j < iAddr; j++)
			{
				if (cmdBytes[j] != _buf[j])
					return ErrorTypeEm.cmd;
			}
			if (_buf[iAddr] != 0)
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
				cmdBytes[iAddr++] = 3;
				cmdBytes[iAddr++] = 2;
				cmdBytes[iAddr++] = 2;
				int cmdFinishAddr = iAddr;
				cmdBytes[iAddr++] = (byte)sendCnt;
				cmdBytes[iAddr++] = (byte)(i + 1);


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

				//netConn.Write(_buf, iAddr, DeviceRead_callBack);
				//readEndSem.WaitOne();
				readLen = netConn.WriteAndRead(_buf, iAddr);
				if (readLen <= cmdFinishAddr)
					return ErrorTypeEm.length;
				for (int j = 0; j < cmdFinishAddr; j++)
				{
					if (cmdBytes[j] != _buf[j])
						return ErrorTypeEm.cmd;
				}
				if (_buf[cmdFinishAddr] != 0)
					return ErrorTypeEm.error;
			}
			ErrorTypeEm rtnRslt = ErrorTypeEm.ok;
			//	DirectlyTransmit_mapSendFinish(outChannel);
			return rtnRslt;
		}
