#ifndef _GET_PROGRAMS_H_
#define _GET_PROGRAMS_H_

#include "datastructdef.h"
#include "list.h"

int code_convert(char *from_charset,char *to_charset,char *inbuf,int inlen,char *outbuf,int outlen);
int u2g(char *inbuf,int inlen,char *outbuf,int outlen);
int g2u(char *inbuf,size_t inlen,char *outbuf,size_t outlen)
void getPrgsJson(char *ip, int inChn, char *outprg);
void adduserprgjson(cJSON *basearry, ChannelProgramSt *pst);
void getoutprgsJson(char *ip, int inChn, char *outprg);
#endif 
