#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include "esp.h"
#include "devinfo.h"
#include "cJSON.h"
#include "datastructdef.h"
#include "clsMuxOutCh.h"
#include "clsMuxprgInfoGet.h"
#include "clsMux.h"
#include "list.h"

extern ClsProgram_st clsProgram;

static void rendersts(const char *str,int status)
{
	cJSON *result = cJSON_CreateObject();
	char* jsonstring;
	cJSON_AddNumberToObject(result,"sts", status);
	jsonstring = cJSON_PrintUnformatted(result);
	memcpy(str, jsonstring, strlen(jsonstring));
	//释放内存
	cJSON_Delete(result);
	free(jsonstring);
}

int nitstream(Nit_section_t *nist, int flag, int i, unsigned char *desBytes, MprJson *jsonparam){
    int j = 0;
    if(flag){
        //add
        nist->streamLoopLen++;
        Nit_streamLoop_t *newstreamLoop = malloc(nist->streamLoopLen * sizeof(Nit_streamLoop_t));
        Nit_streamLoop_t *newstreamLoopList = newstreamLoop;
        Nit_streamLoop_t *streamLoop = nist->streamLoop;
        //copy old stream to new
        for(j=0;j<nist->streamLoopLen - 1;j++){
            memcpy(newstreamLoop, streamLoop, sizeof(Nit_streamLoop_t));
            newstreamLoop->desList = malloc(sizeof(BufferUn_st));
            memcpy(newstreamLoop->desList, streamLoop->desList, sizeof(BufferUn_st));
            newstreamLoop->desList->pbuf = malloc(newstreamLoop->desList->bufLen * sizeof(unsigned char));
            memcpy(newstreamLoop->desList->pbuf, streamLoop->desList->pbuf, streamLoop->desList->bufLen);
            newstreamLoop++;
            streamLoop++;
        }
        newstreamLoop->BufferUn_stLen = 1;
        newstreamLoop->streamId = atoi(mprGetJson(jsonparam, "streamid"));
        newstreamLoop->original_network_id = atoi(mprGetJson(jsonparam, "netid"));
        newstreamLoop->desList = malloc(sizeof(BufferUn_st));
        newstreamLoop->desList->pbuf = malloc(i * sizeof(unsigned char));
        newstreamLoop->desList->bufLen = i;
        memcpy(newstreamLoop->desList->pbuf, desBytes, i);
        //释放过时的stream内存
        streamLoop = nist->streamLoop;
        if(nist->streamLoopLen - 1 > 0){
            for(j=0;j<nist->streamLoopLen - 1;j++){
                if(streamLoop->BufferUn_stLen > 0){
                    if(streamLoop->desList->bufLen > 0){
                        free(streamLoop->desList->pbuf);
                        streamLoop->desList->pbuf = NULL;
                    }
                    free(streamLoop->desList);
                    streamLoop->desList = NULL;
                }
                streamLoop++;
            }
            free(nist->streamLoop);
            streamLoop = NULL;
        }
        //指向新内存地址
        nist->streamLoop = newstreamLoopList;
    }else{
        //edit
        Nit_streamLoop_t *streamLoop = nist->streamLoop;
        int originalstreamid = atoi(mprGetJson(jsonparam, "oristreamid"));
        for(j=0;j<nist->streamLoopLen;j++){
            if(streamLoop->streamId == originalstreamid){
                //释放过时的流
                free(streamLoop->desList->pbuf);
                free(streamLoop->desList);

                streamLoop->BufferUn_stLen = 1;
                streamLoop->streamId = atoi(mprGetJson(jsonparam, "streamid"));
                streamLoop->original_network_id = atoi(mprGetJson(jsonparam, "netid"));
                streamLoop->desList = malloc(sizeof(BufferUn_st));
                streamLoop->desList->pbuf = malloc(i * sizeof(unsigned char));
                streamLoop->desList->bufLen = i;
                memcpy(streamLoop->desList->pbuf, desBytes, i);
                break;
            }
            streamLoop++;
        }
    }
}

