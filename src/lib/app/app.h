#pragma once

#include "irouter/irouter.h"
#include "iserver/iserver.h"
#include "util/containers.h"

namespace mvc
{
    class App
    {
        public:
            App(Unique<IServer>);
            void start();
        
        private:
            Unique<IServer> _server;
    };
}
