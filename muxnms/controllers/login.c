/* 
    login Controller for esp-html-mvc (esp-html-mvc) 
 */  
#include "esp.h"  
static void checkLogin() {  
    cchar *name = param("username");  
    cchar *pwd = param("password");
	//Edi *db = ediOpen("db/muxnms.mdb", "mdb", EDI_AUTO_SAVE );
	/*EdiRec *user1 = ediCreateRec(db, "User");
	ediSetField(user1, "username", "root");
	ediSetField(user1, "password", "root");
	printf("=======3====\n");
	if(!ediUpdateRec(db, user1)){
		ediSave(db);
	}*/
	//读取用户认证信息
	EdiRec *user = readRecWhere("user", "username", "==", name);
	MprJson *jsonparam = mprParseJson(ediRecAsJson(user, 0));
	//printf("==========jsonparam===========%s\n", mprJsonToString(jsonparam, MPR_JSON_QUOTES));
	//ediClose(db);
	//printf("===username===>>>>%s======password=====>>>%s\n", mprGetJson(jsonparam, "username"), mprGetJson(jsonparam, "password"));
    if(strcmp(mprGetJson(jsonparam, "username"),name) && strcmp(mprGetJson(jsonparam, "password"),pwd))  
    {  
        redirect("/login.esp");   
    }else{  
		setSessionVar("isAuthed", "true");
		setSessionVar("userName", name);
		setSessionVar("role", mprGetJson(jsonparam, "roles"));
        redirect("/index.esp");
		 
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