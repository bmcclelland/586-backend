#pragma once

#include "iserver.h"
#include "ihandler/ihandler.h"
#include "http.h"
#include "util/containers.h"
#include "config/http.h"
#include <cpprest/http_listener.h>
#include <cpprest/http_msg.h>

namespace mvc
{
    class HttpServer : public IServer
    {
        public:
            using Listener = web::http::experimental::listener::http_listener;
            
            HttpServer(HttpConfig const&, Unique<IHandler>);
            void run() override;
        
        private:
            Listener _listener;
            Unique<IHandler> _handler;
    };
}
