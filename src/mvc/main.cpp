#include "app/app.h"
#include "di.h"
#include "util/print.h"
#include "config/config.h"
#include "config/sqlite.h"
#include "config/auth.h"
#include "config/http.h"

#include "iserver/httpserver.h"
#include "ihandler/httphandler.h"
#include "irouter/apirouter.h"
#include "idatabase/sqlitedatabase.h"

namespace mvc
{
    void main()
    {
        Config const config("config.json"); // TODO
        
        // NB: boost::di binds values by reference, leading to easy UB!
        // Giving them static lifetimes solves this.
        static HttpConfig   const http_config(config);
        static AuthConfig   const auth_config(config);
        static SqliteConfig const sqlite_config(config);
            
        println("HttpListenAddr: ", http_config.listen_addr());
        println("SqliteFile:     ", sqlite_config.db_path());
        println("AuthPublicKey:  ", auth_config.public_key());
        println("AuthIssuer:     ", auth_config.issuer());

        DI::make_injector = []()
        {
            return di::make_injector(
                di::bind<IServer>.to<HttpServer>().in(di::unique),
                di::bind<IHandler>.to<HttpHandler>().in(di::unique),
                di::bind<IRouter>.to<ApiRouter>().in(di::unique),
                di::bind<IDatabase>.to<SqliteDatabase>().in(di::unique),
                di::bind<Authenticator>.in(di::unique),
                di::bind<>.to(http_config),
                di::bind<>.to(sqlite_config),
                di::bind<>.to(auth_config)
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