int Perform_S(MprJson *jsonparam, Nit_section_t *nist, int flag){
    int i = 0, j = 0, val = 0;
    char idstr[16] = {0};
    cchar *tmpstr;
    int cnt = atoi(mprGetJson(jsonparam, "cnt"));
    int hz = atoi(mprGetJson(jsonparam, "hz"));
    int ksm = atoi(mprGetJson(jsonparam, "ksm"));
    int path = atoi(mprGetJson(jsonparam, "path"));
    unsigned int u32int;
    unsigned char desBytes[13 + cnt * 3 + 2];
    desBytes[i++] = 0x43;
    desBytes[i++] = 11;
    // frequency
    if(hz == 0){
        u32int = 0;
    }else{
        if(hz<0 || hz> 0x99999999)
            return 0;
        u32int = hz;
    }
    desBytes[i++] = (unsigned char)(u32int >> 24);
    desBytes[i++] = (unsigned char)(u32int >> 16);
    desBytes[i++] = (unsigned char)(u32int >> 8);
    desBytes[i++] = (unsigned char)(u32int);
    // orbital_position
    if(path == 0){
        u32int = 0;
    }else{
        if(path<0 || path> 0x9999)
            return 0;
        u32int = path;
    }
    desBytes[i++] = (unsigned char)(u32int >> 8);
    desBytes[i++] = (unsigned char)(u32int);
    //west_east_flag polarization modulation
    desBytes[i++] = (unsigned char)(atoi(mprGetJson(jsonparam, "tip")) << 7) |
        (atoi(mprGetJson(jsonparam, "pol")) << 5) | atoi(mprGetJson(jsonparam, "qpsk"));
    // symbol rate and fec inner
    if(ksm == 0){
        u32int = 0;
    }else{
        if(ksm<0 || ksm> 0x99999999)
            return 0;
        u32int = ksm;
    }
    u32int *= 0x10; // K步进补齐省略位
    u32int <<= 4;
    u32int += (unsigned int)(atoi(mprGetJson(jsonparam, "fecin")) & 0x0f);  // fec inner
    desBytes[i++] = (unsigned char)(u32int >> 24);
    desBytes[i++] = (unsigned char)(u32int >> 16);
    desBytes[i++] = (unsigned char)(u32int >> 8);
    desBytes[i++] = (unsigned char)(u32int);
    if (cnt > 0)
    {
        desBytes[i++] = 0x41;
        desBytes[i++] = (unsigned char)(cnt * 3);
        for(j=0;j<cnt;j++){
            memset(idstr, 0, sizeof(idstr));
            sprintf(idstr, "nits_id%d", j);
            tmpstr = mprGetJson(jsonparam, idstr);
            sscanf(tmpstr, "%d", &val);
            desBytes[i++] = (unsigned char)(val >> 8);
            desBytes[i++] = (unsigned char)val;
            memset(idstr, 0, sizeof(idstr));
            sprintf(idstr, "nits_type%d", j);
            tmpstr = mprGetJson(jsonparam, idstr);
            sscanf(tmpstr, "%d", &val);
            desBytes[i++] = (unsigned char)val;
        }
    }
    printf("===desbytes===\n");
    for(j = 0;j<i;j++){
        printf("[%d]%02x\n", j, desBytes[j]);
    }
    nitstream(nist, flag, i, desBytes, jsonparam);
    return 1;
}

