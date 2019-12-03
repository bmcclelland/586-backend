#pragma once

#include "util/string.h"
#include "util/newtype.h"

namespace mvc
{
    class IServer
    {
        public:
            virtual ~IServer() = default;
            virtual void run() = 0;
    };
}
