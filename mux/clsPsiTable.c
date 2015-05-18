#include <stdio.h>
#include "datastructdef.h"
#include "list.h"
unsigned char Muxer_ChannelScan(void)
{
	return  22;
}

Dev_prgInfo_st* buildOoutPrgList()
{
	//prg1
	Dev_prgInfo_st* p_Dev_prgInfo_t=(Dev_prgInfo_st*)malloc(sizeof(Dev_prgInfo_st));
	p_Dev_prgInfo_t->prgNum=301;
	p_Dev_prgInfo_t->pmtPid=257;
	p_Dev_prgInfo_t->chnId=2;p_Dev_prgInfo_t->index=1;
	p_Dev_prgInfo_t->isCrypto=FALSE;p_Dev_prgInfo_t->networkId=2184;
	p_Dev_prgInfo_t->newPcrPid=8190;
	p_Dev_prgInfo_t->pmtPid=257;
	p_Dev_prgInfo_t->prgName="CCTV1";
	p_Dev_prgInfo_t->pmtPid=257;
	p_Dev_prgInfo_t->providerName="CCTV";
	p_Dev_prgInfo_t->serviceType=1;
	p_Dev_prgInfo_t->streamId=3;
	p_Dev_prgInfo_t->userNew=FALSE;
	//p_Dev_prgInfo_t->p_next=NULL;

}

unsigned  int  CrcBytes(unsigned char *inBytes,int offset, int length)
{
	if (1)
	{
		unsigned  int  crc_table[] = {
			0x00000000, 0x04c11db7, 0x09823b6e, 0x0d4326d9, 0x130476dc, 0x17c56b6b,
			0x1a864db2, 0x1e475005, 0x2608edb8, 0x22c9f00f, 0x2f8ad6d6, 0x2b4bcb61,
			0x350c9b64, 0x31cd86d3, 0x3c8ea00a, 0x384fbdbd, 0x4c11db70, 0x48d0c6c7,
			0x4593e01e, 0x4152fda9, 0x5f15adac, 0x5bd4b01b, 0x569796c2, 0x52568b75,
			0x6a1936c8, 0x6ed82b7f, 0x639b0da6, 0x675a1011, 0x791d4014, 0x7ddc5da3,
			0x709f7b7a, 0x745e66cd, 0x9823b6e0, 0x9ce2ab57, 0x91a18d8e, 0x95609039,
			0x8b27c03c, 0x8fe6dd8b, 0x82a5fb52, 0x8664e6e5, 0xbe2b5b58, 0xbaea46ef,
			0xb7a96036, 0xb3687d81, 0xad2f2d84, 0xa9ee3033, 0xa4ad16ea, 0xa06c0b5d,
			0xd4326d90, 0xd0f37027, 0xddb056fe, 0xd9714b49, 0xc7361b4c, 0xc3f706fb,
			0xceb42022, 0xca753d95, 0xf23a8028, 0xf6fb9d9f, 0xfbb8bb46, 0xff79a6f1,
			0xe13ef6f4, 0xe5ffeb43, 0xe8bccd9a, 0xec7dd02d, 0x34867077, 0x30476dc0,
			0x3d044b19, 0x39c556ae, 0x278206ab, 0x23431b1c, 0x2e003dc5, 0x2ac12072,
			0x128e9dcf, 0x164f8078, 0x1b0ca6a1, 0x1fcdbb16, 0x018aeb13, 0x054bf6a4,
			0x0808d07d, 0x0cc9cdca, 0x7897ab07, 0x7c56b6b0, 0x71159069, 0x75d48dde,
			0x6b93dddb, 0x6f52c06c, 0x6211e6b5, 0x66d0fb02, 0x5e9f46bf, 0x5a5e5b08,
			0x571d7dd1, 0x53dc6066, 0x4d9b3063, 0x495a2dd4, 0x44190b0d, 0x40d816ba,
			0xaca5c697, 0xa864db20, 0xa527fdf9, 0xa1e6e04e, 0xbfa1b04b, 0xbb60adfc,
			0xb6238b25, 0xb2e29692, 0x8aad2b2f, 0x8e6c3698, 0x832f1041, 0x87ee0df6,
			0x99a95df3, 0x9d684044, 0x902b669d, 0x94ea7b2a, 0xe0b41de7, 0xe4750050,
			0xe9362689, 0xedf73b3e, 0xf3b06b3b, 0xf771768c, 0xfa325055, 0xfef34de2,
			0xc6bcf05f, 0xc27dede8, 0xcf3ecb31, 0xcbffd686, 0xd5b88683, 0xd1799b34,
			0xdc3abded, 0xd8fba05a, 0x690ce0ee, 0x6dcdfd59, 0x608edb80, 0x644fc637,
			0x7a089632, 0x7ec98b85, 0x738aad5c, 0x774bb0eb, 0x4f040d56, 0x4bc510e1,
			0x46863638, 0x42472b8f, 0x5c007b8a, 0x58c1663d, 0x558240e4, 0x51435d53,
			0x251d3b9e, 0x21dc2629, 0x2c9f00f0, 0x285e1d47, 0x36194d42, 0x32d850f5,
			0x3f9b762c, 0x3b5a6b9b, 0x0315d626, 0x07d4cb91, 0x0a97ed48, 0x0e56f0ff,
			0x1011a0fa, 0x14d0bd4d, 0x19939b94, 0x1d528623, 0xf12f560e, 0xf5ee4bb9,
			0xf8ad6d60, 0xfc6c70d7, 0xe22b20d2, 0xe6ea3d65, 0xeba91bbc, 0xef68060b,
			0xd727bbb6, 0xd3e6a601, 0xdea580d8, 0xda649d6f, 0xc423cd6a, 0xc0e2d0dd,
			0xcda1f604, 0xc960ebb3, 0xbd3e8d7e, 0xb9ff90c9, 0xb4bcb610, 0xb07daba7,
			0xae3afba2, 0xaafbe615, 0xa7b8c0cc, 0xa379dd7b, 0x9b3660c6, 0x9ff77d71,
			0x92b45ba8, 0x9675461f, 0x8832161a, 0x8cf30bad, 0x81b02d74, 0x857130c3,
			0x5d8a9099, 0x594b8d2e, 0x5408abf7, 0x50c9b640, 0x4e8ee645, 0x4a4ffbf2,
			0x470cdd2b, 0x43cdc09c, 0x7b827d21, 0x7f436096, 0x7200464f, 0x76c15bf8,
			0x68860bfd, 0x6c47164a, 0x61043093, 0x65c52d24, 0x119b4be9, 0x155a565e,
			0x18197087, 0x1cd86d30, 0x029f3d35, 0x065e2082, 0x0b1d065b, 0x0fdc1bec,
			0x3793a651, 0x3352bbe6, 0x3e119d3f, 0x3ad08088, 0x2497d08d, 0x2056cd3a,
			0x2d15ebe3, 0x29d4f654, 0xc5a92679, 0xc1683bce, 0xcc2b1d17, 0xc8ea00a0,
			0xd6ad50a5, 0xd26c4d12, 0xdf2f6bcb, 0xdbee767c, 0xe3a1cbc1, 0xe760d676,
			0xea23f0af, 0xeee2ed18, 0xf0a5bd1d, 0xf464a0aa, 0xf9278673, 0xfde69bc4,
			0x89b8fd09, 0x8d79e0be, 0x803ac667, 0x84fbdbd0, 0x9abc8bd5, 0x9e7d9662,
			0x933eb0bb, 0x97ffad0c, 0xafb010b1, 0xab710d06, 0xa6322bdf, 0xa2f33668,
			0xbcb4666d, 0xb8757bda, 0xb5365d03, 0xb1f740b4};

		int i;
		int len = offset + length;
		unsigned  int  crc = 0xffffffff;

		for (i = offset; i < len; i++)
			crc = (unsigned  int)((crc << 8) ^ crc_table[((crc >> 24) ^ inBytes[i]) & 0xff]);
		return crc;
	}
	else
	{
		return 0x19820524;
	}

}

static int BigFormat_intToBytes(int intValue, unsigned char outBytes[], int offset, int bytesLen)
{
	int i;

	for (i = bytesLen; i > 0; i--)
	{
		outBytes[offset + i - 1] = (unsigned char)intValue;
		intValue >>= 8;
	}

	return bytesLen;
}

static int BigFormat_uintToBytes(unsigned long intValue, unsigned char outBytes[], int offset, int bytesLen)
{
	int i;

	for (i = bytesLen; i > 0; i--)
	{
		outBytes[offset + i - 1] = (unsigned char)(intValue);
		intValue >>= 8;
	}

	return bytesLen;
}

static int BigFormat_fromBytes(int offset, int length, unsigned char inBytes[])
{
	int rtnInt = 0;
	int i;
	for (i = 0; i < length; i++)
	{
		rtnInt <<= 8;
		rtnInt += inBytes[offset++];
	}
	return rtnInt;
}