int Perform(MprJson *jsonparam, Nit_section_t *nist, int flag)
{
    int i = 0, j = 0, val = 0;
    char idstr[16] = {0};
    cchar *tmpstr;
    int cnt = atoi(mprGetJson(jsonparam, "cnt"));
    int hz = atoi(mprGetJson(jsonparam, "hz"));
    int ksm = atoi(mprGetJson(jsonparam, "ksm"));
    int fecout = atoi(mprGetJson(jsonparam, "fecout"));
    unsigned char desBytes[13 + cnt * 3 + 2];
    unsigned int u32int, u32Tmp;
    desBytes[i++] = 0x44;
    desBytes[i++] = 11;
    if(hz == 0){
        u32int = 0;
    }else{
        if(hz<0 || hz> 0x99999999)
            return 0;
        u32int = hz;
    }
    u32int *= 0x10; // kHz步进省去的1位
    desBytes[i++] = (unsigned char)(u32int >> 24);
    desBytes[i++] = (unsigned char)(u32int >> 16);
    desBytes[i++] = (unsigned char)(u32int >> 8);
    desBytes[i++] = (unsigned char)(u32int);
    // fec outer
    u32Tmp = (unsigned int)(0xfff0 + (fecout & 0x0f));
    desBytes[i++] = (unsigned char)(u32Tmp >> 8);
    desBytes[i++] = (unsigned char)(u32Tmp);
    // modulation
    desBytes[i++] = (unsigned char)atoi(mprGetJson(jsonparam, "qam"));
    if(ksm == 0){
        u32int = 0;
    }else{
        if(ksm<0 || ksm> 0x99999999)
            return 0;
        u32int = ksm;
    }
    u32int *= 0x10; // k步进省去的1位
    u32int <<= 4;
    u32int += (unsigned int)(atoi(mprGetJson(jsonparam, "fecin")) & 0x0f);  // fec inner
    desBytes[i++] = (unsigned char)(u32int >> 24);
    desBytes[i++] = (unsigned char)(u32int >> 16);
    desBytes[i++] = (unsigned char)(u32int >> 8);
    desBytes[i++] = (unsigned char)(u32int);
    if (cnt > 0)
    {
        desBytes[i++] = 0x41;
        desBytes[i++] = (unsigned char)(cnt * 3);
        for(j=0;j<cnt;j++){
            memset(idstr, 0, sizeof(idstr));
            sprintf(idstr, "nitc_id%d", j);
            tmpstr = mprGetJson(jsonparam, idstr);
            sscanf(tmpstr, "%d", &val);
            desBytes[i++] = (unsigned char)(val >> 8);
            desBytes[i++] = (unsigned char)val;
            memset(idstr, 0, sizeof(idstr));
            sprintf(idstr, "nitc_type%d", j);
            tmpstr = mprGetJson(jsonparam, idstr);
            sscanf(tmpstr, "%d", &val);
            desBytes[i++] = (unsigned char)val;
        }
    }
    nitstream(nist, flag, i, desBytes, jsonparam);
    return 1;
}

static int ShowCableDes(Nit_streamLoop_t *streamLoop , cJSON *result){
    unsigned char desBytes[streamLoop->desList->bufLen];
    memcpy(desBytes, streamLoop->desList->pbuf, streamLoop->desList->bufLen);
    if(desBytes[0] == 0x44){
        cJSON_AddNumberToObject(result,"streamid", streamLoop->streamId);
        cJSON_AddNumberToObject(result,"netid", streamLoop->original_network_id);
        int offset = 2;
        if ((streamLoop->desList->bufLen < (11 + offset)))
            return 0;
        int i = offset, j = 0;
        unsigned int u32Tmp;
        // freq
        u32Tmp = (unsigned int)(desBytes[i++] << 24);
        u32Tmp += (unsigned int)(desBytes[i++] << 16);
        u32Tmp += (unsigned int)(desBytes[i++] << 8);
        u32Tmp += (unsigned int)(desBytes[i++]);
        u32Tmp /= 0x10; // k步进省略1位
        cJSON_AddNumberToObject(result,"hz", u32Tmp);//hz
        // reserved
        i++;
        // fec outer
        cJSON_AddNumberToObject(result,"fecout", desBytes[i++] & 0x0f);
        // modulation
        cJSON_AddNumberToObject(result,"qam", desBytes[i++]);
        // symbol rate
        u32Tmp = (unsigned int)(desBytes[i++] << 24);
        u32Tmp += (unsigned int)(desBytes[i++] << 16);
        u32Tmp += (unsigned int)(desBytes[i++] << 8);
        u32Tmp += (unsigned int)(desBytes[i]);
        u32Tmp /= 0x10; // k步进省略1位
        cJSON_AddNumberToObject(result,"ksm", (u32Tmp >> 4));
        // fec inter
        cJSON_AddNumberToObject(result,"fecin", (int)(desBytes[i++] & 0x0f));
        int length = streamLoop->desList->bufLen - 2;
        cJSON *streamsarray, *streamjson;
        if (i < offset + length){
            cJSON_AddItemToObject(result, "children", streamsarray = cJSON_CreateArray());
            //unsigned char desTag = desBytes[i++];
            //unsigned char desLength = desBytes[i++];
            i += 2;
            for (j = 0; j < offset + length; j++){
                if (i + 3 > offset + length)
                    break;
                cJSON_AddItemToArray(streamsarray,streamjson = cJSON_CreateObject());
                int serId = desBytes[i++] << 8;
                serId |= desBytes[i++];
                cJSON_AddNumberToObject(streamjson,"serid", serId);
                int serType = desBytes[i++];
                cJSON_AddNumberToObject(streamjson,"sertype", serType);
            }
        }
    }
    cJSON_AddNumberToObject(result,"sts", 1);
    return 1;
}

