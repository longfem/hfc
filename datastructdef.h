#ifndef _DATASTRUCT_DEFINE_H_
#define _DATASTRUCT_DEFINE_H_


typedef struct Commdes_st
{
	int userNew; // 用户自定义内容
	int index;
	int tag;
	int dataLen;	
	unsigned char *data;
}Commdes_t;

typedef struct DataStream_st
{
	int userNew; // 用户自定义内容
	int index;
	int inChn;
	int streamType;
	int inPid;
	int outPid;
	int desNodeLen;
	int destlen;
	Commdes_t *desNode; // Commdes_st
}DataStream_t;

typedef struct program_info_s
{
    int userNew; // 用户自定义内容
    int index;
    int prgNum;		//2字节
    int chnId;		//1字节
    int streamId;	//2字节
    int networkId;	//2字节
    int pmtPid;		//2字节
    int oldPcrPid;		//2字节
    int newPcrPid;		//2字节
	//7.	PMT表描述个数 //1字节
	//For(i=0;i<n;i++)
	//{
	//    //描述标志  1字节
	////描述长度  1字节
	////描述内容
	//}
	int pmtDesListLen;
	Commdes_t *pmtDesList; // Commdes_st

	//8.	数据流个数 //1字节
	//For(i=0;i<n1;i++)
	//{
	//    //数据流类型  1字节
	////数据流PID  2字节
	//描述子个数 //1字节
	//For(i=0;i<n;i++)
	//{
	//    //描述标志  1字节
	////描述长度  1字节
	////描述内容
	//}
	//}
	int pdataStreamListLen;
	DataStream_t *pdataStreamList; // DataStream_st

	//9.	节目名字长度//1字节
	//10.	具体的节目名字内容
	int  prgNameLen;
	unsigned char *prgName;
	//11.	服务提供商名字长度
	//12.	服务提供商名字
	int providerNameLen;
	char *providerName;
	int serviceType;
	//13.	CA标志
	int isCrypto;
	//14.	SDT表描述子个数
	//For(i=0;i<n;i++)
	//{
	//    //描述标志  1字节
	////描述长度  1字节
	////描述内容
	//}
	int psdtDesListLen;
	Commdes_t *psdtDesList; // Commdes_st

} program_info_t;


// typedef struct ChannelProgramSt
// {
// 	int channelId;
// 	program_info_t *prgNodes; // struct Dev_prgInfo_st
// 	Chn_ca_st *caNode;
// 	User_prgInfo_t *userPrgNodes; // struct User_prgInfo_t
// 	MuxPidInfo_st *dtPidList; // MuxPidInfo_st , 透传表, 数组以输出通道为序
// }ChannelProgramSt_t



typedef struct __ts_table_pmt_es_t
{
	unsigned char		descriptor_flag;
	unsigned char	    descriptor_len;
	unsigned char       *descriptor;
	//struct __ts_table_pmt_es_t*		next;

}ts_table_pmt_es_t;


typedef struct out_program_num_s
{
	unsigned char inChn;       //1 byte
	unsigned char prgIndex;    //1 byte
	unsigned short newPrgId;   //2 byte
	unsigned short newPid;	   //2 byte
} out_program_num_t;


#define cdr_int16_to(from) \
	( (((from) & (0xffu << 8)) >> 8)  \
	| (((from) & 0xffu) << 8) )


/*
		public int prgNum;		//2字节
		public int chnId;		//1字节
		public int streamId;	//2字节
		public int networkId;	//2字节
		public int pmtPid;		//2字节
		public int oldPcrPid;		//2字节
		public int newPcrPid;		//2字节
*/
		
#endif		