int  CreatePat(list_t  prginfolist,list_t  userprginfolist,unsigned char patTable[],int streamId, int networkId, int verisonNumber)
{

	int i;
	int iAddr = 0;
	Dev_prgInfo_st *ptmpPrgInfo;
	User_prgInfo_t *userptmpPrgInfo;
	pat_senction_st* p_pat=(pat_senction_st*)malloc(sizeof(pat_senction_st));
	unsigned char tmpBytes[1024];

	// -- head --
	tmpBytes[iAddr++] = 0x47;
	tmpBytes[iAddr++] = 0x40;
	tmpBytes[iAddr++] = 0;
	tmpBytes[iAddr++] = 0x10;
	tmpBytes[iAddr++] = 0;



	p_pat->table_id = 0;
	tmpBytes[iAddr++] = 0;
	int sectionLenAddr = iAddr;


	p_pat->section_syntax_indicator = 0x1;
	p_pat->flag_0=0;
	p_pat->reserved0=0x3;

	iAddr += 2;

	int contentStartAddr = iAddr;
	p_pat->transport_stream_id = streamId;
	iAddr += BigFormat_intToBytes(streamId, tmpBytes, iAddr, 2);


	p_pat->reserved1= 0x3;
	p_pat->version_number= verisonNumber;
	p_pat->current_next_indicator= 0x1;
	p_pat->section_number= 0;
	p_pat->last_section_number= 0;



	//ipAddr=10
	tmpBytes[iAddr++] = (unsigned char)((0x3 << 6) // reserved 2bit
		| ((verisonNumber & 0x1f) << 1)  // version number 5bit
		| (1 << 0)); // current_next_indicator //1bit
	//ipAddr=11
	tmpBytes[iAddr++] = 0; // section number 8bit=1Byte
	tmpBytes[iAddr++] = 0; // last_section_number 8bit=1Byte


	if (networkId > 0) // add network id
	{
		tmpBytes[iAddr++] = 0;
		tmpBytes[iAddr++] = 0;
		//networkId = 0x10;
		iAddr += BigFormat_intToBytes(((0xe0 << 8) | (networkId & 0x1fff)), tmpBytes, iAddr, 2);
	}

	//p_pat->p_first_program= NULL;
	if(FALSE)
	{
	}else
	{

		for (i = 0; i <list_len(&prginfolist); i++)
		{
			list_get(&prginfolist, i, &ptmpPrgInfo);
			//patPrg_t* p_patPrg_t=(patPrg_t*)malloc(sizeof(patPrg_t));


			//DIRECETLY COPY HERE
			//p_patPrg_t->program_number=ptmpPrgInfo->prgNum;
			//p_patPrg_t->pid=ptmpPrgInfo->pmtPid;


			//printf("prgnumber:%d \n",ptmpPrgInfo->prgNum);


			iAddr += BigFormat_intToBytes(ptmpPrgInfo->prgNum, tmpBytes, iAddr, 2);//program_number	 :16; //��Ŀ��
			iAddr += BigFormat_intToBytes(((0xe0 << 8) | ptmpPrgInfo->pmtPid), tmpBytes, iAddr, 2);

			//printf("prgnumber:%d ",p_patPrg_t->program_number);
		}

		for (i = 0; i <list_len(&userprginfolist); i++)
		{
			list_get(&userprginfolist, i, &userptmpPrgInfo);
			//patPrg_t* p_patPrg_t=(patPrg_t*)malloc(sizeof(patPrg_t));


			//DIRECETLY COPY HERE
			//p_patPrg_t->program_number=ptmpPrgInfo->prgNum;
			//p_patPrg_t->pid=ptmpPrgInfo->pmtPid;


			//printf("prgnumber:%d \n",ptmpPrgInfo->prgNum);


			iAddr += BigFormat_intToBytes(userptmpPrgInfo->prgNum, tmpBytes, iAddr, 2);//program_number	 :16; //��Ŀ��
			iAddr += BigFormat_intToBytes(((0xe0 << 8) | userptmpPrgInfo->pmtPid), tmpBytes, iAddr, 2);

			//printf("prgnumber:%d ",p_patPrg_t->program_number);
		}
		BigFormat_intToBytes(((0xb << 12) | ((iAddr + 4 - sectionLenAddr - 2) & 0xfff)), tmpBytes, sectionLenAddr, 2);
		p_pat->section_length = iAddr + 4 - sectionLenAddr - 2;

	}


	unsigned  long crcWord=  CrcBytes(tmpBytes,5,iAddr - 5);
	iAddr += BigFormat_uintToBytes(crcWord, tmpBytes, iAddr, 4);


#if 0

	for(i=0; i<iAddr; i++)
		printf("  %d          offset:%d  \n",tmpBytes[i],i);

	printf("length is %d\n",iAddr);
#endif

	if (iAddr <= 188)
	{
		for (i = 0; i < iAddr; i++)
			patTable[i] = tmpBytes[i];
		for (i = iAddr; i < 188; i++)
			patTable[i] = 0xff;
		return 1;
	}

	iAddr -= 4;
	p_pat->crc32 = (tmpBytes[iAddr++] << 24) | (tmpBytes[iAddr++] << 16) | (tmpBytes[iAddr++] << 8) | tmpBytes[iAddr++];
	free(p_pat);
	p_pat=NULL;

	return 0;
}
int  CreatePmt(Dev_prgInfo_st *outPrgInfo, unsigned char pmtTable[], int verisonNumber)
{
	unsigned char tmpBytes[1024];
	pmt_senction_st* p_pmt=(pmt_senction_st*)malloc(sizeof(pmt_senction_st));


	int offStep = 0;
	int iAddr = 0;
	int i;
	// head
	tmpBytes[iAddr++] = 0x47;
	iAddr += BigFormat_intToBytes(((0x2 << 13) | outPrgInfo->pmtPid), tmpBytes, iAddr, 2); // es_info_length

	tmpBytes[iAddr++] = 0x10;
	//printf("debug :offset %d  value %d\n",iAddr,tmpBytes[iAddr-1]);
	tmpBytes[iAddr++] = 0;



	p_pmt->table_id = 0x02;
	tmpBytes[iAddr++] = 0x02;
	//tmpByteValue = buf[iAddr++];//section_syntax_indicator+flag_0+erserverd0=4bit+section_length=16bits
	p_pmt->section_syntax_indicator = 0x1;
	p_pmt->flag_0 = 0;
	p_pmt->reserved0 = 0x3;


	int sescLen = 0;
	sescLen += 2;//�����sectionLenth�����2���ֽ�
	iAddr += 2;

	p_pmt->program_number = outPrgInfo->prgNum;
	offStep = BigFormat_intToBytes(outPrgInfo->prgNum, tmpBytes, iAddr, 2);//program_number
	sescLen += offStep;
	iAddr += offStep;

	p_pmt->reserved1 = 0x3;
	p_pmt->version_number = verisonNumber;
	p_pmt->current_next_indicator = 0x1;

	sescLen++;
	//����5����PATһ��
	tmpBytes[iAddr++] = (unsigned char)((3 << 6) // reserved
		| ((verisonNumber & 0x1f) << 1)  // version number
		| (1 << 0)); // current_next_indicator



	p_pmt->section_number = 0;
	sescLen++;
	p_pmt->last_section_number = 0;
	sescLen++;
	tmpBytes[iAddr++] = 0; // section number
	tmpBytes[iAddr++] = 0; // last_section_number


	p_pmt->reserved2 = 0x7;
	p_pmt->pcrPid = outPrgInfo->newPcrPid;
	sescLen += 2;

	//iAddr=13 ,14
	iAddr += BigFormat_intToBytes(((0x7 << 13) | outPrgInfo->newPcrPid), tmpBytes, iAddr, 2);//reserved2+pcr_pid=2Byte


	//CA�����߼�����Ҫ�ȴ����
	int prgInfoLen = 0;
	iAddr += 2;



	//int indexL1 = 1;




	if (outPrgInfo->pmtDesListLen!=0)
	{

		Commdes_t* desInfoTmp = outPrgInfo->pmtDesList;


		//p_pmt->pmtDesListLen=outPrgInfo->pmtDesListLen;
		//	p_pmt->pmtDesList=(Commdes_t*)malloc(sizeof(Commdes_t)*(outPrgInfo->pmtDesListLen));
		//	Commdes_t* p_Commdes_st= p_pmt->pmtDesList;

		for (i = 0; i < outPrgInfo->pmtDesListLen; i++)
		{
			prgInfoLen++;
			prgInfoLen++;
			prgInfoLen += desInfoTmp->dataLen;


			//	p_Commdes_st->tag = desInfoTmp->tag;
			//p_Commdes_st->index = indexL1++;
			//	p_Commdes_st->data = malloc(desInfoTmp->dataLen);
			//	memcpy(p_Commdes_st->data, desInfoTmp->data, desInfoTmp->dataLen);


			tmpBytes[iAddr++] = (unsigned char)(desInfoTmp->tag);
			tmpBytes[iAddr++] = (unsigned char)(desInfoTmp->dataLen);


			memcpy(tmpBytes+iAddr, desInfoTmp->data,desInfoTmp->dataLen);
			iAddr += desInfoTmp->dataLen;

			desInfoTmp++;
			//p_Commdes_st++;


		}


	}


	//iaddr=28��ʼ
	p_pmt->reserved3 = 0xf;
	p_pmt->program_info_length = prgInfoLen;
	sescLen += p_pmt->program_info_length;
	BigFormat_intToBytes(((0xf << 12)  // reserved3
		| (p_pmt->program_info_length & 0xfff)), tmpBytes, iAddr - p_pmt->program_info_length - 2, 2);

	//sescLenǰ���program_info_length���λ��=9+program_info_length,�������� sescLen����׷��


	if (outPrgInfo->pdataStreamListLen !=0)
	{
		DataStream_t* dataStreamInfoTmp = outPrgInfo->pdataStreamList;


		//p_pmt->pdataStreamListLen=outPrgInfo->pdataStreamListLen;
		//p_pmt->pdataStreamList=(DataStream_t*)malloc(sizeof(DataStream_t)*(outPrgInfo->pdataStreamListLen));
		//DataStream_t* p_DataStream_st= p_pmt->pdataStreamList;



		for (i = 0; i < outPrgInfo->pdataStreamListLen; i++)
		{

			//p_DataStream_st->streamType = dataStreamInfoTmp->streamType;
			sescLen++;
			//p_DataStream_st->outPid = dataStreamInfoTmp->outPid;
			sescLen += 2;


			int indexL2 = 1;
			sescLen += 2;


			tmpBytes[iAddr++] = (unsigned char)(dataStreamInfoTmp->streamType);
			iAddr += BigFormat_intToBytes(((0x7 << 13) | dataStreamInfoTmp->outPid), tmpBytes, iAddr, 2);
			int esInfoLenAddr = iAddr;
			iAddr += 2;


			if (dataStreamInfoTmp->desNodeLen != 0)
			{
				int j;
				Commdes_t* desInfoTmp = dataStreamInfoTmp->desNode;

				//dataStreamInfoTmp->desNodeLen=dataStreamInfoTmp->desNodeLen;
				//dataStreamInfoTmp->desNode=(Commdes_t*)malloc(sizeof(Commdes_t)*(dataStreamInfoTmp->desNodeLen));
				//Commdes_t* p_Commdes_st= dataStreamInfoTmp->desNode;


				for (j = 0; j < dataStreamInfoTmp->desNodeLen; j++)
				{

					sescLen++;
					sescLen++;
					sescLen += desInfoTmp->dataLen;


					//p_Commdes_st->tag = desInfoTmp->tag;
					//p_Commdes_st->index = indexL2++;
					//p_Commdes_st->data = malloc(desInfoTmp->dataLen);
					//memcpy(p_Commdes_st->data, desInfoTmp->data, desInfoTmp->dataLen);


					tmpBytes[iAddr++] = (unsigned char)(desInfoTmp->tag);
					tmpBytes[iAddr++] = (unsigned char)(desInfoTmp->dataLen);


					memcpy(tmpBytes+iAddr, desInfoTmp->data,desInfoTmp->dataLen);
					iAddr += desInfoTmp->dataLen;


					desInfoTmp++;
					//	p_Commdes_st++;

				}
			}
			//p_DataStream_st->index = indexL1++;
			dataStreamInfoTmp++;
			//p_DataStream_st++;
			BigFormat_intToBytes(((0xf << 12)  // reserved
				| ((iAddr - esInfoLenAddr - 2) & 0xfff)), tmpBytes, esInfoLenAddr, 2);


		}


	}


	p_pmt->section_length = sescLen + 4;//
	BigFormat_intToBytes((0xb000//(0x7 << 13)  // 1011=b
		| (p_pmt->section_length & 0xfff)), tmpBytes, iAddr + 4 - p_pmt->section_length - 2, 2); // section_length��,�ڸղ�����ABA����ţ�����ADDR��������

	// -- crc -
	unsigned  long crcWord=  CrcBytes(tmpBytes,5,iAddr - 5);

	iAddr += BigFormat_uintToBytes(crcWord, tmpBytes, iAddr, 4);
	//for git
#if 0
	for(i=0; i<iAddr; i++)
		printf("  %d          offset:%d  \n",tmpBytes[i],i);

	printf("length is %d\n",iAddr);

#endif
	if (iAddr <= 188)
	{
		for (i = 0; i < iAddr; i++)
			pmtTable[i] = tmpBytes[i];
		for (i = iAddr; i < 188; i++)
			pmtTable[i] = 0xff;
		return 1;
	}

	iAddr -= 4;
	p_pmt->crc32 = (tmpBytes[iAddr++] << 24) | (tmpBytes[iAddr++] << 16) | (tmpBytes[iAddr++] << 8) | tmpBytes[iAddr++];
	free(p_pmt);
	p_pmt=NULL;

	return 0;
}
int  CreatePmtUser(User_prgInfo_t *outPrgInfo, unsigned char pmtTable[], int verisonNumber)
{
	unsigned char tmpBytes[1024];
	pmt_senction_st* p_pmt=(pmt_senction_st*)malloc(sizeof(pmt_senction_st));


	int offStep = 0;
	int iAddr = 0;
	int i;
	// head
	tmpBytes[iAddr++] = 0x47;
	iAddr += BigFormat_intToBytes(((0x2 << 13) | outPrgInfo->pmtPid), tmpBytes, iAddr, 2); // es_info_length

	tmpBytes[iAddr++] = 0x10;
	//printf("debug :offset %d  value %d\n",iAddr,tmpBytes[iAddr-1]);
	tmpBytes[iAddr++] = 0;



	p_pmt->table_id = 0x02;
	tmpBytes[iAddr++] = 0x02;
	//tmpByteValue = buf[iAddr++];//section_syntax_indicator+flag_0+erserverd0=4bit+section_length=16bits
	p_pmt->section_syntax_indicator = 0x1;
	p_pmt->flag_0 = 0;
	p_pmt->reserved0 = 0x3;


	int sescLen = 0;
	sescLen += 2;//�����sectionLenth�����2���ֽ�
	iAddr += 2;

	p_pmt->program_number = outPrgInfo->prgNum;
	offStep = BigFormat_intToBytes(outPrgInfo->prgNum, tmpBytes, iAddr, 2);//program_number
	sescLen += offStep;
	iAddr += offStep;

	p_pmt->reserved1 = 0x3;
	p_pmt->version_number = verisonNumber;
	p_pmt->current_next_indicator = 0x1;

	sescLen++;
	//����5����PATһ��
	tmpBytes[iAddr++] = (unsigned char)((3 << 6) // reserved
		| ((verisonNumber & 0x1f) << 1)  // version number
		| (1 << 0)); // current_next_indicator



	p_pmt->section_number = 0;
	sescLen++;
	p_pmt->last_section_number = 0;
	sescLen++;
	tmpBytes[iAddr++] = 0; // section number
	tmpBytes[iAddr++] = 0; // last_section_number


	p_pmt->reserved2 = 0x7;
	p_pmt->pcrPid = outPrgInfo->newPcrPid;
	sescLen += 2;

	//iAddr=13 ,14
	iAddr += BigFormat_intToBytes(((0x7 << 13) | outPrgInfo->newPcrPid), tmpBytes, iAddr, 2);//reserved2+pcr_pid=2Byte


	//CA�����߼�����Ҫ�ȴ����
	int prgInfoLen = 0;
	iAddr += 2;



	//int indexL1 = 1;




	if (outPrgInfo->pmtDesListLen!=0)
	{

		Commdes_t* desInfoTmp = outPrgInfo->pmtDesList;


		//p_pmt->pmtDesListLen=outPrgInfo->pmtDesListLen;
		//	p_pmt->pmtDesList=(Commdes_t*)malloc(sizeof(Commdes_t)*(outPrgInfo->pmtDesListLen));
		//	Commdes_t* p_Commdes_st= p_pmt->pmtDesList;

		for (i = 0; i < outPrgInfo->pmtDesListLen; i++)
		{
			prgInfoLen++;
			prgInfoLen++;
			prgInfoLen += desInfoTmp->dataLen;


			//	p_Commdes_st->tag = desInfoTmp->tag;
			//p_Commdes_st->index = indexL1++;
			//	p_Commdes_st->data = malloc(desInfoTmp->dataLen);
			//	memcpy(p_Commdes_st->data, desInfoTmp->data, desInfoTmp->dataLen);


			tmpBytes[iAddr++] = (unsigned char)(desInfoTmp->tag);
			tmpBytes[iAddr++] = (unsigned char)(desInfoTmp->dataLen);


			memcpy(tmpBytes+iAddr, desInfoTmp->data,desInfoTmp->dataLen);
			iAddr += desInfoTmp->dataLen;

			desInfoTmp++;
			//p_Commdes_st++;


		}


	}


	//iaddr=28��ʼ
	p_pmt->reserved3 = 0xf;
	p_pmt->program_info_length = prgInfoLen;
	sescLen += p_pmt->program_info_length;
	BigFormat_intToBytes(((0xf << 12)  // reserved3
		| (p_pmt->program_info_length & 0xfff)), tmpBytes, iAddr - p_pmt->program_info_length - 2, 2);

	//sescLenǰ���program_info_length���λ��=9+program_info_length,�������� sescLen����׷��


	if (outPrgInfo->pdataStreamListLen !=0)
	{
		DataStream_t* dataStreamInfoTmp = outPrgInfo->pdataStreamList;


		//p_pmt->pdataStreamListLen=outPrgInfo->pdataStreamListLen;
		//p_pmt->pdataStreamList=(DataStream_t*)malloc(sizeof(DataStream_t)*(outPrgInfo->pdataStreamListLen));
		//DataStream_t* p_DataStream_st= p_pmt->pdataStreamList;



		for (i = 0; i < outPrgInfo->pdataStreamListLen; i++)
		{

			//p_DataStream_st->streamType = dataStreamInfoTmp->streamType;
			sescLen++;
			//p_DataStream_st->outPid = dataStreamInfoTmp->outPid;
			sescLen += 2;


			int indexL2 = 1;
			sescLen += 2;


			tmpBytes[iAddr++] = (unsigned char)(dataStreamInfoTmp->streamType);
			iAddr += BigFormat_intToBytes(((0x7 << 13) | dataStreamInfoTmp->outPid), tmpBytes, iAddr, 2);
			int esInfoLenAddr = iAddr;
			iAddr += 2;


			if (dataStreamInfoTmp->desNodeLen != 0)
			{
				int j;
				Commdes_t* desInfoTmp = dataStreamInfoTmp->desNode;

				//dataStreamInfoTmp->desNodeLen=dataStreamInfoTmp->desNodeLen;
				//dataStreamInfoTmp->desNode=(Commdes_t*)malloc(sizeof(Commdes_t)*(dataStreamInfoTmp->desNodeLen));
				//Commdes_t* p_Commdes_st= dataStreamInfoTmp->desNode;


				for (j = 0; j < dataStreamInfoTmp->desNodeLen; j++)
				{

					sescLen++;
					sescLen++;
					sescLen += desInfoTmp->dataLen;


					//p_Commdes_st->tag = desInfoTmp->tag;
					//p_Commdes_st->index = indexL2++;
					//p_Commdes_st->data = malloc(desInfoTmp->dataLen);
					//memcpy(p_Commdes_st->data, desInfoTmp->data, desInfoTmp->dataLen);


					tmpBytes[iAddr++] = (unsigned char)(desInfoTmp->tag);
					tmpBytes[iAddr++] = (unsigned char)(desInfoTmp->dataLen);


					memcpy(tmpBytes+iAddr, desInfoTmp->data,desInfoTmp->dataLen);
					iAddr += desInfoTmp->dataLen;


					desInfoTmp++;
					//	p_Commdes_st++;

				}
			}
			//p_DataStream_st->index = indexL1++;
			dataStreamInfoTmp++;
			//p_DataStream_st++;
			BigFormat_intToBytes(((0xf << 12)  // reserved
				| ((iAddr - esInfoLenAddr - 2) & 0xfff)), tmpBytes, esInfoLenAddr, 2);


		}


	}


	p_pmt->section_length = sescLen + 4;//
	BigFormat_intToBytes((0xb000//(0x7 << 13)  // 1011=b
		| (p_pmt->section_length & 0xfff)), tmpBytes, iAddr + 4 - p_pmt->section_length - 2, 2); // section_length��,�ڸղ�����ABA����ţ�����ADDR��������

	// -- crc -
	unsigned  long crcWord=  CrcBytes(tmpBytes,5,iAddr - 5);

	iAddr += BigFormat_uintToBytes(crcWord, tmpBytes, iAddr, 4);
	//for git
#if 0
	for(i=0; i<iAddr; i++)
		printf("  %d          offset:%d  \n",tmpBytes[i],i);

	printf("length is %d\n",iAddr);

#endif
	if (iAddr <= 188)
	{
		for (i = 0; i < iAddr; i++)
			pmtTable[i] = tmpBytes[i];
		for (i = iAddr; i < 188; i++)
			pmtTable[i] = 0xff;
		return 1;
	}

	iAddr -= 4;
	p_pmt->crc32 = (tmpBytes[iAddr++] << 24) | (tmpBytes[iAddr++] << 16) | (tmpBytes[iAddr++] << 8) | tmpBytes[iAddr++];
	free(p_pmt);
	p_pmt=NULL;

	return 0;
}