static void addnitinfo(HttpConn *conn) {
    char str[16] = {0};
    cchar *role = getSessionVar("role");
    if(role == NULL){
        rendersts(str, 9);
        render(str);
        return;
    }
    if((strcmp(role, "root") !=0) && (strcmp(role, "admin") !=0)){
        rendersts(str, 5);//无权限
        render(str);
        return;
    }
    MprJson *jsonparam = httpGetParams(conn);
    //printf("==========addnitinfo===========%s\n", mprJsonToString (jsonparam, MPR_JSON_QUOTES));
    int channel = atoi(mprGetJson(jsonparam, "channel"));
    int flag = atoi(mprGetJson(jsonparam, "flag"));
    int netid = atoi(mprGetJson(jsonparam, "netid"));
    cchar *name = mprGetJson(jsonparam, "name");
    Nit_section_t *nist = NULL;
    list_get(&clsProgram.NitSection, channel - 1, &nist);
    if(flag){
        //add
        if(nist->networkId != 0x00){
            rendersts(str, 0);
            render(str);
            return;
        }else{
            nist->networkId = netid;
            nist->streamLoopLen = 0;
            Commdes_t *nameList = malloc(sizeof(Commdes_t));
            memset(nameList, 0 , sizeof(Commdes_t));
            nameList->index = 1;
            nameList->tag = 0x40;
            nameList->dataLen = strlen((unsigned char*)name);
            nameList->data = malloc(sizeof(unsigned char) * nameList->dataLen + 1);
            memset(nameList->data, 0, nameList->dataLen);
            strcpy(nameList->data, name);
            nist->nameList = nameList;
        }
    }else{
        //edit
        nist->networkId = netid;
        free(nist->nameList->data);
        nist->nameList->data = NULL;
        nist->nameList->dataLen = strlen((unsigned char*)name);
        nist->nameList->data = malloc(sizeof(unsigned char) * nist->nameList->dataLen + 1);
        memset(nist->nameList->data, 0, nist->nameList->dataLen);
        strcpy(nist->nameList->data, name);
    }
    rendersts(str, 1);
    render(str);
}

static void delnit(HttpConn *conn) {
    char str[16] = {0};
    int i = 0;
    cchar *role = getSessionVar("role");
    if(role == NULL){
        rendersts(str, 9);
        render(str);
        return;
    }
    if((strcmp(role, "root") !=0) && (strcmp(role, "admin") !=0)){
        rendersts(str, 5);//无权限
        render(str);
        return;
    }
    MprJson *jsonparam = httpGetParams(conn);
    cchar *inChn = mprGetJson(jsonparam, "channel");
    Nit_section_t *nist = NULL;
    int inCh = atoi(inChn);
    list_get(&clsProgram.NitSection, inCh-1, &nist);
    if(nist->nameList->dataLen > 0){
        free(nist->nameList->data);
    }
    free(nist->nameList);
    Nit_streamLoop_t *streamLoop = nist->streamLoop;
    for(i=0; i<nist->streamLoopLen;i++){
        free(streamLoop->desList->pbuf);
        free(streamLoop->desList);
        streamLoop++;
    }
    if(nist->streamLoopLen > 0){
        free(nist->streamLoop);
    }
    memset(nist, 0, sizeof(Nit_section_t));
    rendersts(str, 1);
    render(str);
}

