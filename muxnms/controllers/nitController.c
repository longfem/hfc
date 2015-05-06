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
    int netid = atoi(mprGetJson(jsonparam, "netid"));
    cchar *name = mprGetJson(jsonparam, "name");
    Nit_section_t *nist = NULL;
    printf("-------->>>11\n");
    list_get(&clsProgram.NitSection, channel - 1, &nist);
    if(nist->networkId != 0x00){
        printf("-------->>>00\n");
        rendersts(str, 0);
        render(str);
        return;
    }else{
        nist->networkId = netid;
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

    rendersts(str, 1);
    render(str);
}

static void delnit(HttpConn *conn) {
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
    cchar *inChn = mprGetJson(jsonparam, "channel");
    Nit_section_t *nist = NULL;
    int inCh = atoi(inChn);
    list_get(&clsProgram.NitSection, inCh-1, &nist);
    if(nist->nameList->dataLen > 0){
        free(nist->nameList->data);
    }
    free(nist->nameList);
    memset(nist, 0, sizeof(Nit_section_t));
    rendersts(str, 1);
    render(str);
}

static void addnitcstream(HttpConn *conn) {
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
    char idstr[16] = {0};
    cchar *tmpstr;
    int inCh = atoi(mprGetJson(jsonparam, "channel"));
    int cnt = atoi(mprGetJson(jsonparam, "cnt"));
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
	espDefineAction(route, "nitController-cmd-addnitcstream", addnitcstream);

#if SAMPLE_VALIDATIONS
    Edi *edi = espGetRouteDatabase(route);
    ediAddValidation(edi, "present", "nitController", "title", 0);
    ediAddValidation(edi, "unique", "nitController", "title", 0);
    ediAddValidation(edi, "banned", "nitController", "body", "(swear|curse)");
    ediAddValidation(edi, "format", "nitController", "phone", "/^\\(?([0-9]{3})\\)?[-. ]?([0-9]{3})[-. ]?([0-9]{4})$/");
#endif
    return 0;
}