int CreateSdt(list_t  prginfolist,list_t  userprginfolist, unsigned char sdtTable[], int streamId, int originalNetworkId, int verisonNumber)
{


	int i;
	int iAddr = 0;
	//sdtTable = new byte[6 * 188];
	//byte[] tmpBytes = new byte[8 * 188];
	unsigned char tmpBytes[8 * 188];
	Dev_prgInfo_st *ptmpPrgInfo;
	User_prgInfo_t *userptmpPrgInfo;



	// head
	tmpBytes[iAddr++] = 0x47;
	tmpBytes[iAddr++] = 0x40;
	tmpBytes[iAddr++] = 0x11;
	tmpBytes[iAddr++] = 0x10;
	tmpBytes[iAddr++] = 0;

	// -- sdt --
	tmpBytes[iAddr++] = 0x42;
	int sectionLenAddr = iAddr;
	iAddr += 2; // �����ֽ�
	iAddr += BigFormat_intToBytes(streamId, tmpBytes, iAddr, 2);
	tmpBytes[iAddr++] = (unsigned char)((3 << 6) // reserved
		| ((verisonNumber & 0x1f) << 1)  // version number
		| (1 << 0)); // current_next_indicator
	tmpBytes[iAddr++] = 0; // section number
	tmpBytes[iAddr++] = 0; // last_section_number
	iAddr += BigFormat_intToBytes(originalNetworkId, tmpBytes, iAddr, 2);
	tmpBytes[iAddr++] = 0xff; // reaserved_future_use



	//
	for (i = 0; i <list_len(&prginfolist); i++)
	{
		list_get(&prginfolist, i, &ptmpPrgInfo);
		iAddr += BigFormat_intToBytes(ptmpPrgInfo->prgNum, tmpBytes, iAddr, 2);
		tmpBytes[iAddr++] = (unsigned char)((0x3f << 2) // reserved_future_use
			| (0 << 1) // EIT_schedule_flag
			| (0 << 0)); // EIT_present_following_flag
		int des_length_addr = iAddr;
		iAddr += 2; // �����ֽ�

		// ����Զ������
		tmpBytes[iAddr++] = 0x48;

		unsigned char *prgNameTmp=malloc(ptmpPrgInfo->prgNameLen);
		memcpy(prgNameTmp, ptmpPrgInfo->prgName, ptmpPrgInfo->prgNameLen);

		//   if (ptmpPrgInfo->prgNameLen == 0)
		// prgNameTmp = string.Empty;//�����Ҫ��
		//byte[] nameStrBytesTmp = Encoding.GetEncoding("GBK").GetBytes(prgNameTmp);

		unsigned char *providerNameTmp=malloc(ptmpPrgInfo->providerNameLen);
		memcpy(providerNameTmp, ptmpPrgInfo->providerName, ptmpPrgInfo->providerNameLen);
		//    if (providerNameTmp == null)
		//   providerNameTmp = string.Empty;
		// byte[] privoderStrBytesTmp = Encoding.GetEncoding("GBK").GetBytes(providerNameTmp);

		tmpBytes[iAddr++] = (unsigned char)(1 + 1 + 1 + ptmpPrgInfo->prgNameLen + ptmpPrgInfo->providerNameLen);
		tmpBytes[iAddr++] =(unsigned char) ptmpPrgInfo->serviceType;

		tmpBytes[iAddr++] = (unsigned char)ptmpPrgInfo->providerNameLen;
		memcpy(tmpBytes+iAddr,providerNameTmp, ptmpPrgInfo->providerNameLen);

		iAddr += ptmpPrgInfo->providerNameLen;

		tmpBytes[iAddr++] = (unsigned char)ptmpPrgInfo->prgNameLen;
		memcpy(tmpBytes+iAddr,prgNameTmp, ptmpPrgInfo->prgNameLen);


		free(prgNameTmp);		
		free(providerNameTmp);
		iAddr += ptmpPrgInfo->prgNameLen;

		int j;
		Commdes_t* desInfoTmp = ptmpPrgInfo->psdtDesList;

		for (j = 0; j < ptmpPrgInfo->psdtDesListLen; j++)
		{
			if (desInfoTmp->tag == 0x48) // service_tag
			{
				desInfoTmp++;
				continue;
			}
			tmpBytes[iAddr++] = (unsigned char)(desInfoTmp->tag);
			tmpBytes[iAddr++] = (unsigned char)(desInfoTmp->dataLen);


			memcpy(tmpBytes+iAddr, desInfoTmp->data,desInfoTmp->dataLen);
			iAddr += desInfoTmp->dataLen;
			desInfoTmp++;
		}

		//printf("now the len%d\n",iAddr);
		int isCrypto;
		if(ptmpPrgInfo->isCrypto)
		{
			isCrypto=1;
		}
		else
		{
			isCrypto=0;
		}
		BigFormat_intToBytes(((4 << 13)  // runing_status
			| (isCrypto << 12) // free_CA_mode
			| ((iAddr - des_length_addr - 2) & 0xfff)), tmpBytes, des_length_addr, 2); // des_loop_length
	}

//usernode
	for (i = 0; i <list_len(&userprginfolist); i++)
	{
			list_get(&userprginfolist, i, &userptmpPrgInfo);
			iAddr += BigFormat_intToBytes(userptmpPrgInfo->prgNum, tmpBytes, iAddr, 2);
			tmpBytes[iAddr++] = (unsigned char)((0x3f << 2) // reserved_future_use
				| (0 << 1) // EIT_schedule_flag
				| (0 << 0)); // EIT_present_following_flag
			int des_length_addr = iAddr;
			iAddr += 2;

			int j;
			Commdes_t* desInfoTmp = userptmpPrgInfo->psdtDesList;

			for (j = 0; j < userptmpPrgInfo->psdtDesListLen; j++)
			{
			if (desInfoTmp->tag == 0x48) // service_tag
			{
                desInfoTmp++;
                continue;
                }
                tmpBytes[iAddr++] = (unsigned char)(desInfoTmp->tag);
                tmpBytes[iAddr++] = (unsigned char)(desInfoTmp->dataLen);
                memcpy(tmpBytes+iAddr, desInfoTmp->data,desInfoTmp->dataLen);
                iAddr += desInfoTmp->dataLen;
                desInfoTmp++;
			}

			tmpBytes[iAddr++] = 0x48;
	
			unsigned char *prgNameTmp=malloc(userptmpPrgInfo->prgNameLen);
			memcpy(prgNameTmp, userptmpPrgInfo->prgName, userptmpPrgInfo->prgNameLen);
			//	 if (ptmpPrgInfo->prgNameLen == 0)
			// prgNameTmp = string.Empty;//�����Ҫ��
			//byte[] nameStrBytesTmp = Encoding.GetEncoding("GBK").GetBytes(prgNameTmp);
	
			unsigned char *providerNameTmp=malloc(userptmpPrgInfo->providerNameLen);
			memcpy(providerNameTmp, userptmpPrgInfo->providerName, userptmpPrgInfo->providerNameLen);
			//	  if (providerNameTmp == null)
			//	 providerNameTmp = string.Empty;
			// byte[] privoderStrBytesTmp = Encoding.GetEncoding("GBK").GetBytes(providerNameTmp);

			tmpBytes[iAddr++] = (unsigned char)(1 + 1 + 1 + userptmpPrgInfo->prgNameLen + userptmpPrgInfo->providerNameLen);
			tmpBytes[iAddr++] =(unsigned char) userptmpPrgInfo->serviceType;
			tmpBytes[iAddr++] = (unsigned char)userptmpPrgInfo->providerNameLen;
			memcpy(tmpBytes+iAddr,providerNameTmp, userptmpPrgInfo->providerNameLen);
			iAddr += userptmpPrgInfo->providerNameLen;
	
			tmpBytes[iAddr++] = (unsigned char)userptmpPrgInfo->prgNameLen;
			memcpy(tmpBytes+iAddr,prgNameTmp, userptmpPrgInfo->prgNameLen);

			free(prgNameTmp);		
			free(providerNameTmp);
			iAddr += userptmpPrgInfo->prgNameLen;
	
	
			//printf("now the len%d\n",iAddr);
			int isCrypto;
			if(userptmpPrgInfo->isCrypto)
			{
				isCrypto=1;
			}
			else
			{
				isCrypto=0;
			}
			BigFormat_intToBytes(((4 << 13)  // runing_status
				| (isCrypto << 12) // free_CA_mode
				| ((iAddr - des_length_addr - 2) & 0xfff)), tmpBytes, des_length_addr, 2); // des_loop_length
		}





	int numLenTmp = ((0xf << 12) |((iAddr + 4 - sectionLenAddr - 2) & 0xfff));
	BigFormat_intToBytes(numLenTmp, tmpBytes, sectionLenAddr, 2);

    printf("=====x 6---\n");
	// -- crc --
	unsigned  long crcWord=  CrcBytes(tmpBytes,5,iAddr - 5);
	iAddr += BigFormat_uintToBytes(crcWord, tmpBytes, iAddr, 4);

	if (iAddr < 1021) // 1108
	{
		for (i = 0; i < iAddr; i++)
			sdtTable[i] = tmpBytes[i];
#if 0   //print sdt byte results				
		for(i=0; i<iAddr; i++)
			printf("  %d		  offset:%d  \n",sdtTable[i],i);

		printf("sdt table length is %d\n",iAddr);
#endif

		return iAddr;
	}
	return 0;

}