static void addnitcstream(HttpConn *conn) {
    char idstr[16] = {0};
    int i = 0, val = 0;
    cchar *role = getSessionVar("role");
    if(role == NULL){
        rendersts(idstr, 9);
        render(idstr);
        return;
    }
     if((strcmp(role, "root") !=0) && (strcmp(role, "admin") !=0)){
        rendersts(idstr, 5);//无权限
        render(idstr);
        return;
    }
    MprJson *jsonparam = httpGetParams(conn);
    cchar *tmpstr;
    int inCh = atoi(mprGetJson(jsonparam, "channel"));
    int cnt = atoi(mprGetJson(jsonparam, "cnt"));
    int flag = atoi(mprGetJson(jsonparam, "flag"));
    //验证数据
    int streamid = atoi(mprGetJson(jsonparam, "streamid"));
    if(streamid>0xffff || streamid<0){
        memset(idstr, 0, sizeof(idstr));
        rendersts(idstr, 0);
        render(idstr);
        return;
    }
    int netid = atoi(mprGetJson(jsonparam, "netid"));
    if(netid>0xffff || netid<0){
        memset(idstr, 0, sizeof(idstr));
        rendersts(idstr, 0);
        render(idstr);
        return;
    }
    for(i=0;i<cnt;i++){
        memset(idstr, 0, sizeof(idstr));
        sprintf(idstr, "nitc_id%d", i);
        tmpstr = mprGetJson(jsonparam, idstr);
        sscanf(tmpstr, "%d", &val);
        if(val>0xffff || val<0){
            memset(idstr, 0, sizeof(idstr));
            rendersts(idstr, 0);
            render(idstr);
            return;
        }
    }
    Nit_section_t *nist = NULL;
    list_get(&clsProgram.NitSection, inCh-1, &nist);
    if(Perform(jsonparam, nist, flag) == 0){
        memset(idstr, 0, sizeof(idstr));
        rendersts(idstr, 0);
        render(idstr);
        return;
    }
    memset(idstr, 0, sizeof(idstr));
    rendersts(idstr, 1);
    render(idstr);
}

static void getsection(HttpConn *conn) {
    char str[100] = {0};
    cchar *role = getSessionVar("role");
    if(role == NULL){
        rendersts(str, 9);
        render(str);
        return;
    }
    if((strcmp(role, "root") !=0) && (strcmp(role, "admin") !=0)){
        rendersts(str, 5);//无权限
        render(str);
        return;
    }
    MprJson *jsonparam = httpGetParams(conn);
    cchar *inChn = mprGetJson(jsonparam, "channel");
    Nit_section_t *nist = NULL;
    int inCh = atoi(inChn);
    list_get(&clsProgram.NitSection, inCh-1, &nist);
    cJSON *result = cJSON_CreateObject();
    char* jsonstring;
    cJSON_AddNumberToObject(result,"streamid", nist->networkId);
    memcpy(str, nist->nameList->data, nist->nameList->dataLen);
    cJSON_AddStringToObject(result,"name", str);
    jsonstring = cJSON_PrintUnformatted(result);
    memset(str, 0 , sizeof(str));
    memcpy(str, jsonstring, strlen(jsonstring));
    render(str);
    //释放内存
    cJSON_Delete(result);
    free(jsonstring);

}

