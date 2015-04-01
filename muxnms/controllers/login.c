/* 
    login Controller for esp-html-mvc (esp-html-mvc) 
 */  
#include "esp.h"

static void checkLogin() {  
    cchar *name = param("username");  
    cchar *pwd = param("password");
    Edi *db = ediOpen("muxnms.mdb", "mdb", EDI_CREATE);

    //MprJson *mjson = mprParseJson("{level: 3,user: 'root',desc: 'test log',logtime: '19.99'}");
//    EdiRec *user2 = ediCreateRec(db, "user");
//    EdiRec *optlog = ediCreateRec(db, "optlog");
//    ediSetField(optlog, "title", "test");
//    ediSetField(optlog, "body", "test");
//    printf("==========user2=======rec->nfields====%d\n", user2->nfields);
//    printf("==========optlog=======rec->nfields====%d\n", optlog->nfields);
    //int res = updateRec(optlog);
    //printf("==========%d========555\n", res);

	//读取用户认证信息
	EdiRec *user = readRecWhere("user", "username", "==", name);
	MprJson *jsonparam = mprParseJson(ediRecAsJson(user, 0));
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