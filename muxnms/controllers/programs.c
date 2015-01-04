/*
    programs Controller for esp-html-mvc (esp-html-mvc)
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "esp.h" 
#include "communicate.h"
#include "getPrograms.h"


//extern int getPrograms(char *ip, int inChn, char *outprg);
static void getprg() { 
	
    char ip[16] = "192.168.1.134";//param("ip");  
    int inChn = 2;//param("inChn");  
	//printf("====inch====%d\n", inChn);
	char pProg[20480] = {0}; 
    getprgsJson(ip, inChn, pProg);	
	render(pProg);
    
} 



static void common(HttpConn *conn) {
}

/*
    Dynamic module initialization
 */
ESP_EXPORT int esp_controller_muxnms_programs(HttpRoute *route, MprModule *module) {
    espDefineBase(route, common);
    espDefineAction(route, "programs-cmd-getprg", getprg);
    
#if SAMPLE_VALIDATIONS
    Edi *edi = espGetRouteDatabase(route);
    ediAddValidation(edi, "present", "programs", "title", 0);
    ediAddValidation(edi, "unique", "programs", "title", 0);
    ediAddValidation(edi, "banned", "programs", "body", "(swear|curse)");
    ediAddValidation(edi, "format", "programs", "phone", "/^\\(?([0-9]{3})\\)?[-. ]?([0-9]{3})[-. ]?([0-9]{4})$/");
#endif
    return 0;
}