int CreateCat(Chn_ca_st *caNode, unsigned char catTable[], int verisonNumber)
{
	unsigned char tmpBytes[1024];
	int iAddr = 0;
	int i;
	if (list_len(&caNode->caIdenList)== 0)
	{
		// catTable = null;
		//return Error_psiTable.noInputData;
		return 0;
	}


	// -- head --
	tmpBytes[iAddr++] = 0x47;
	tmpBytes[iAddr++] = 0x40;
	tmpBytes[iAddr++] = 0x01;
	tmpBytes[iAddr++] = 0x10;
	tmpBytes[iAddr++] = 0;

	// -- cat --
	tmpBytes[iAddr++] = 0x01;
	int sectionLenAddr = iAddr;
	iAddr += 2; // �����ֽ�
	tmpBytes[iAddr++] = 0xff; // reserved
	tmpBytes[iAddr++] = 0xff;
	tmpBytes[iAddr++] = (unsigned char)((0x3 << 6) // reserved
		| ((verisonNumber & 0x1f) << 1)  // version number
		| (1 << 0)); // current_next_indicator
	tmpBytes[iAddr++] = 0; // section number
	tmpBytes[iAddr++] = 0; // last_section_number


	CA_descriptor *catInfoTmp;
	for (i = 0; i <list_len(&caNode->caIdenList); i++)
	{
		list_get(&caNode->caIdenList, i, &catInfoTmp);
		tmpBytes[iAddr++] = (unsigned char)catInfoTmp->tag;
		tmpBytes[iAddr++] = (unsigned char)(4 + catInfoTmp->private_data_byte_len);
		tmpBytes[iAddr++] = (unsigned char)(catInfoTmp->outCaSysId >> 8);
		tmpBytes[iAddr++] = (unsigned char)(catInfoTmp->outCaSysId);
		tmpBytes[iAddr++] = (unsigned char)(catInfoTmp->reserved << 5 | ((catInfoTmp->outCaPid >> 8) & 0x1f));
		tmpBytes[iAddr++] = (unsigned char)(catInfoTmp->outCaPid);

		if (catInfoTmp->private_data_byte_len>0)
		{
			memcpy(tmpBytes+iAddr, catInfoTmp->private_data_byte,catInfoTmp->private_data_byte_len);
			iAddr += catInfoTmp->private_data_byte_len;

		}
	}

	BigFormat_intToBytes(((0xb << 12) | ((iAddr + 4 - sectionLenAddr - 2) & 0xfff)), tmpBytes, sectionLenAddr, 2);

	// -- crc --
	unsigned  long crcWord=  CrcBytes(tmpBytes,5,iAddr - 5);
	iAddr += BigFormat_uintToBytes(crcWord, tmpBytes, iAddr, 4);


#if 0

	for(i=0; i<iAddr; i++)
		printf("  %d		  offset:%d  \n",tmpBytes[i],i);

	printf("cat table length is %d\n",iAddr);
#endif



	if (iAddr <= 188)
	{
		for (i = 0; i < iAddr; i++)
			catTable[i] = tmpBytes[i];
		for (i = iAddr; i < 188; i++)
			catTable[i] = 0xff;
		return 1;
	}
	return 0;
}



