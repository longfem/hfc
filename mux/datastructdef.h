
#include "list.h"

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


//Dev_prgInfo_st
typedef struct Dev_prgInfo_st
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

} Dev_prgInfo_st;





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

typedef struct Database_st
{
	unsigned char isAutoMakeMux;
	unsigned char isAutoOutTreeListExpandAll;
	unsigned char isAutoInTreeListExpandAll;
	unsigned char isPrgInfoHexPid;
	unsigned char isConnectPid;
	unsigned char isLogEnable;
	DatabaseOutputChannel_st *poutChnArray;

	//string net_ipAddrStr;
	//int net_port;
	//ConnectType_en net_type;
	int startPid;
	//bool isServerMode;

}Database_st;

typedef struct DatabaseOutputChannel_st
{
	unsigned int outputRate;
	unsigned int oringal_networkid;
	unsigned int networkId;
	unsigned int streamId;
	unsigned char isAutoRaiseVersion;
	unsigned int version;
	unsigned char isManualMapMode;
	unsigned char isAutoRankPAT;

	unsigned char isNeedSend_pat;
	unsigned char isNeedSend_pmt;
	unsigned char isNeedSend_sdt;
	unsigned char isNeedSend_cat;
	unsigned char isNeedSend_nit;
}DatabaseOutputChannel_st;



#define cdr_int16_to(from) \
	( (((from) & (0xffu << 8)) >> 8)  \
	| (((from) & 0xffu) << 8) )
		
#endif		

typedef struct MuxPrgInfoGet_st
{
	int inChannel;
	int prgIndex; // 该消息指示的节目列表中消息的索引
	int prgNum;
	int prgPid;
}MuxPrgInfoGet_st;

typedef struct ChannelProgramSt
{
	int channelId;
	list_t prgNodes; // struct Dev_prgInfo_st
	//Chn_ca_st caNode;
	list_t userPrgNodes; // struct User_prgInfo_t
	list_t dtPidList; // MuxPidInfo_st , 透传表, 数组以输出通道为序
}ChannelProgramSt;

typedef struct  ClsProgram_t
{
	int _intChannelCntMax;
	int _outChannelCntMax;
	int _pmtMaxCnt = 29;
	list_t  inPrgList;  //ChannelProgramSt  8 input
	list_t outPrgList;  // 2 output
	// int[] chnBypass2; // 通道直通, 0=禁用，other=输出直通某个输出
	// bool[] chnBypassEnable; // 通道直通功能激活
	// List<ScramblePrgSt> scramblePrgList;
	// List<ScramblePrgSt_v2> scramblePrgListV2;
	// bool[,] needInputData; // 标志需要用到输入通道的数据，当数据丢失时报警
	 int prgNum_min = 1;
	 int prgPid_min = 0x100;
	 int prgPid_max = 0xfff;
	 int subPrgPid_min = 0x1000;
	 int subPrgPid_max = 0x1ff0;
	// int[] m_autoMuxStartPid; // 自动映射起始PID
	// ClsMux muxer = null;
	// ArrayList[] PrgPmtMuxList = null; // MuxPrgInfoGet_st
	// public ArrayList[] PrgAVMuxList = null;  // MuxPidInfo_st , 节目映射PID, 数组以输出通道为序
	// ArrayList[] nitSection = null; // Nit_section_st
	// Dglt_showPidMap dglt_showPidMap = null;
}ClsProgram_t;	