static void getstream(HttpConn *conn) {
    int i = 0;
    char idstr[32] = {0};
    char str[1024] = {0};
    cJSON *result = cJSON_CreateObject();
    char* jsonstring;
    MprJson *jsonparam = httpGetParams(conn);
    cchar *inChn = mprGetJson(jsonparam, "channel");
    Nit_section_t *nist = NULL;
    int inCh = atoi(inChn);
    list_get(&clsProgram.NitSection, inCh-1, &nist);
    int streamid = atoi(mprGetJson(jsonparam, "streamid"));
    Nit_streamLoop_t *streamLoop = nist->streamLoop;
    for(i=0; i< nist->streamLoopLen; i++){
        if(streamLoop->streamId == streamid){
            if(ShowCableDes(streamLoop, result) == 0){
                memset(idstr, 0, sizeof(idstr));
                cJSON_AddNumberToObject(result,"sts", 0);
                memcpy(idstr, jsonstring, strlen(jsonstring));
                render(idstr);
            }else{
                jsonstring = cJSON_PrintUnformatted(result);
                memcpy(str, jsonstring, strlen(jsonstring));
                render(str);
            }
            break;
        }
        streamLoop++;
    }
    //释放内存
    cJSON_Delete(result);
    free(jsonstring);
}

static void delstr(HttpConn *conn) {
    int i = 0;
    char str[32] = {0};
    MprJson *jsonparam = httpGetParams(conn);
    cchar *inChn = mprGetJson(jsonparam, "channel");
    Nit_section_t *nist = NULL;
    int inCh = atoi(inChn);
    list_get(&clsProgram.NitSection, inCh-1, &nist);
    int streamid = atoi(mprGetJson(jsonparam, "streamid"));
    int newlen = nist->streamLoopLen - 1;
    Nit_streamLoop_t *streamLoop = nist->streamLoop;
    Nit_streamLoop_t *newstreamLoopList = NULL;
    if(newlen > 0){
        Nit_streamLoop_t *newstreamLoop = malloc(newlen * sizeof(Nit_streamLoop_t));
        newstreamLoopList = newstreamLoop;
        for(i=0;i<nist->streamLoopLen;i++){
            if(streamLoop->streamId != streamid){
                //copy old stream to new
                memcpy(newstreamLoop, streamLoop, sizeof(Nit_streamLoop_t));
                newstreamLoop->desList = malloc(sizeof(BufferUn_st));
                memcpy(newstreamLoop->desList, streamLoop->desList, sizeof(BufferUn_st));
                newstreamLoop->desList->pbuf = malloc(newstreamLoop->desList->bufLen * sizeof(unsigned char));
                memcpy(newstreamLoop->desList->pbuf, streamLoop->desList->pbuf, streamLoop->desList->bufLen);
                newstreamLoop++;
            }
            streamLoop++;
        }
    }
    //释放过时的stream内存
    streamLoop = nist->streamLoop;
    if(nist->streamLoopLen > 0){
        for(i=0;i<nist->streamLoopLen;i++){
            if(streamLoop->BufferUn_stLen > 0){
                if(streamLoop->desList->bufLen > 0){
                    free(streamLoop->desList->pbuf);
                    streamLoop->desList->pbuf = NULL;
                }
                free(streamLoop->desList);
                streamLoop->desList = NULL;
            }
            streamLoop++;
        }
        free(nist->streamLoop);
        streamLoop = NULL;
    }
    if(newlen > 0){
        //指向新内存地址
        nist->streamLoop = newstreamLoopList;
    }
    nist->streamLoopLen = newlen;
    rendersts(str, 1);
    render(str);
}

static void delallstr(HttpConn *conn) {
    int i = 0;
    char str[32] = {0};
    MprJson *jsonparam = httpGetParams(conn);
    cchar *inChn = mprGetJson(jsonparam, "channel");
    Nit_section_t *nist = NULL;
    int inCh = atoi(inChn);
    list_get(&clsProgram.NitSection, inCh-1, &nist);
    Nit_streamLoop_t *streamLoop = nist->streamLoop;
    if(nist->streamLoopLen > 0){
        for(i=0;i<nist->streamLoopLen;i++){
            if(streamLoop->BufferUn_stLen > 0){
                if(streamLoop->desList->bufLen > 0){
                    free(streamLoop->desList->pbuf);
                    streamLoop->desList->pbuf = NULL;
                }
                free(streamLoop->desList);
                streamLoop->desList = NULL;
            }
            streamLoop++;
        }
        free(nist->streamLoop);
        streamLoop = NULL;
    }
    nist->streamLoopLen = 0;
    rendersts(str, 1);
    render(str);
}

