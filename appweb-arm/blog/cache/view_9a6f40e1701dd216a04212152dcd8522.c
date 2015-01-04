/*
   Generated from client/index.esp
 */
#include "esp.h"

static void view_9a6f40e1701dd216a04212152dcd8522(HttpConn *conn) {
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
  espRenderBlock(conn, "<article>\n\
    <h1>Welcome to Embedded Server Pages</h1>\n\
\n\
    <section>\n\
        <p>ESP is a powerful \"C\"-based web framework for blazing fast dynamic web applications.\n\
        It has an page templating engine, layout pages, a Model-View-Controller framework,\n\
            embedded database, content caching and application generator.</p>\n\
    </section>\n\
\n\
    <section>\n\
        <h2>Quick Start</h2>\n\
        <ol>\n\
            <li><b><a href=\"http://embedthis.com/products/appweb/doc/guide/esp/users/pages.html\">Create ESP Pages</b></a>\n\
            <p>Create web pages under the <b>clients</b> directory.\n\
                Modify the layout in \"layouts/default.esp\" and customize the Less-based style sheet in the \n\
                \"css/app.less\" and \"css/theme.less\".</p>\n\
            </li>\n\
            <li><b><a href=\"http://embedthis.com/products/appweb/doc/guide/esp/users/controllers.html\">Generate \n\
                Models and Controllers</a></b>:\n\
                <p>Create controllers to manage your app. Run \n\
                    <a href=\"http://embedthis.com/products/appweb/doc/guide/esp/users/esp.html\"><b>esp</b></a> \n\
                    with no options to see its usage.</p>\n\
                <pre>esp generate controller NAME [action, ...]</pre>\n\
            </li>\n\
            <li><b><a href=\"http://embedthis.com/products/appweb/doc/guide/esp/users/generator.html#scaffolds\">Generate \n\
                Scaffolds</a></b>:\n\
                <p>Create entire scaffolds for large sections of your application. \n\
                <pre>esp generate scaffold NAME [field:type, ...]</pre>\n\
            </li>\n\
            \n\
            <li><b><a href=\"http://embedthis.com/products/appweb/doc/index.html\">Read the Documentation</a>\n\
                <p>Go to <a href=\"http://embedthis.com/products/appweb/doc/index.html\">\n\
                http://embedthis.com/products/appweb/doc/product/index.html</a> for the latest ESP documentation. \n\
                Here you can read quick tours, overviews and access all the ESP APIs.</b>\n\
            </li>\n\
            <li><b>Enjoy!</b></li>\n\
        </ol>\n\
    </section>\n\
</article>\n\
\n\
<aside>\n\
    <h2 class=\"section\">ESP Links</h2>\n\
    <ul>\n\
        <li><a href=\"http://embedthis.com\">Official Web Site</a></li>\n\
        <li><a href=\"http://embedthis.com/products/appweb/doc/index.html\">Documentation</a></li>\n\
        <li><a href=\"http://embedthis.com/products/appweb/doc/guide/esp/users/index.html\">ESP Guide</a></li>\n\
        <li><a href=\"https://groups.google.com/forum/#!forum/appweb\">Support Forum</a></li>\n\
        <li><a href=\"http://embedthis.com/blog/\">Blog</a></li>\n\
    </ul>\n\
</aside>\n\
", 2607);
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

ESP_EXPORT int esp_view_9a6f40e1701dd216a04212152dcd8522(HttpRoute *route, MprModule *module) {
   espDefineView(route, "client/index.esp", view_9a6f40e1701dd216a04212152dcd8522);
   return 0;
}
