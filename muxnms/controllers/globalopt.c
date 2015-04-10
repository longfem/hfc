/*
    globalopt Controller for esp-html-mvc (esp-html-mvc)
 */
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include "esp.h"
#include "devinfo.h"
#include "cJSON.h"
/*
    Create a new resource in the database
 */
 //char ip[16] = "192.168.1.134";
 char ip[16] = "127.0.0.1";
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

void substr(const char*str,unsigned start, unsigned end, char *stbuf)
{
   unsigned n = end - start;
   strncpy(stbuf, str + start, n);
   stbuf[n] = 0;
}

static void reboot() {
    char str[16] = {0};
    cchar *role = getSessionVar("role");
    if(role == NULL){
        rendersts(str, 9);
        render(str);
        return;
    }
    if((strcmp(role, "root") !=0) && (strcmp(role, "admin") !=0)){
        rendersts(str, 5);//无权限设置Ip
        render(str);
        return;
    }
    
	rebootDevice(ip);
	rendersts(str, 1);
	render(str);
}

static void reset() {
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
    
	restoreFactory(ip);
	rendersts(str, 1);
	render(str);
}

static void setDevip(HttpConn *conn) {
    char str[16] = {0};
    cchar *role = getSessionVar("role");
    if(role == NULL){
        rendersts(str, 9);
        render(str);
        return;
    }
    if((strcmp(role, "root") !=0) && (strcmp(role, "admin") !=0)){
        rendersts(str, 5);//无权限设置Ip
        render(str);
        return;
    }
	cchar *param = espGetQueryString(conn);	
   
	char newip[16] = {0}; 	
	char newgatway[16] = {0}; 
	char submask[16] = {0}; 
	char* ptr = strchr(param, '&');  
    int pos = ptr-param;  	
	substr(param, 0, pos, newip);
	char *ptr1 = strrchr (param, '&');  
	int flag = ptr1 - param;
	substr(param, pos + 1, flag, newgatway);
	substr(param, flag + 1, strlen(param), submask);
	unsigned int tmpip = ntohl( inet_addr( newip ) );
	unsigned int tmpgatway = ntohl( inet_addr( newgatway ) );
	unsigned int tmpsubmask = ntohl( inet_addr( submask ) );
	if(0 == setIp(ip, tmpip)){
		setGateway(ip, tmpgatway);
		getSubMask(ip, tmpsubmask);
	}
	rendersts(str, 1);
	render(str);
}

static void setPassword(HttpConn *conn) {
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
    MprJson *jsonparam = mprParseJson(espGetQueryString(conn));
    cchar *oldpassword = mprGetJson(jsonparam, "oldpassword");
    cchar *newpassword = mprGetJson(jsonparam, "newpassword");

    cchar *username = getSessionVar("userName");
    //Edi *db = ediOpen("db/muxnms.mdb", "mdb", EDI_AUTO_SAVE );
    EdiRec *user = readRecWhere("user", "username", "==", username);
    MprJson *userjson = mprParseJson(ediRecAsJson(user, 0));
    //printf("=======password====%s========%s\n", oldpassword, mprGetJson(userjson, "password"));
    if(strcmp(oldpassword, mprGetJson(userjson, "password")) == 0){
        ediSetField(user, "password", newpassword);
        updateRec(user);
        rendersts(str, 1);
    }else{
        rendersts(str, 0);
    }
    //ediClose(db);
    render(str);
}


static void common(HttpConn *conn) {
}

/*
    Dynamic module initialization
 */
ESP_EXPORT int esp_controller_muxnms_globalopt(HttpRoute *route, MprModule *module) {
    espDefineBase(route, common);
    espDefineAction(route, "globalopt-cmd-reboot", reboot);
	espDefineAction(route, "globalopt-cmd-reset", reset);
	espDefineAction(route, "globalopt-cmd-setDevip", setDevip);
	espDefineAction(route, "globalopt-cmd-setPassword", setPassword);
    
#if SAMPLE_VALIDATIONS
    Edi *edi = espGetRouteDatabase(route);
    ediAddValidation(edi, "present", "globalopt", "title", 0);
    ediAddValidation(edi, "unique", "globalopt", "title", 0);
    ediAddValidation(edi, "banned", "globalopt", "body", "(swear|curse)");
    ediAddValidation(edi, "format", "globalopt", "phone", "/^\\(?([0-9]{3})\\)?[-. ]?([0-9]{3})[-. ]?([0-9]{4})$/");
#endif
    return 0;
}