int  CreateNit(unsigned char nitTable[], Nit_section_t *nitInfo, int networkId, int verisonNumber)
{

	unsigned char tmpBytes[8 * 188];
	int iAddr = 0;
	int i,j;
	// head
	tmpBytes[iAddr++] = 0x47;
	tmpBytes[iAddr++] = 0x40;
	tmpBytes[iAddr++] = 0x10;
	tmpBytes[iAddr++] = 0x10;
	tmpBytes[iAddr++] = 0;

	// -- nit --
	tmpBytes[iAddr++] = 0x40;
	int sectionLenAddr = iAddr;
	iAddr += 2; // �����ֽ�
	iAddr += BigFormat_intToBytes(networkId, tmpBytes, iAddr, 2);
	tmpBytes[iAddr++] = (unsigned char)((3 << 6) // reserved
		| ((verisonNumber & 0x1f) << 1)  // version number
		| (1 << 0)); // current_next_indicator
	tmpBytes[iAddr++] = 0; // section number
	tmpBytes[iAddr++] = 0; // last_section_number
	int networkDesLenAddr = iAddr;
	iAddr += 2; // �����������ֽ�
	if (nitInfo->nameListLen>0)
	{
		Commdes_t *nameDesTmp=nitInfo->nameList;
		for (i = 0; i <nitInfo->nameListLen; i++)
		{
			tmpBytes[iAddr++] = (unsigned char)nameDesTmp->tag;
			if (nameDesTmp->dataLen>0)
			{
				tmpBytes[iAddr++] = (unsigned char)nameDesTmp->dataLen;
				memcpy(tmpBytes+iAddr, nameDesTmp->data,nameDesTmp->dataLen);
				iAddr += nameDesTmp->dataLen;
			}
			else
			{
				tmpBytes[iAddr++] = 0;
			}
			nameDesTmp++;
		}
	}
	int nameDesLen = iAddr - networkDesLenAddr - 2;
	tmpBytes[networkDesLenAddr] = (unsigned char)(0xf0 | (nameDesLen >> 8));
	tmpBytes[networkDesLenAddr + 1] = (unsigned char)nameDesLen;

	int tsLoopLenAddr = iAddr;
	iAddr += 2; // �����������ֽ�

	if (nitInfo->streamLoopLen>0)
	{

		Nit_streamLoop_t *tsLoopTmp=nitInfo->streamLoop;
		for (i = 0; i <nitInfo->streamLoopLen; i++)
		{
			tmpBytes[iAddr++] = (unsigned char)(tsLoopTmp->streamId >> 8);
			tmpBytes[iAddr++] = (unsigned char)(tsLoopTmp->streamId);
			tmpBytes[iAddr++] = (unsigned char)(tsLoopTmp->original_network_id >> 8);
			tmpBytes[iAddr++] = (unsigned char)(tsLoopTmp->original_network_id);
			int tsLoopDesLenAddr = iAddr;
			iAddr += 2;  
			if (tsLoopTmp->BufferUn_stLen>0)
			{
				BufferUn_st *tsDesBytesTmp=tsLoopTmp->BufferUn_stList;
				for (j = 0; j <tsLoopTmp->BufferUn_stLen; j++)
				{
					if (tsDesBytesTmp->bufLen>0)
					{
						memcpy(tmpBytes+iAddr, tsDesBytesTmp->pbuf,tsDesBytesTmp->bufLen);
						iAddr +=tsDesBytesTmp->bufLen;
					}
					tsDesBytesTmp++;
				}
			}
			int tsDesLen = iAddr - tsLoopDesLenAddr - 2;
			tmpBytes[tsLoopDesLenAddr] = (unsigned char)(0xf0 | (tsDesLen >> 8));
			tmpBytes[tsLoopDesLenAddr + 1] = (unsigned char)tsDesLen;
			tsLoopTmp++;
		}
	}
	int tsLoopLen = iAddr - tsLoopLenAddr - 2;
	tmpBytes[tsLoopLenAddr] = (unsigned char)(0xf0 | (tsLoopLen >> 8));
	tmpBytes[tsLoopLenAddr + 1] = (unsigned char)tsLoopLen;


	printf("creat nit sectionLenAddr   %d  \n",sectionLenAddr);
	printf("creat nit  addr	 %d  \n",iAddr);

	BigFormat_intToBytes(((0xb << 12) | ((iAddr + 4 - sectionLenAddr - 2) & 0xfff)), tmpBytes, sectionLenAddr, 2);
	// -- crc --
	unsigned  long crcWord=  CrcBytes(tmpBytes,5,iAddr - 5);
	iAddr += BigFormat_uintToBytes(crcWord, tmpBytes, iAddr, 4);

	if (iAddr < 1021) // 1108
	{
		for (i = 0; i < iAddr; i++)
			nitTable[i] = tmpBytes[i];
#if 0
		for(i=0; i<iAddr; i++)
			printf("  %d		  offset:%d  \n",nitTable[i],i);

		printf("nit table length is %d\n",iAddr);
#endif

		return iAddr;
	}
	return 0;



}

