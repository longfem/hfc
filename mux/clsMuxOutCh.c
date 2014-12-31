

#include "EnumDef.h"
#include "communicate.h"

ErrorTypeEm SetOutChnTSID(int outChn, ushort tsId)
	{
		// 双字节，从低到高分别为：PAT\SDT\CAT\NIT
		byte[] headCmd = new byte[20];
		int iAddr = 0;
		headCmd[iAddr++] = 0x77;
		headCmd[iAddr++] = 0x6c;
		headCmd[iAddr++] = 0x21;
		headCmd[iAddr++] = (byte)outChn;
		headCmd[iAddr++] = 0x07;
		headCmd[iAddr++] = 0x02;

		int CmdStringAddr = iAddr;
		iAddr += ClsDataOper.LittleFormat_intToBytes(tsId, headCmd, iAddr, 2);

		Array.Copy(headCmd, _buf, iAddr);
		int readLen = netConn.WriteAndRead(_buf, iAddr);
		ErrorTypeEm checkRslt = CheckReturnBytes(headCmd, CmdStringAddr, _buf, readLen);
		if (checkRslt != ErrorTypeEm.ok)
			return ErrorTypeEm.cmd;
		if (_buf[iAddr] == 0)
			return ErrorTypeEm.ok;
		else
			return ErrorTypeEm.error;
	}