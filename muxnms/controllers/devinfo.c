/*
    devinfo Controller for esp-html-mvc (esp-html-mvc)
 */
#include "esp.h"
#include "getBaseJson.h"

/*
    Get a resource
 */

//char ip[16] = "192.168.1.49";
//char ip[16] = "127.0.0.1";

static void getDevinfo(HttpConn *conn) { 
	char *globalip = NULL;
	if(session("isAuthed") != NULL){
		if (strcmp(session("isAuthed"),"true") != 0) {
			render("login.esp");
			return;
		}
	}else{
		render("login.esp");
		return;
	}
	MprJson *jsonparam = httpGetParams(conn);
    globalip = mprGetJson(jsonparam, "ip");
    printf("=======ip===>>>%s\n", globalip);
	char pProg[256] = {0}; 
    getbaseJson(globalip, pProg);
	render(pProg);
}


static void common(HttpConn *conn) {
	
}

/*
    Dynamic module initialization
 */
ESP_EXPORT int esp_controller_muxnms_devinfo(HttpRoute *route, MprModule *module) {
    espDefineBase(route, common);
    espDefineAction(route, "devinfo-cmd-getDevinfo", getDevinfo);
    
#if SAMPLE_VALIDATIONS
    Edi *edi = espGetRouteDatabase(route);
    ediAddValidation(edi, "present", "devinfo", "title", 0);
    ediAddValidation(edi, "unique", "devinfo", "title", 0);
    ediAddValidation(edi, "banned", "devinfo", "body", "(swear|curse)");
    ediAddValidation(edi, "format", "devinfo", "phone", "/^\\(?([0-9]{3})\\)?[-. ]?([0-9]{3})[-. ]?([0-9]{4})$/");
#endif
    return 0;
}
