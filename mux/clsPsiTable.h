




#ifndef _CLS_PSI_TABLE_H_
#define _CLS_PSI_TABLE_H_

unsigned char Muxer_ChannelScan(void);
Dev_prgInfo_st* buildOoutPrgList();
unsigned  long  CrcBytes(unsigned char *inBytes,int offset, int length);
static int BigFormat_intToBytes(int intValue, unsigned char outBytes[], int offset, int bytesLen);
static int BigFormat_uintToBytes(unsigned long intValue, unsigned char outBytes[], int offset, int bytesLen);
pat_senction_st*  CreatePat(list_t  prginfolist,unsigned char tmpBytes[],int streamId, int networkId, int verisonNumber);
pat_senction_st*  CreatePatNoju(Dev_prgInfo_st* outPrgList,unsigned char tmpBytes[],int streamId, int networkId, int verisonNumber);
pmt_senction_st * CreatePmt(Dev_prgInfo_st *outPrgInfo, unsigned char tmpBytes[], int verisonNumber);
pmt_senction_st * CreatePmtNoju(Dev_prgInfo_st *outPrgInfo, unsigned char tmpBytes[], int verisonNumber);

#endif
