/*
    globalopt Controller for esp-html-mvc (esp-html-mvc)
 */
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include "esp.h"
#include "devinfo.h"
/*
    Create a new resource in the database
 */ 
 
 void substr(const char*str,unsigned start, unsigned end, char *stbuf)
{
   unsigned n = end - start;
   strncpy(stbuf, str + start, n);
   stbuf[n] = 0;
}

static void reboot() { 
    char ip[16] = "192.168.1.134";
	rebootDevice(ip);
	render("reboot");
}

static void reset() { 
    char ip[16] = "192.168.1.134";
	restoreFactory(ip);
	render("reset");
}

static void setDevip(HttpConn *conn) { 
	cchar *param = espGetQueryString(conn);	
    char ip[16] = "192.168.1.134";
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
	render("-----------------setIp receive %s====%s=====%s\n", newip, newgatway, submask);
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
    
#if SAMPLE_VALIDATIONS
    Edi *edi = espGetRouteDatabase(route);
    ediAddValidation(edi, "present", "globalopt", "title", 0);
    ediAddValidation(edi, "unique", "globalopt", "title", 0);
    ediAddValidation(edi, "banned", "globalopt", "body", "(swear|curse)");
    ediAddValidation(edi, "format", "globalopt", "phone", "/^\\(?([0-9]{3})\\)?[-. ]?([0-9]{3})[-. ]?([0-9]{4})$/");
#endif
    return 0;
}