int  ParsePmt(unsigned char buf[], int offset, pmt_senction_st *p_pmt)
{
	int iAddr = offset;
	int tmpByteValue;
	int indexL1 = 1;

	p_pmt->table_id = buf[iAddr++];
	tmpByteValue = buf[iAddr++];
	p_pmt->section_syntax_indicator = (tmpByteValue >> 7) & 0x1;
	p_pmt->flag_0 = (tmpByteValue >> 6) & 0x1;
	p_pmt->reserved0 = (tmpByteValue >> 4) & 0x3;
	p_pmt->section_length = ((tmpByteValue & 0xf) << 8) | buf[iAddr++];

	if (p_pmt->section_length < 9 || p_pmt->section_length > 1101)
		return 0;
	if (CrcBytes(buf, offset, 3 + p_pmt->section_length) != 0)
		return 0;


	p_pmt->program_number = (buf[iAddr] << 8) | buf[++iAddr];
	iAddr++;
	tmpByteValue = buf[iAddr++];
	p_pmt->reserved1 = (tmpByteValue >> 6) & 0x3;
	p_pmt->version_number = (tmpByteValue >> 1) & 0x1f;
	p_pmt->current_next_indicator = (tmpByteValue >> 0) & 0x1;
	p_pmt->section_number = buf[iAddr++];
	p_pmt->last_section_number = buf[iAddr++];
	tmpByteValue = buf[iAddr++];
	p_pmt->reserved2 = (tmpByteValue >> 5) & 0x7;
	p_pmt->pcrPid = ((tmpByteValue << 8) & 0x1f00) | buf[iAddr++]; ;
	tmpByteValue = buf[iAddr++];
	p_pmt->reserved3 = (tmpByteValue >> 4) & 0xf;
	p_pmt->program_info_length = ((tmpByteValue << 8) & 0x0f00) | buf[iAddr++];



	int pStart = iAddr;
	int pEnd = iAddr + p_pmt->program_info_length;


	int tmpLen=0;
	int tmppStart=pStart;
	while (tmppStart + 2 <= pEnd)
	{
		tmppStart++;
		int tmppmtDesLen =buf[tmppStart++];
		tmppStart += tmppmtDesLen;
		tmpLen++;
	}


	p_pmt->pmtDesListLen=tmpLen;
	p_pmt->pmtDesList=(Commdes_t*)malloc(sizeof(Commdes_t)*(p_pmt->pmtDesListLen));
	Commdes_t* p_Commdes_st= p_pmt->pmtDesList;

	while (pStart + 2 <= pEnd)
	{
		p_Commdes_st->tag = buf[pStart++];
		int pmtDesLen = buf[pStart++];
		if (pmtDesLen <= pEnd - pStart)
		{
			p_Commdes_st->index = indexL1++;
			p_Commdes_st->data = malloc(pmtDesLen);
			memcpy(p_Commdes_st->data, buf+pStart, pmtDesLen);
			p_Commdes_st->dataLen=pmtDesLen;
			p_Commdes_st++;
		}

		pStart += pmtDesLen;

	}
	iAddr += p_pmt->program_info_length;
	pStart = pEnd;
	pEnd = pStart + (p_pmt->section_length - 9 - p_pmt->program_info_length - 4);



	tmppStart=pStart;

	tmpLen=0;
	unsigned char cLen[188];
	while (tmppStart + 5 <= pEnd)
	{
		cLen[tmpLen]=0;
		tmppStart++;
		tmppStart++;	tmppStart++;
		int tmpes_info_lengh = ((buf[tmppStart] << 8) & 0x0f00) | buf[++tmppStart];
		tmppStart++;

		if (tmpes_info_lengh + tmppStart <= pEnd)
		{
			if (tmpes_info_lengh > 0)
			{
				int tmpppStart = tmppStart;
				int tmpppEnd = tmpppStart + tmpes_info_lengh;

				while (tmpppStart + 2 < tmpppEnd)
				{
					tmpppStart++;
					int tmpdsDesLen =buf[tmpppStart++];
					tmpppStart += tmpdsDesLen;
					cLen[tmpLen]=cLen[tmpLen]+1;
				}
			}
		}
		tmppStart+=tmpes_info_lengh;
		tmpLen++;
	}


	p_pmt->pdataStreamListLen=tmpLen;
	p_pmt->pdataStreamList=(DataStream_t*)malloc(sizeof(DataStream_t)*tmpLen);
	DataStream_t* p_DataStream_st= p_pmt->pdataStreamList;

	int q=0;
	while (pStart + 5 <= pEnd)
	{


		p_DataStream_st->streamType = buf[pStart++];
		p_DataStream_st->outPid = ((buf[pStart] << 8) & 0x1f00) | buf[++pStart];
		pStart++;
		int es_info_lengh = ((buf[pStart] << 8) & 0x0f00) | buf[++pStart];
		pStart++;
		if (es_info_lengh + pStart <= pEnd)
		{


			if (es_info_lengh > 0)
			{

				int ppStart = pStart;
				int ppEnd = ppStart + es_info_lengh;
				int indexL2 = 1;
				//printf("tmpdatastremcomdstLen            :%d\n ",cLen[q]);


				p_DataStream_st->desNodeLen=cLen[q];
				p_DataStream_st->desNode=(Commdes_t*)malloc(sizeof(Commdes_t)*cLen[q]);
				Commdes_t* p_Commdes_st= p_DataStream_st->desNode;
				while (ppStart + 2 < ppEnd)
				{

					p_Commdes_st->tag = buf[ppStart++];

					int dsDesLen = buf[ppStart++];
					if (dsDesLen <= ppEnd - ppStart)
					{

						p_Commdes_st->index = indexL2++;
						p_Commdes_st->data = malloc(dsDesLen);
						memcpy(p_Commdes_st->data, buf+ppStart, dsDesLen);
						p_Commdes_st->dataLen=dsDesLen;
						p_Commdes_st++;

					}
					ppStart += dsDesLen;

				}
			}
			p_DataStream_st->index = indexL1++;
			p_DataStream_st++;
			q++;
		}
		pStart += es_info_lengh;
	}
	iAddr = pEnd;
	p_pmt->crc32 = (buf[iAddr] << 24) | (buf[++iAddr] << 16) | (buf[++iAddr] << 8) | buf[++iAddr];
	iAddr++;

	return 1;

}

