/*
    devinfo Controller for esp-html-mvc (esp-html-mvc)
 */
#include "esp.h"
#include "clsParams.h"
#include "getBaseJson.h"
#include "datastructdef.h"
/*
    Get a resource
 */

extern ClsProgram_st clsProgram;

//char *tmpip = "192.168.1.49";
//conn->rx->parsedUri->host
static void getDevinfo(HttpConn *conn) { \
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
	char pProg[256] = {0}; 
    getbaseJson(conn->rx->parsedUri->host, pProg);
	render(pProg);
}

static void espinit() {
	int i=0;
	ChannelProgramSt *pst = NULL;
	//全局变量初始化
	clsProgram._outChannelCntMax = 2;
	clsProgram._intChannelCntMax = 8;
	clsProgram._pmtMaxCnt = 29;
	clsProgram.prgNum_min = 1;
	clsProgram.prgPid_min = 0x100;
	clsProgram.prgPid_max = 0xfff;
	clsProgram.subPrgPid_min = 0x1000;
	clsProgram.subPrgPid_max = 0x1ff0;
	//给全局变量申请内存
	for(i=0; i<clsProgram._intChannelCntMax; i++){
		pst = malloc(sizeof(ChannelProgramSt));
		memset(pst, 0, sizeof(ChannelProgramSt));
		pst->channelId = i + 1;
		list_append(&(clsProgram.inPrgList), pst);
	}
	for(i=0; i<clsProgram._outChannelCntMax; i++){
		pst = malloc(sizeof(ChannelProgramSt));
		memset(pst, 0, sizeof(ChannelProgramSt));
		pst->channelId = i + 1;
		list_append(&(clsProgram.outPrgList), pst);
	}

	clsProgram.m_autoMuxStartPid = malloc(clsProgram._outChannelCntMax);
	clsProgram.chnBypass2 = malloc(sizeof(int) * clsProgram._outChannelCntMax);
	clsProgram.chnBypassEnable = malloc(sizeof(unsigned char)*clsProgram._outChannelCntMax);
	for (i = 0; i < clsProgram._outChannelCntMax; i++)
	{
		clsProgram.m_autoMuxStartPid[i] = 0x100;
	}

	Init(clsProgram._outChannelCntMax);


	printf("======>>>>esp init!!!!!!!\n");
}


static void common(HttpConn *conn) {

}

/*
    Dynamic module initialization
 */
ESP_EXPORT int esp_controller_muxnms_devinfo(HttpRoute *route, MprModule *module) {
    espDefineBase(route, common);
    espinit();
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
