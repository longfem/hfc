
unsigned char AutoMux_makeMuxInfoAndSend(int outChannel, unsigned char isNeedSendDevMux)
{
	list_t sendList;
	MuxPrgInfoGet_st *pmtPrg;
	ChannelProgramSt *eachChn = null;
	Dev_prgInfo_st *prgInfo = null;

	int outPrgListLen = list_len(&clsProgram.outPrgList);
	int prgNodesLen = 0;

	if (outPrgListLen > 0 )
	{
		for (int i = 0; i < outPrgListLen; i++)
		{
			sendList.Clear();
			//removeall list

			if (outChannel != 0 && outChannel != i + 1)
				continue;

			
			list_get(&clsProgram.outPrgList, i, &eachChn);

			if(eachChn !=null){
				prgNodesLen = list_len(&eachChn->prgNodes);

				for (int j = 0; j < prgNodesLen; j++)
				{
					list_get(&eachChn->prgNodes, i, &prgInfo);

					if(prgInfo){
						pmtPrg = malloc(sizeof(MuxPrgInfoGet_st));

						pmtPrg->inChannel = prgInfo->chnId;
						pmtPrg->prgIndex = prgInfo->index;						
						list_append(&sendList, pmtPrg);
					}
											
				}

				ErrorTypeEm rslt = muxer.SendOutPrgMuxMap(i + 1, sendList);
				if (rslt != ErrorTypeEm.ok)
					return false;
			}
			

		}
	}
	return true;


}