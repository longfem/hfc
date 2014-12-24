/* 
    login Controller for esp-html-mvc (esp-html-mvc) 
 */  
#include "esp.h"  
static void checkLogin() {  
    cchar *name = param("name");  
    cchar *pwd = param("pwd");  
    if(smatch("abc",name) && smatch("123",pwd))  
    {  
        renderView("login/home");  
    }else{  
        renderView("login/login-error");    
    }        
}  
static void common(HttpConn *conn) {  
}  
  
/* 
    Dynamic module initialization 
 */  
ESP_EXPORT int esp_controller_esp_mvc_login(HttpRoute *route, MprModule *module) {  
    espDefineBase(route, common);  
    espDefineAction(route, "login-cmd-check", checkLogin);  
    return 0;  
}  