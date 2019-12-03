#pragma once

#include "ihandler.h"
#include "irouter/irouter.h"
#include "http.h"
#include "util/print.h"
#include "config/auth.h"
#include "auth/authenticator.h"

namespace mvc
{
    class HttpHandler : public IHandler
    {
        public:
            HttpHandler(Unique<IRouter>, Unique<Authenticator>);
            HttpResponse handle(HttpRequest&) override;

        private:
            Unique<IRouter> _router;
            Unique<Authenticator> _authenticator;
    };
}