int ParsePat(unsigned char buf[], int offset, pat_senction_st *p_pat)
{
	int i;
	int iAddr = offset;
	int tmpByteValue;
	//p_pat = (pat_senction_st*)malloc(sizeof(pat_senction_st));
	p_pat->table_id = buf[iAddr++];
	tmpByteValue = buf[iAddr++];


	//�����ĸ�ռһ���ֽ�
	p_pat->section_syntax_indicator = (tmpByteValue >> 7) & 0x1;
	p_pat->flag_0 = (tmpByteValue >> 6) & 0x1;
	p_pat->reserved0 = (tmpByteValue >> 4) & 0x3;
	p_pat->section_length = ((tmpByteValue & 0xf) << 8) | buf[iAddr++];



	if (p_pat->section_length < 9 || p_pat->section_length > 1101)
		return 0;
	if (CrcBytes(buf, offset, 3 + p_pat->section_length) != 0)
		return 0;

	int contentStartAddr = iAddr;
	p_pat->transport_stream_id = (buf[iAddr] << 8) | buf[++iAddr];
	iAddr++;


	tmpByteValue = buf[iAddr++];
	p_pat->reserved1 = (tmpByteValue >> 6) & 0x3;
	p_pat->version_number = (tmpByteValue >> 1) & 0x1f;
	p_pat->current_next_indicator = (tmpByteValue >> 0) & 0x1;
	p_pat->section_number = buf[iAddr++];
	p_pat->last_section_number = buf[iAddr++];

	int prgListLen = p_pat->section_length + contentStartAddr - iAddr - 4;

	if (prgListLen % 4 != 0)
		return 0;

	p_pat->patPrg_tList=(patPrg_t*)malloc(sizeof(patPrg_t)*(prgListLen / 4));
	p_pat->patPrg_tListLen=prgListLen / 4;
	patPrg_t* p_first_patPrg_t=p_pat->patPrg_tList;

	for (i = 0; i < prgListLen / 4; i++){

		p_first_patPrg_t->program_number = (buf[iAddr] << 8) | buf[++iAddr];
		iAddr++;

		tmpByteValue = buf[iAddr++];
		p_first_patPrg_t->reserved = (tmpByteValue >> 5) & 0x7;
		p_first_patPrg_t->pid = ((tmpByteValue & 0x1f) << 8) | buf[iAddr++];

		//printf("prgnumber:%d ",p_patPrg_t->program_number);

		p_first_patPrg_t++;



	}


	p_pat->crc32 = (buf[iAddr] << 24) | (buf[++iAddr] << 16) | (buf[++iAddr] << 8) | buf[++iAddr];
	iAddr++;

	return 1;
}

int ParseSdt(unsigned char buf[], int offset, sdt_senction_st *sdtSec)
{
	int i;
	int iAddr = offset;
	int tmpByteValue;

	sdtSec->table_id = buf[iAddr++];
	tmpByteValue = buf[iAddr++];
	sdtSec->section_syntax_indicator = (tmpByteValue >> 7) & 0x1;
	sdtSec->reserved_future_use1 = (tmpByteValue >> 6) & 0x1;
	sdtSec->reserved0 = (tmpByteValue >> 4) & 0x3;
	sdtSec->section_length = ((tmpByteValue & 0xf) << 8) | buf[iAddr++];

	if (sdtSec->section_length < 9 || sdtSec->section_length > 1101)
		return 0;
	if (CrcBytes(buf, offset, 3 + sdtSec->section_length) != 0)
		return 0;

	sdtSec->transport_stream_id = (buf[iAddr] << 8) | buf[++iAddr];
	iAddr++;
	tmpByteValue = buf[iAddr++];
	sdtSec->reserved1 = (tmpByteValue >> 6) & 0x3;
	sdtSec->version_number = (tmpByteValue >> 1) & 0x1f;
	sdtSec->current_next_indicator = (tmpByteValue >> 0) & 0x1;
	sdtSec->section_number = buf[iAddr++];
	sdtSec->last_section_number = buf[iAddr++];
	sdtSec->original_network_id = ((buf[iAddr] << 8) & 0xff00) | buf[++iAddr];
	iAddr++;
	sdtSec->reserved_future_use2 = buf[iAddr++];

	int pStart = iAddr;
	int pEnd = pStart + sdtSec->section_length - 12;




	int tmppStart=pStart;

	int tmpLen=0;
	unsigned char cLen[188];
	while (tmppStart + 5 <= pEnd)
	{
		cLen[tmpLen]=0;
		tmppStart++;
		tmppStart++;	tmppStart++;
		tmpByteValue =buf[tmppStart++];
		int tmpes_info_lengh = ((tmpByteValue << 8) & 0x0f00) | buf[tmppStart++];



		int tmpppStart = tmppStart;
		int tmpppEnd = tmpppStart + tmpes_info_lengh;
		//printf("tmpppStart:%d\n ",tmpppStart);
		while (tmpppStart + 2 < tmpppEnd)
		{
			tmpppStart++;
			int tmpdsDesLen =buf[tmpppStart++];
			tmpppStart += tmpdsDesLen;
			tmppStart+=2 + tmpdsDesLen;
			cLen[tmpLen]=cLen[tmpLen]+1;
		}


		//tmppStart+=tmpes_info_lengh;
		tmpLen++;
	}




	sdtSec->nameListLen =tmpLen;
	sdtSec->nameList=(sdtPrgName_st*)malloc(sizeof(sdtPrgName_st)*tmpLen);
	sdtPrgName_st *nameNode= sdtSec->nameList;
	//printf("nameNode->nameListLen11111 %d \n ",sdtSec->nameListLen );


	int q=0;



	while (pStart + 5 <= pEnd)
	{
		//printf("1 time name list\n ");

		nameNode->service_id = ((buf[pStart] << 8) & 0xff00) | buf[++pStart];
		pStart++;
		tmpByteValue = buf[pStart++];
		nameNode->reserved_future_use = (tmpByteValue >> 2) & 0x3f;
		nameNode->EIT_schedule_flag = (tmpByteValue >> 1) & 0x01;
		nameNode->EIT_present_following_flag = (tmpByteValue >> 0) & 0x01;
		tmpByteValue = buf[pStart++];
		nameNode->runing_status = (tmpByteValue >> 5) & 0x07;
		nameNode->free_CA_mode = (tmpByteValue >> 4) & 0x01;
		nameNode->descriptors_loop_length = ((tmpByteValue << 8) & 0x0f00) | buf[pStart++];

		int ppStart = pStart;
		int ppEnd = ppStart + nameNode->descriptors_loop_length;
		int indexDes = 1;
		if (nameNode->descriptors_loop_length >= 2)
		{
			//nameNode.desList = new ArrayList();
		}


		int k;
		nameNode->desListLen=cLen[q];
		nameNode->desList=(Commdes_t*)malloc(sizeof(Commdes_t)*cLen[q]);
		Commdes_t* desNode= nameNode->desList;

		while (ppStart + 2 <= ppEnd)
		{

			desNode->index = indexDes++;
			desNode->tag = buf[ppStart++];
			int desNodeLen = buf[ppStart++];

			if (ppStart + desNodeLen <= ppEnd)
			{

				desNode->data  = malloc(desNodeLen);
				memcpy(desNode->data, buf+ppStart, desNodeLen);
				desNode->dataLen=desNodeLen;

				/*
				printf("go to print\n %d  ");
				unsigned char *ptmp=desNode->data;

				for (k = 0; k < desNode->dataLen; k++)
				{

				printf("gunandrose %d  ",*ptmp++);

				}

				*/
			}
			ppStart += desNodeLen;
			pStart += 2 + desNodeLen;
			desNode++;//for desnode always ++;desNode->data   is possiblely be NULL

		}

		iAddr += 5 + nameNode->descriptors_loop_length;
		nameNode++;
		q++;
	}


	sdtSec->crc32 = (buf[iAddr] << 24) | (buf[++iAddr] << 16) | (buf[++iAddr] << 8) | buf[++iAddr];
	iAddr++;

	return 1;
}




