/* 
    login Controller for esp-html-mvc (esp-html-mvc) 
 */  
#include "esp.h"  
static void checkLogin() {  
    cchar *name = param("username");  
    cchar *pwd = param("password");  
	Edi *db = ediOpen("db/muxnms.mdb", "mdb", EDI_CREATE );
	/*EdiRec *user = ediCreateRec(db, "User");
	ediSetField(user, "username", "root");
	ediSetField(user, "password", "root");
	if(!ediUpdateRec(db, user)){
		ediSave(db);
	}*/
	//读取用户认证信息
	EdiRec *user = ediReadRec(db, "User", "1");
	MprJson *jsonparam = mprParseJson(ediRecAsJson(user, 0));
	
	ediClose(db);
	//printf("===username===>>>>%s======password=====>>>%s\n", mprGetJson(jsonparam, "username"), mprGetJson(jsonparam, "password"));
    if(strcmp(mprGetJson(jsonparam, "username"),name) && strcmp(mprGetJson(jsonparam, "password"),pwd))  
    {  
        redirect("/login.esp");   
    }else{  
		setSessionVar("isAuthed", "true");
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