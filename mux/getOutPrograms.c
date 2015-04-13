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

extern ClsParams_st *pdb;
extern ClsProgram_st clsProgram;
// ------ 获取输出通道信息 -----
int getOutPrograms(char *ip, int outChn)
{


	if(pdb == NULL){
		printf("pdb had not inited!\n");
		return -1;
	}


	enum ErrorTypeEm res;
	unsigned int  tmpShort = 0;
	if(GetOutChnTSID(ip, outChn + 1, &tmpShort) == ok){
		pdb->pvalueTree->poutChnArray[outChn].streamId = tmpShort;
		//printf("GetOutChnTSID = %d |0x %x\n", tmpShort);
	}
	if (GetOutChnNetID(ip, outChn + 1, &tmpShort) == ok){
		pdb->pvalueTree->poutChnArray[outChn].networkId = tmpShort;
		//printf("GetOutChnNetID = %d |0x %x\n", tmpShort);
	}
	if (GetOutChnOrgNetID(ip, outChn + 1, &tmpShort) == ok){
		pdb->pvalueTree->poutChnArray[outChn].oringal_networkid = tmpShort;
		//printf("GetOutChnOrgNetID = %d |0x %x\n", tmpShort);
	}
	
	unsigned char tmpByte = 0;
	if (GetOutChnTableVer(ip, outChn + 1, &tmpByte) == ok){
		pdb->pvalueTree->poutChnArray[outChn].version = tmpByte;
		//printf("GetOutChnTableVer = %d |0x %x\n", tmpByte);
	}
	
	if (GetPidOffset(ip, outChn + 1, &tmpShort) == ok){
		clsProgram.m_autoMuxStartPid[outChn] = tmpShort;
		//printf("GetPidOffset = %d |0x %x\n", tmpShort);
	}
	
	// ---- 获取表激活命令 ----
	LoadBitrateAndTableEnable(ip, outChn);
	


	return 0;
}

