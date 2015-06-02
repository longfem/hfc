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
static void getDevinfo(HttpConn *conn) {
	if(session("isAuthed") != NULL){
		if (strcmp(session("isAuthed"),"true") != 0) {
			render("login.esp");
			return;
		}
	}else{
		render("login.esp");
		return;
	}
	char pProg[256] = {0};
	int r = 0;
	EdiField *src;
    getbaseJson(conn->rx->parsedUri->host, pProg);
	render(pProg);
	//delete optlog 7days ago
    Edi *db = ediOpen("db/muxnms.mdb", "mdb", EDI_AUTO_SAVE);
    time_t curTime;
    time(&curTime);
    EdiGrid *opts = ediReadTable(db, "optlog");
    if(opts != NULL){
       for (r = 0; r < opts->nrecords; r++) {
           src = opts->records[r]->fields;
           src += 4;
           if(atoi(src->value) < (curTime - 7*24*3600)){
                ediRemoveRec(db, "optlog", opts->records[r]->id);
           }
       }
    }
}

static void espinit() {
	int i=0;
	ChannelProgramSt *pst = NULL;
	Nit_section_t *nit = NULL;
	//全局变量初始化
	clsProgram._outChannelCntMax = 2;
	clsProgram._intChannelCntMax = 8;
	clsProgram._pmtMaxCnt = 29;
	clsProgram.prgNum_min = 1;
	clsProgram.prgPid_min = 0x100;
	clsProgram.prgPid_max = 0xfff;
	clsProgram.subPrgPid_min = 0x1000;
	clsProgram.subPrgPid_max = 0x1ff0;
	clsProgram.pidMap_eachTransmit_numberMax = 200;
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
		list_init(&pst->prgNodes);
		list_append(&(clsProgram.outPrgList), pst);
	}
	for(i=0; i<clsProgram._outChannelCntMax; i++){
        nit = malloc(sizeof(Nit_section_t));
        memset(nit, 0, sizeof(Nit_section_t));
        nit->networkId = 0x00;
        list_append(&(clsProgram.NitSection), nit);
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
