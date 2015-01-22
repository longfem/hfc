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

ClsProgram_t clsProgram;

static char* substr(const char*str,unsigned start, unsigned end)
{
   unsigned n = end - start;
   static char stbuf[256];
   strncpy(stbuf, str + start, n);
   stbuf[n] = 0;
   return stbuf;
}

static void getprg(HttpConn *conn) { 
	MprJson *jsonparam = httpGetParams(conn);
    char ip[16] = "192.168.1.134";//param("ip"); 
    char *inChn = mprGetJson(jsonparam, "inch"); 
	int inCh = atoi(inChn);
	char pProg[20480] = {0}; 
	ChannelProgram_t pst;
	ChannelProgram_t tmpst;
	pst.channelId = inCh;
	list_t test;
    getprgsJson(ip, inCh, pProg, &pst);
	//list_init(&(clsProgram.inPrgList));
	//list_append(&(clsProgram.inPrgList), &pst);
	//list_get(&(clsProgram.inPrgList), inChn - 1, &tmpst);
	list_init(&test);
	list_append(&test, &pst);
	list_get(&test, 0, &tmpst);
	//program_info_t *ptmpPrgInfo;
	//list_get(&(clsProgram.inPrgList), 1, &ptmpPrgInfo);	
	printf("=====%d=>>>>%d\n", pst.channelId, tmpst.channelId);
	render(pProg);
    
} 

static void maketable(HttpConn *conn) { 
	MprJson *jsonparam = httpGetParams(conn);
	printf("======>>>>%s\n", mprGetJson(jsonparam, "ip"));
	render("-----------");
    
} 


static void common(HttpConn *conn) {
	
	
}

/*
    Dynamic module initialization
 */
ESP_EXPORT int esp_controller_muxnms_programs(HttpRoute *route, MprModule *module) {
    espDefineBase(route, common);
    espDefineAction(route, "programs-cmd-getprg", getprg);
	espDefineAction(route, "programs-cmd-maketable", maketable);
    
#if SAMPLE_VALIDATIONS
    Edi *edi = espGetRouteDatabase(route);
    ediAddValidation(edi, "present", "programs", "title", 0);
    ediAddValidation(edi, "unique", "programs", "title", 0);
    ediAddValidation(edi, "banned", "programs", "body", "(swear|curse)");
    ediAddValidation(edi, "format", "programs", "phone", "/^\\(?([0-9]{3})\\)?[-. ]?([0-9]{3})[-. ]?([0-9]{4})$/");
#endif
    return 0;
}