int  ParseCat(unsigned char buf[], int offset, cat_senction_st *catSec)
{
	int iAddr = offset;
	int tmpByteValue;
	catSec->table_id = buf[iAddr++];
	tmpByteValue = buf[iAddr++];
	catSec->section_syntax_indicator = (tmpByteValue >> 7) & 0x1;
	catSec->flag_0 = (tmpByteValue >> 6) & 0x1;
	catSec->reserved0 = (tmpByteValue >> 4) & 0x3;
	catSec->section_length = ((tmpByteValue & 0xf) << 8) | buf[iAddr++];

	if (catSec->section_length < 9 || catSec->section_length > 1101)
		return 0;
	if (CrcBytes(buf, offset, 3 + catSec->section_length) != 0)
		return 0;

	int contentStartAddr = iAddr;
	catSec->reserved1 = (buf[iAddr++] << 8) | buf[iAddr++];
	tmpByteValue = buf[iAddr++];
	catSec->reserved2 = (tmpByteValue >> 6) & 0x3;
	catSec->version_number = (tmpByteValue >> 1) & 0x1f;
	catSec->current_next_indicator = (tmpByteValue >> 0) & 0x1;
	catSec->section_number = buf[iAddr++];
	catSec->last_section_number = buf[iAddr++];

	int catListLen = catSec->section_length + contentStartAddr - iAddr - 4;




	int pStart = iAddr;
	int pEnd = pStart + catSec->section_length - 9;



	int tmppStart=pStart;
	int tmpDescriptor_length;
	int tmpLen=0;

	while (pStart + 6 <= pEnd)
	{
		tmppStart++;
		tmpDescriptor_length = buf[tmppStart++];
		tmppStart++;	tmppStart++;	tmppStart++;	tmppStart++;
		tmppStart +=tmpDescriptor_length;
		tmpLen++;
	}



	catSec->caIdenListLen=tmpLen;
	catSec->caIdenList=(CA_descriptor*)malloc(sizeof(CA_descriptor)*tmpLen);
	CA_descriptor *caDesTmp= catSec->caIdenList;


	while (pStart + 6 <= pEnd)
	{
		caDesTmp->tag = buf[pStart++];
		caDesTmp->descriptor_length = buf[pStart++]; // desLen;
		caDesTmp->inCaSysId = caDesTmp->outCaSysId = (buf[pStart] << 8) | buf[++pStart];
		pStart++;
		tmpByteValue = buf[pStart++];
		caDesTmp->reserved = (tmpByteValue >> 5) & 0x07;
		caDesTmp->inCaPid = caDesTmp->outCaPid = ((tmpByteValue << 8) & 0x1f00) | buf[pStart++];
		int private_data_byte_length = caDesTmp->descriptor_length - 4;

		caDesTmp->private_data_byte  = malloc(private_data_byte_length);
		memcpy(caDesTmp->private_data_byte, buf+iAddr, private_data_byte_length);
		pStart += private_data_byte_length;
		caDesTmp++;

	}
	iAddr = pEnd;
	catSec->crc32 = (buf[iAddr] << 24) | (buf[++iAddr] << 16) | (buf[++iAddr] << 8) | buf[++iAddr];
	iAddr++;

	return 1;
}

int  ParseNit(unsigned char buf[], int offset, Nit_section_t *nitSec)
{
	int i;
	int iAddr = offset;
	if (buf[iAddr++] != 0x40)//
		return 0;
	int secLen = ((buf[iAddr] & 0x0f) << 8) | buf[iAddr + 1];
	iAddr += 2;
	nitSec->networkId = BigFormat_fromBytes(iAddr, 2, buf); iAddr += 2;
	nitSec->version = (buf[iAddr++] >> 1) & 0x1f;
	int secNum = buf[iAddr++];
	int lastSecNum = buf[iAddr++];
	int nameDesLen = ((buf[iAddr] & 0x0f) << 8) | buf[iAddr + 1];
	iAddr += 2;


	int tmpLen=0;
	int tmpiAddr=iAddr;
	for (i = 0; i < nameDesLen; )
	{
		tmpiAddr++;
		int desDataLen = buf[tmpiAddr++];
		tmpiAddr += desDataLen;
		i += 2 + desDataLen;

		tmpLen++;			 
	}
	nitSec->nameListLen=tmpLen;
	nitSec->nameList=(Commdes_t*)malloc(sizeof(Commdes_t)*tmpLen);
	Commdes_t *nameDesTmp= nitSec->nameList;

	for (i = 0; i < nameDesLen; )
	{
		nameDesTmp->tag = buf[iAddr++];
		int desDataLen = buf[iAddr++];

		nameDesTmp->data= malloc(desDataLen);
		memcpy(nameDesTmp->data, buf+iAddr, desDataLen);
		nameDesTmp->dataLen=desDataLen;

		iAddr += desDataLen;
		nameDesTmp++;
		i += 2 + desDataLen;
	}

	int tsLoopLen = ((buf[iAddr] & 0x0f) << 8) | buf[iAddr + 1];
	iAddr += 2;

	printf("tsLoopLen ***************** ----%d\n",tsLoopLen);

	//reset  count
	tmpLen=0;
	tmpiAddr=iAddr;
	for (i = 0; i < tsLoopLen; )
	{
		tmpiAddr+=2;
		tmpiAddr+=2;
		int tsDesLen = ((buf[tmpiAddr] & 0x0f) << 8) | buf[tmpiAddr + 1];
		tmpiAddr+=2;

		tmpiAddr += tsDesLen;
		i += 6 + tsDesLen;

		tmpLen++;			 
	}

	printf("tmpLen ***************** ----%d\n",tmpLen);

	nitSec->streamLoopLen=tmpLen;
	nitSec->streamLoop=(Nit_streamLoop_t*)malloc(sizeof(Nit_streamLoop_t)*tmpLen);
	Nit_streamLoop_t *tsLoopTmp= nitSec->streamLoop;

	for (i = 0; i < tsLoopLen; )
	{
		tsLoopTmp->BufferUn_stList = (BufferUn_st*)malloc(sizeof(BufferUn_st));
		tsLoopTmp->BufferUn_stLen=1;
		BufferUn_st *desBytes=tsLoopTmp->BufferUn_stList;
		tsLoopTmp->streamId = BigFormat_fromBytes(iAddr, 2, buf);
		iAddr += 2;
		tsLoopTmp->original_network_id = BigFormat_fromBytes(iAddr, 2, buf);
		iAddr += 2;
		int tsDesLen = ((buf[iAddr] & 0x0f) << 8) | buf[iAddr + 1];
		iAddr += 2;

		desBytes->pbuf= malloc(tsDesLen);
		memcpy(desBytes->pbuf, buf+iAddr, tsDesLen);
		desBytes->bufLen=tsDesLen;

		iAddr += tsDesLen;
		desBytes++;//no mean ,always 1 time
		tsLoopTmp++;
		i += 6 + tsDesLen;
	}
	return 1;
}

