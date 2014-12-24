/*
   Generated from client/login.esp
 */
#include "esp.h"

static void view_d7aa838dd5e2ecf7eb578f6577b047b2(HttpConn *conn) {
  espRenderBlock(conn, "<!DOCTYPE html>\r\n\
<html lang=\"en\">\r\n\
<head>\r\n\
    <title>数字网管---登录</title>\r\n\
    <meta charset=\"utf-8\" />\r\n\
    <link rel=\"icon\" href=\"img/favicon.ico\" type=\"image/x-icon\">    \r\n\
    <link rel=\"stylesheet\" href=\"css/all.css\" type=\"text/css\">\r\n\
</head>\r\n\
<body>\r\n\
    <div id=\"wrapper\">	\r\n\
		<div id=\"login\" class=\"animate form\">\r\n\
			<form  action=\"mysuperscript.php\"> \r\n\
				<h1>用户登录</h1> \r\n\
				<p> \r\n\
					<label for=\"username\" class=\"uname\" data-icon=\"u\" > 用户名 </label>\r\n\
					<input id=\"username\" name=\"username\" required=\"required\" type=\"text\" placeholder=\"请输入用户名\"/>\r\n\
				</p>\r\n\
				<p> \r\n\
					<label for=\"password\" class=\"youpasswd\" data-icon=\"p\"> 密码 </label>\r\n\
					<input id=\"password\" name=\"password\" required=\"required\" type=\"password\" placeholder=\"请输入密码\" /> \r\n\
				</p>\r\n\
				<p class=\"keeplogin\"> \r\n\
					<input type=\"checkbox\" name=\"loginkeeping\" id=\"loginkeeping\" value=\"loginkeeping\" /> \r\n\
					<label for=\"loginkeeping\">记住密码</label>\r\n\
				</p>\r\n\
				<p class=\"login button\"> \r\n\
					<input type=\"submit\" value=\"登录\" /> \r\n\
				</p>\r\n\
			</form>\r\n\
		</div>\r\n\
		\r\n\
    </div>   \r\n\
	<div class=\"footer\">\r\n\
		<hr/>\r\n\
		<p>© Prevail Software, 2014-2024. All rights reserved.</p>\r\n\
	</div>\r\n\
    <script type=\"text/javascript\">		\r\n\
		\r\n\
    </script> \r\n\
</body>\r\n\
</html>\r\n\
", 1309);
}

ESP_EXPORT int esp_view_d7aa838dd5e2ecf7eb578f6577b047b2(HttpRoute *route, MprModule *module) {
   espDefineView(route, "client/login.esp", view_d7aa838dd5e2ecf7eb578f6577b047b2);
   return 0;
}
