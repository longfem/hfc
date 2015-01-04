/*
   Generated from client/app/post/post-edit.esp
 */
#include "esp.h"

static void view_dbae5238f5c982d831ab5de2a2ab28b2(HttpConn *conn) {
EdiRec      *rec = getRec();
    EdiField    *fp;
  
  espRenderBlock(conn, "<!DOCTYPE html>\n\
<html lang=\"en\">\n\
<head>\n\
    <title></title>\n\
    <meta charset=\"utf-8\" />\n\
    <link rel=\"icon\" href=\"", 116);
  espRenderString(conn, conn->rx->route->prefix);  espRenderBlock(conn, "/assets/favicon.ico\" type=\"image/x-icon\">\n\
    ", 46);
stylesheets(NULL);   espRenderBlock(conn, "\n\
</head>\n\
<body id='body' data-config='", 38);
renderConfig();   espRenderBlock(conn, "'>\n\
    <nav class=\"navbar\">\n\
        <div class=\"navbar-header\">\n\
            <a class=\"navbar-brand\" href=\"", 106);
  espRenderString(conn, conn->rx->route->prefix);  espRenderBlock(conn, "\">Home</a>\n\
        </div>\n\
    </nav>\n\
    <main class=\"container\">\n\
        <section class=\"content col-md-10\" id=\"content\">\n\
            ", 135);
  espRenderBlock(conn, "\n\
<h1>", 5);
  espRenderSafeString(conn, param("id") ? "Edit" : "Create");
  espRenderBlock(conn, " Post</h1>\n\
\n\
<form name='PostForm' class='form-horizontal' action='", 66);
  espRenderString(conn, sjoin(conn->rx->route->prefix ? conn->rx->route->prefix : "", conn->rx->route->serverPrefix, NULL));  espRenderBlock(conn, "/post/", 6);
  espRenderVar(conn, "id");
  espRenderBlock(conn, "' method=\"POST\">\n\
    ", 21);
for (fp = 0; (fp = ediGetNextField(rec, fp, 1)) != 0; ) {   espRenderBlock(conn, " \n\
        <div class='form-group'>\n\
            <label class='control-label col-md-3'>", 85);
  espRenderSafeString(conn, stitle(fp->name));
  espRenderBlock(conn, "</label>\n\
            <div class='input-group col-md-8 ", 54);
  espRenderSafeString(conn, getFieldError(fp->name) ? "has-error" : "");
  espRenderBlock(conn, "'>\n\
                ", 19);
input(fp->name, "{class: 'form-control'}");   espRenderBlock(conn, "\n\
            </div>\n\
        </div>\n\
    ", 39);
}   espRenderBlock(conn, "\n\
    <div class='form-group'>\n\
        <div class='col-md-offset-2 col-md-10'>\n\
            <input type='submit' class='btn btn-primary' name=\"submit\" value=\"Save\">\n\
            <a href='list'><button class='btn' type=\"button\">Cancel</button></a>\n\
            ", 256);
if (hasRec()) {   espRenderBlock(conn, "\n\
                <input type='submit' class='btn' name=\"submit\" value=\"Delete\">\n\
            ", 92);
}   espRenderBlock(conn, "\n\
            ", 13);
inputSecurityToken();   espRenderBlock(conn, "\n\
        </div>\n\
    </div>\n\
</form>\n\
", 35);
  espRenderBlock(conn, "\n\
        </section>\n\
        <div class=\"feedback\">\n\
            ", 63);
renderFlash("all");   espRenderBlock(conn, "\n\
        </div>\n\
    </main>\n\
    ", 32);
scripts(NULL);   espRenderBlock(conn, "\n\
</body>\n\
</html>\n\
", 17);
}

ESP_EXPORT int esp_view_dbae5238f5c982d831ab5de2a2ab28b2(HttpRoute *route, MprModule *module) {
   espDefineView(route, "client/app/post/post-edit.esp", view_dbae5238f5c982d831ab5de2a2ab28b2);
   return 0;
}
