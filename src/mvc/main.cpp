#include "app/app.h"
#include "di.h"
#include "util/print.h"
#include "config/config.h"

#include "iserver/httpserver.h"
#include "ihandler/httphandler.h"
#include "irouter/apirouter.h"
#include "idatabase/sqlitedatabase.h"

namespace mvc
{
    void main()
    {
        DI::make_injector = []()
        {
            return di::make_injector(
                di::bind<IServer>.to<HttpServer>().in(di::unique),
                di::bind<IHandler>.to<HttpHandler>().in(di::unique),
                di::bind<IRouter>.to<ApiRouter>().in(di::unique),
                di::bind<IDatabase>.to<SqliteDatabase>().in(di::unique),
                di::bind<>.to(ConfigPath("config.json"))
            );
        };

        auto app = DI::make_injector().create<App>();
        app.start();
    }
}

int main()
{
    mvc::main();
    return 0;
}
