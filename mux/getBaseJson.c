#include <string.h>
#include "cJSON.h"
#include "devinfo.h"

void getbaseJson(char *ip, char *outprg){
	cJSON *baseinfo = cJSON_CreateObject();
	char *prgjsonstring;
	char str[8] = {0};
	char srcip[16] = {0};
	unsigned int devType = 0, HardVersion = 0, SoftVersion = 0, FpgaVersion = 0, port = 0, outip = 0;
	getDeviceType(ip, &devType);	
	sprintf(str,"%x", devType);
	if(strcmp("101", str)){
		cJSON_AddStringToObject(baseinfo,"devType", "8合2复用器");
	}else if(strcmp("102", str)){
		cJSON_AddStringToObject(baseinfo,"devType", "MUX8in1");
	}else if(strcmp("201", str)){
		cJSON_AddStringToObject(baseinfo,"devType", "4合1QAM调制器");
	}else if(strcmp("211", str)){
		cJSON_AddStringToObject(baseinfo,"devType", "4合1QAM调制器9789");
	}else if(strcmp("202", str)){
		cJSON_AddStringToObject(baseinfo,"devType", "QPSK转QAM调制器");
	}else if(strcmp("203", str)){
		cJSON_AddStringToObject(baseinfo,"devType", "单路QAM调制器");
	}else if(strcmp("213", str)){
		cJSON_AddStringToObject(baseinfo,"devType", "单路QAM调制器9789");
	}else if(strcmp("204", str)){
		cJSON_AddStringToObject(baseinfo,"devType", "HQAM调制器");
	}else if(strcmp("301", str)){
		cJSON_AddStringToObject(baseinfo,"devType", "单路编码器");
	}else if(strcmp("311", str)){
		cJSON_AddStringToObject(baseinfo,"devType", "单路编码器(86391)");
	}else if(strcmp("302", str)){
		cJSON_AddStringToObject(baseinfo,"devType", "四路编码器");
	}else if(strcmp("312", str)){
		cJSON_AddStringToObject(baseinfo,"devType", "四路编码器(86391)");
	}else if(strcmp("303", str)){
		cJSON_AddStringToObject(baseinfo,"devType", "高清编码器");
	}else if(strcmp("306", str)){
		cJSON_AddStringToObject(baseinfo,"devType", "8合1标清H264编码器");
	}else if(strcmp("401", str)){
		cJSON_AddStringToObject(baseinfo,"devType", "四路TS流卫星接收机");
	}else if(strcmp("501", str)){
		cJSON_AddStringToObject(baseinfo,"devType", "IP网关，TS转IP");
	}else if(strcmp("502", str)){
		cJSON_AddStringToObject(baseinfo,"devType", "IP网关，IP转TS");
	}else if(strcmp("503", str)){
		cJSON_AddStringToObject(baseinfo,"devType", "IP网管，TS-IP双向单路");
	}else if(strcmp("601", str)){
		cJSON_AddStringToObject(baseinfo,"devType", "标准加扰机");
	}else{
		cJSON_AddStringToObject(baseinfo,"devType", "unkown");
	}	
	getPort(ip, &port);
	cJSON_AddNumberToObject(baseinfo, "port", port);
	getFpgaVersion(ip, &FpgaVersion);
	cJSON_AddNumberToObject(baseinfo, "FpgaVersion", FpgaVersion);
	getSoftVersion(ip, &SoftVersion);
	sprintf(str,"%x.%02x", SoftVersion>>8, SoftVersion & 0xFF);
	cJSON_AddStringToObject(baseinfo, "SoftVersion", str);
	getHardVersion(ip, &HardVersion);
	memset(str, 0 ,sizeof(str));
	sprintf(str,"%x.%02x", HardVersion>>8, HardVersion & 0xFF);
	cJSON_AddStringToObject(baseinfo, "HardVersion", str);
	getIp(ip, &outip);
	sprintf(srcip,"%d.%d.%d.%d", (outip & 0xff000000) >> 24, (outip & 0xff0000) >> 16, (outip & 0xff00) >> 8, outip & 0xff);
	cJSON_AddStringToObject(baseinfo, "IP", srcip);
	prgjsonstring = cJSON_PrintUnformatted(baseinfo);
	memcpy(outprg, prgjsonstring, strlen(prgjsonstring));
		
	//释放内存	
	cJSON_Delete(baseinfo);		
	free(prgjsonstring);
}