static void addnitsstream(HttpConn *conn) {
    char idstr[16] = {0};
    int i = 0, val = 0;
    cchar *role = getSessionVar("role");
    if(role == NULL){
        rendersts(idstr, 9);
        render(idstr);
        return;
    }
     if((strcmp(role, "root") !=0) && (strcmp(role, "admin") !=0)){
        rendersts(idstr, 5);//无权限
        render(idstr);
        return;
    }
    MprJson *jsonparam = httpGetParams(conn);
    cchar *tmpstr;
    int inCh = atoi(mprGetJson(jsonparam, "channel"));
    int cnt = atoi(mprGetJson(jsonparam, "cnt"));
    int flag = atoi(mprGetJson(jsonparam, "flag"));
    //验证数据
    int streamid = atoi(mprGetJson(jsonparam, "streamid"));
    if(streamid>0xffff || streamid<0){
        memset(idstr, 0, sizeof(idstr));
        rendersts(idstr, 0);
        render(idstr);
        return;
    }
    int netid = atoi(mprGetJson(jsonparam, "netid"));
    if(netid>0xffff || netid<0){
        memset(idstr, 0, sizeof(idstr));
        rendersts(idstr, 0);
        render(idstr);
        return;
    }
    for(i=0;i<cnt;i++){
        memset(idstr, 0, sizeof(idstr));
        sprintf(idstr, "nits_id%d", i);
        tmpstr = mprGetJson(jsonparam, idstr);
        sscanf(tmpstr, "%d", &val);
        if(val>0xffff || val<0){
            memset(idstr, 0, sizeof(idstr));
            rendersts(idstr, 0);
            render(idstr);
            return;
        }
    }
    Nit_section_t *nist = NULL;
    list_get(&clsProgram.NitSection, inCh-1, &nist);
    if(Perform_S(jsonparam, nist, flag) == 0){
        memset(idstr, 0, sizeof(idstr));
        rendersts(idstr, 0);
        render(idstr);
        return;
    }
    memset(idstr, 0, sizeof(idstr));
    rendersts(idstr, 1);
    render(idstr);
}

static void common(HttpConn *conn) {

}

/*
    Dynamic module initialization
 */
ESP_EXPORT int esp_controller_muxnms_nitController(HttpRoute *route, MprModule *module) {
    espDefineBase(route, common);
    espDefineAction(route, "nitController-cmd-delnit", delnit);
	espDefineAction(route, "nitController-cmd-addnitinfo", addnitinfo);
	espDefineAction(route, "nitController-cmd-getsection", getsection);
	espDefineAction(route, "nitController-cmd-getstream", getstream);
	espDefineAction(route, "nitController-cmd-addnitcstream", addnitcstream);
	espDefineAction(route, "nitController-cmd-delstr", delstr);
	espDefineAction(route, "nitController-cmd-addnitsstream", addnitsstream);
	espDefineAction(route, "nitController-cmd-delallstr", delallstr);



#if SAMPLE_VALIDATIONS
    Edi *edi = espGetRouteDatabase(route);
    ediAddValidation(edi, "present", "nitController", "title", 0);
    ediAddValidation(edi, "unique", "nitController", "title", 0);
    ediAddValidation(edi, "banned", "nitController", "body", "(swear|curse)");
    ediAddValidation(edi, "format", "nitController", "phone", "/^\\(?([0-9]{3})\\)?[-. ]?([0-9]{3})[-. ]?([0-9]{4})$/");
#endif
    return 0;
}
