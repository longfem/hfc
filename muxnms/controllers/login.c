/* 
    login Controller for esp-html-mvc (esp-html-mvc) 
 */  
#include "esp.h"
#include "cJSON.h"

static void checkLogin() {
    cchar *name = param("username");
    cchar *pwd = param("password");
    cJSON *result = cJSON_CreateObject();
    char* jsonstring;
    char str[16] = {0};
	//读取用户认证信息
	EdiRec *user = readRecWhere("user", "username", "==", name);
	if(user == NULL){
	    cJSON_AddNumberToObject(result,"sts", 1);
        jsonstring = cJSON_PrintUnformatted(result);
        memcpy(str, jsonstring, strlen(jsonstring));
	    render(jsonstring);
	    //释放内存
        cJSON_Delete(result);
        free(jsonstring);
	    return;
	}
	MprJson *userjson = mprParseJson(ediRecAsJson(user, 0));

    if(strcmp(mprGetJson(userjson, "password"),pwd) == 0 )
    {
        setSessionVar("isAuthed", "true");
        setSessionVar("userName", name);
        setSessionVar("role", mprGetJson(userjson, "roles"));
        cJSON_AddNumberToObject(result,"sts", 0);
        jsonstring = cJSON_PrintUnformatted(result);
        memcpy(str, jsonstring, strlen(jsonstring));
        render(jsonstring);
        //释放内存
        cJSON_Delete(result);
        free(jsonstring);

    }else{
		cJSON_AddNumberToObject(result,"sts", 2);
        jsonstring = cJSON_PrintUnformatted(result);
        memcpy(str, jsonstring, strlen(jsonstring));
        render(jsonstring);
        //释放内存
        cJSON_Delete(result);
        free(jsonstring);
    }        
}  

static void checkLoginOut() {  
	setSessionVar("isAuthed", "false");
	render("OK");
}  

static void common(HttpConn *conn) {  
}  
  
/* 
    Dynamic module initialization 
 */  
ESP_EXPORT int esp_controller_muxnms_login(HttpRoute *route, MprModule *module) {  
    espDefineBase(route, common);  
    espDefineAction(route, "login-cmd-check", checkLogin);  
	espDefineAction(route, "login-cmd-logout", checkLoginOut); 
    return 0;  
}  