#include "httpserver.h"
#include "util/containers.h"
#include "util/print.h"

namespace mvc
{
    HttpServer::HttpServer(HttpConfig const& config, Unique<IHandler> handler)
        : _listener(config.listen_addr().val())
        , _handler(std::move(handler))
    {
        println("HttpServer::HttpServer");
        _listener.support(
            [this] (HttpRequest req)
            {
                HttpResponse rsp = _handler->handle(req);
                req.reply(rsp);
            }
        );
        _listener.support(web::http::methods::OPTIONS,
            [this] (HttpRequest req)
            {
                HttpResponse rsp = _handler->handle(req);
                req.reply(rsp);
            }
        );
    }
   
    void HttpServer::run()
    {
        println("HttpServer::run");
        // In debug mode, open() throws an error code even on success.
        // So we'll just intercept that.
        try 
        {
            _listener.open().wait();
        }
        catch (boost::system::system_error &e) 
        {
            if (e.code().value() != 0) 
            {
                throw;
            }
        }
    }

}
