
#include <stdio.h>
#include <stdlib.h>
#include "list.h"
#include "datastructdef.h"
#include "communicate.h"
#include "clsProgram.h"

extern ClsProgram_st clsProgram;
unsigned char AutoMux_makeMuxInfoAndSend(int outChannel, unsigned char isNeedSendDevMux)
{
	list_t sendList;
	MuxPrgInfoGet_st *pmtPrg;
	ChannelProgramSt *eachChn = NULL;
	Dev_prgInfo_st *prgInfo = NULL;

	int outPrgListLen = list_len(&clsProgram.outPrgList);
	int prgNodesLen = 0;
	int i=0, j=0;

	if (outPrgListLen > 0 )
	{
		for (i = 0; i < outPrgListLen; i++)
		{
			//sendList.Clear();

			//removeall list

			if (outChannel != 0 && outChannel != i + 1)
				continue;

			
			list_get(&clsProgram.outPrgList, i, &eachChn);

			if(eachChn !=NULL){
				prgNodesLen = list_len(&eachChn->prgNodes);

				for (j = 0; j < prgNodesLen; j++)
				{
					list_get(&eachChn->prgNodes, i, &prgInfo);

					if(prgInfo){
						pmtPrg = malloc(sizeof(MuxPrgInfoGet_st));

						pmtPrg->inChannel = prgInfo->chnId;
						pmtPrg->prgIndex = prgInfo->index;						
						list_append(&sendList, pmtPrg);
					}
											
				}

				// ErrorTypeEm rslt = muxer.SendOutPrgMuxMap(i + 1, &sendList);
				// if (rslt != ErrorTypeEm.ok)
				// 	return false;
			}
			

		}
	}
	return 1;


}


// int MakeOutPutBytes(int outChn, unsigned char *outBytes, unsigned int outlen)
// {
// 	MemoryStream memStream = NULL;
// 	outBytes = NULL;
// 	try
// 	{
// 		memStream = new MemoryStream();
// 		IFormatter formatter = new BinaryFormatter();
// 		formatter.Serialize(memStream, outPrgList[outChn - 1]);
// 		outBytes = memStream.GetBuffer();
// 		memStream.Close();

// 	}
// 	catch 
// 	{
// 		if(memStream != NULL)
// 			memStream.Close();
// 		MessageBox.Show(lang.Get("保存通道信息出错") + "!", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
// 	}
// 	return outBytes.Length;
// 	return 0;
// }

// bool MakeOutputBytesAndSend(int outChn)
// {
// 	unsigned char *tmpBytes;

// 	int sendLen = MakeOutPutBytes(outChn, out tmpBytes);
// 	if (sendLen > 0)
// 	{
// 		if (muxer.SendOutputPrgInfo(outChn, tmpBytes, sendLen))
// 			return true;
// 	}
// 	return false;
// }
