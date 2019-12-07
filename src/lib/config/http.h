#pragma once

#include "util/newtype.h"
#include "util/string.h"
#include "config.h"

namespace mvc
{
    NEW_TYPE(String, HttpListenAddr);

    struct HttpConfig
    {
        public:
            explicit HttpConfig(Config const&);
            HttpListenAddr listen_addr() const;

        private:
            HttpListenAddr _listen_addr;
    };
}
