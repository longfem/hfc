/*
    muxer Controller for esp-html-mvc (esp-html-mvc)
 */
#include "esp.h"


/*
    Initialize a new resource for the client to complete
 */
static void getinput() { 
    //render("{input1:3, 'input2':'33'}");
    render("{input1:3, 'input2':'33'}");
}

/*
    List the resources in this group
 */ 
static void getoutput(HttpTx *tx) { 
	addHeader("Access-Control-Allow-Origin", "*");
    //render("{\"output1\":\"3\", \"output2\":\"33\"}");
    render("{\"title\": \"输入通道\", \"folder\": true, \"key\": \"id3\", \"expanded\": true,\"children\": [{\"title\": \"通道1 (ASI-1)\",\"children\": [{\"title\": \"Sub-item 3.1.1\", \"key\": \"id3.1.1\" },{\"title\": \"Sub-item 3.1.2\", \"key\": \"id3.1.2\" }]}]}");
}


static void common(HttpConn *conn) {
}

/*
    Dynamic module initialization
 */
ESP_EXPORT int esp_controller_blog_muxer(HttpRoute *route, MprModule *module) {
    espDefineBase(route, common);
    espDefineAction(route, "muxer-cmd-getinput", getinput);
    espDefineAction(route, "muxer-cmd-getoutput", getoutput);
    
    return 0;
}
