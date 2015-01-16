
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "communicate.h"
#include "list.h"
#include "clsMuxOutCh.h"
#include "getOutPrograms.h"


// ------ 获取输出通道信息 -----
int getOutPrograms(char *ip, int outChn, list_t *prginfolist)
{

	enum ErrorTypeEm res;
	unsigned int  tmpShort = 0;
	if(GetOutChnTSID(i + 1, ref tmpShort) == res.ok)
	// 	db.valueTree.outChnArray[i].streamId = tmpShort;
	// if (GetOutChnNetID(i + 1, ref tmpShort) == ErrorTypeEm.ok)
	// 	db.valueTree.outChnArray[i].networkId = tmpShort;
	// if (GetOutChnOrgNetID(i + 1, ref tmpShort) == ErrorTypeEm.ok)
	// 	db.valueTree.outChnArray[i].oringal_networkid = tmpShort;
	// byte tmpByte = 0;
	// if (GetOutChnTableVer(i + 1, ref tmpByte) == ErrorTypeEm.ok)
	// 	db.valueTree.outChnArray[i].version = tmpByte;
	// if (GetPidOffset(i + 1, ref tmpShort) == ErrorTypeEm.ok)
	// 	clsProgram.m_autoMuxStartPid[i] = tmpShort;
	// // ---- 获取表激活命令 ----
	// LoadBitrateAndTableEnable(outChn);

	return 0;
}

