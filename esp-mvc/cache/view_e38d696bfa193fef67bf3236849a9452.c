/*
   Generated from client/index.esp
 */
#include "esp.h"

static void view_e38d696bfa193fef67bf3236849a9452(HttpConn *conn) {
  espRenderBlock(conn, "<!DOCTYPE html>\n\
<html lang=\"en\">\n\
<head>\n\
    <title>数字网管</title>\n\
    <meta charset=\"utf-8\" />\n\
    <link rel=\"icon\" href=\"img/favicon.ico\" type=\"image/x-icon\">    \n\
    <link rel=\"stylesheet\" href=\"css/ui.fancytree.min.css\" type=\"text/css\"> \n\
    <link rel=\"stylesheet\" href=\"css/jquery-ui.min.css\" type=\"text/css\">\n\
    <link rel=\"stylesheet\" href=\"css/jquery.dataTables.min.css\" type=\"text/css\">\n\
    <link rel=\"stylesheet\" href=\"css/all.css\" type=\"text/css\">\n\
</head>\n\
<body>\n\
	<div class=\"navbar\">\n\
		<nav>\n\
			<img src=\"img/nmslogo.png\" />\n\
		</nav>\n\
    </div>\n\
    <div class=\"container\">	\n\
		<div class=\"l-sidebar\">	  \n\
			<div class=\"ny_zb\">\n\
				<div class=\"ny_zblb1\">\n\
					<ul class=\"clearfix\">\n\
						<li id=\"l1\">\n\
							<a class=\"cur STRING_BASE\" href=\"javascript:toHome()\"><strong>基本信息</strong></a>\n\
						</li>\n\
						<li class=\"subs\" id=\"l2\">\n\
							<a class=\"flag STRING_DETAIL\" href=\"javascript:void(0)\"><strong>详细信息</strong></a>\n\
							<ul>               \n\
								<li id=\"l21\"><a href=\"javascript:devinfo_output()\">输出通道</a></li> 							              \n\
								<li id=\"l23\"><a href=\"\">监测</a></li>    \n\
								<li id=\"l24\"><a href=\"javascript:dev_srcset()\">源设置</a></li> \n\
								<li id=\"l25\"><a href=\"javascript:dev_outset()\">输出设置</a></li> \n\
							</ul>\n\
						</li>\n\
						<li class=\"subs\" id=\"l3\">\n\
							<a class=\"flag STRING_NETSET\" href=\"javascript:void(0)\"><strong>网络设置</strong></a>\n\
							<ul>               \n\
								<li id=\"l31\"><a href=\"\">IP地址设置</a></li>\n\
							</ul>\n\
						</li>\n\
						<li class=\"subs\" id=\"l4\">\n\
							<a class=\"flag STRING_MDEVICE\" href=\"javascript:void(0)\"><strong>设备管理</strong></a>\n\
							<ul>               \n\
								<li id=\"l41\"><a href=\"\">设备升级</a></li>               \n\
								<li id=\"l42\"><a href=\"\">恢复出厂设置</a></li>               \n\
								<li id=\"l43\"><a href=\"\">重启设备</a></li>  \n\
							</ul>\n\
						</li>	\n\
						<li class=\"subs\" id=\"l5\">\n\
							<a class=\"flag STRING_LOG\" href=\"javascript:void(0)\"><strong>日志记录</strong></a>\n\
							<ul>               \n\
								<li id=\"l51\"><a href=\"javascript:alarm_output(2)\">操作日志</a></li>               \n\
								<li id=\"l52\"><a href=\"javascript:alarm_output(1)\">告警日志</a></li> \n\
							</ul>\n\
						</li>											\n\
						<li id=\"l6\">\n\
							<a href=\"javascript:void(0)\" class=\"STRING_HELP\"><strong>帮助</strong></a>\n\
						</li>\n\
						<li id=\"l7\">\n\
							<a href=\"javascript:btn_close()\" class=\"STRING_EXIT\"><strong>退出</strong></a>\n\
						</li>\n\
					</ul>\n\
				</div>\n\
				\n\
			</div>\n\
		</div>   \n\
		<div class=\"main-content\">\n\
			<!--  CONTENT  -->\n\
		</div>\n\
		\n\
    </div>   \n\
	<div class=\"footer\">\n\
		<hr/>\n\
		<p> ", 2693);
  espRenderSafeString(conn, mprGetDate(0));
  espRenderBlock(conn, "</p>\n\
		<p>© Prevail Software, 2014-2024. All rights reserved.</p>\n\
	</div>\n\
    <script type=\"text/javascript\" src=\"lib/jquery.min.js\"></script>\n\
    <script type=\"text/javascript\" src=\"lib/jquery-ui.min.js\"></script>\n\
    <script type=\"text/javascript\" src=\"lib/jquery.fancytree.min.js\"></script>    \n\
    <script type=\"text/javascript\" src=\"lib/jquery.dataTables.min.js\"></script>  \n\
	<!--<script type=\"text/javascript\" src=\"lib/jquery.i18n.properties.js\"></script> -->\n\
	<script type=\"text/javascript\" src=\"js/alarm.js\"></script> \n\
	<script type=\"text/javascript\" src=\"js/devinfo.js\"></script> \n\
	<script src=\"lib/jquery.ui.popup.js\" type=\"text/javascript\"></script>\n\
	<script src=\"lib/jquery.fancytree.menu.js\" type=\"text/javascript\"></script>\n\
    <script type=\"text/javascript\">		\n\
		$('.main-content').empty();\n\
		$('.main-content').append(\n\
			'<table id=\"basedev\" class=\"tbl_baseinfo\" cellspacing=\"0\" width=\"100%\">'				\n\
				+'<tbody>'\n\
					+'<tr>'\n\
						+'<th><label>名称:</label></th>'\n\
						+'<th><label id=\"dev-name\" class=\"tbl_th\">T192.168.1.134</label></th>'\n\
					+'</tr>'\n\
					+'<tr>'\n\
						+'<th><label>类型:</label></th>'\n\
						+'<th><label id=\"dev-type\" class=\"tbl_th\">MUX8inl</label></th>'\n\
					+'</tr>'\n\
					+'<tr>'\n\
						+'<th><label>有效性:</label></th>'\n\
						+'<th><label id=\"dev-valid\" class=\"tbl_th\">是</label></th>'\n\
					+'</tr>'\n\
					+'<tr>'\n\
						+'<th><label>链接方式:</label></th>'\n\
						+'<th><label id=\"dev-connect\" class=\"tbl_th\">TCP</label></th>'\n\
					+'</tr>'\n\
					+'<tr>'\n\
						+'<th><label>网络地址:</label></th>'\n\
						+'<th><label id=\"dev-ip\" class=\"tbl_th\">192.168.1.134</label></th>'\n\
					+'</tr>'\n\
					+'<tr>'\n\
						+'<th><label>端口号:</label></th>'\n\
						+'<th><label id=\"dev-port\" class=\"tbl_th\">161</label></th>'\n\
					+'</tr>'\n\
					+'<tr>'\n\
						+'<th><label>硬件版本:</label></th>'\n\
						+'<th><label id=\"dev-biosversion\" class=\"tbl_th\">2.00</label></th>'\n\
					+'</tr>'\n\
					+'<tr>'\n\
						+'<th><label>软件版本:</label></th>'\n\
						+'<th><label id=\"dev-softversion\" class=\"tbl_th\">2.06</label></th>'\n\
					+'</tr>'\n\
					+'<tr>'\n\
						+'<th><label>固件版本:</label></th>'\n\
						+'<th><label id=\"dev-firmware\" class=\"tbl_th\">1</label></th>'\n\
					+'</tr>'\n\
				+'</tbody>'\n\
			+'</table>'			\n\
		);		\n\
		\n\
		function toHome() {\n\
			//TODO\n\
			//获取设备基本信息\n\
			//\n\
			//\n\
			$('.main-content').empty();\n\
			$('.main-content').append(\n\
				'<table id=\"basedev\" class=\"tbl_baseinfo\" cellspacing=\"0\" width=\"100%\">'				\n\
					+'<tbody>'\n\
						+'<tr>'\n\
							+'<th><label>名称:</label></th>'\n\
							+'<th><label id=\"dev-name\" class=\"tbl_th\">T192.168.1.134</label></th>'\n\
						+'</tr>'\n\
						+'<tr>'\n\
							+'<th><label>类型:</label></th>'\n\
							+'<th><label id=\"dev-type\" class=\"tbl_th\">MUX8inl</label></th>'\n\
						+'</tr>'\n\
						+'<tr>'\n\
							+'<th><label>有效性:</label></th>'\n\
							+'<th><label id=\"dev-valid\" class=\"tbl_th\">是</label></th>'\n\
						+'</tr>'\n\
						+'<tr>'\n\
							+'<th><label>链接方式:</label></th>'\n\
							+'<th><label id=\"dev-connect\" class=\"tbl_th\">TCP</label></th>'\n\
						+'</tr>'\n\
						+'<tr>'\n\
							+'<th><label>网络地址:</label></th>'\n\
							+'<th><label id=\"dev-ip\" class=\"tbl_th\">192.168.1.134</label></th>'\n\
						+'</tr>'\n\
						+'<tr>'\n\
							+'<th><label>端口号:</label></th>'\n\
							+'<th><label id=\"dev-port\" class=\"tbl_th\">161</label></th>'\n\
						+'</tr>'\n\
						+'<tr>'\n\
							+'<th><label>硬件版本:</label></th>'\n\
							+'<th><label id=\"dev-biosversion\" class=\"tbl_th\">2.00</label></th>'\n\
						+'</tr>'\n\
						+'<tr>'\n\
							+'<th><label>软件版本:</label></th>'\n\
							+'<th><label id=\"dev-softversion\" class=\"tbl_th\">2.06</label></th>'\n\
						+'</tr>'\n\
						+'<tr>'\n\
							+'<th><label>固件版本:</label></th>'\n\
							+'<th><label id=\"dev-firmware\" class=\"tbl_th\">1</label></th>'\n\
						+'</tr>'\n\
					+'</tbody>'\n\
				+'</table>'	\n\
			);\n\
		}\n\
		\n\
		$(\"#dev-name\")[0].textContent = \"T192.168.1.134\";\n\
		$(\"#dev-type\")[0].textContent = \"MUX8inl\";\n\
		$(\"#dev-valid\")[0].textContent = \"是\";\n\
		$(\"#dev-connect\")[0].textContent = \"TCP\";\n\
		$(\"#dev-ip\")[0].textContent = \"192.168.1.134\";\n\
		$(\"#dev-port\")[0].textContent = \"161\";\n\
		$(\"#dev-biosversion\")[0].textContent = \"2.00\";\n\
		$(\"#dev-softversion\")[0].textContent = \"2.06\";\n\
		$(\"#dev-firmware\")[0].textContent = \"1\";\n\
\n\
		function btn_close() {\n\
			var userAgent = navigator.userAgent;\n\
			if (userAgent.indexOf(\"Firefox\") != -1 || userAgent.indexOf(\"Presto\") != -1) {\n\
				window.location.replace(\"about:blank\");\n\
			} else {\n\
				window.opener = null;\n\
				window.open(\"\", \"_self\");\n\
				window.close();\n\
			}\n\
		}\n\
		\n\
		$(function(){						\n\
			$(\".ny_zblb1 ul li\").click(function(){\n\
				var thisSpan=$(this);\n\
				$(\".ny_zblb1 a\").removeClass(\"cur\");\n\
				$(this).children(\"a\").addClass(\"cur\");\n\
				$(this).children(\"ul\").slideDown(\"fast\");\n\
				$(this).siblings().children(\"ul\").slideUp(\"fast\");\n\
			});\n\
		});	  \n\
		/*loadProperties();\n\
		function loadProperties(){ \n\
			$.i18n.properties({\n\
				 name:'language',\n\
				 path:'i18n/', \n\
				 mode:'map', \n\
				 callback: function() {\n\
					$('.STRING_BASE')[0].text = $.i18n.prop('STRING_BASE');\n\
					$('.STRING_DETAIL')[0].text = $.i18n.prop('STRING_DETAIL');\n\
					$('.STRING_NETSET')[0].text = $.i18n.prop('STRING_NETSET');\n\
					$('.STRING_MDEVICE')[0].text = $.i18n.prop('STRING_MDEVICE');\n\
					$('.STRING_LOG')[0].text = $.i18n.prop('STRING_LOG');\n\
					$('.STRING_HELP')[0].text = $.i18n.prop('STRING_HELP');\n\
					$('.STRING_EXIT')[0].text = $.i18n.prop('STRING_EXIT');\n\
					$('.STRING_READ')[0].textContent = $.i18n.prop('STRING_READ');\n\
					$('.STRING_WRITE')[0].textContent = $.i18n.prop('STRING_WRITE');\n\
				 } \n\
			}); \n\
		};*/\n\
    </script> \n\
</body>\n\
</html>\n\
", 5655);
}

ESP_EXPORT int esp_view_e38d696bfa193fef67bf3236849a9452(HttpRoute *route, MprModule *module) {
   espDefineView(route, "client/index.esp", view_e38d696bfa193fef67bf3236849a9452);
   return 0;
}
