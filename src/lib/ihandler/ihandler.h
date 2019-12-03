#pragma once

#include "http.h"

namespace mvc
{
    class IHandler
    {
        public:
            virtual ~IHandler() = default;
            virtual HttpResponse handle(HttpRequest&) = 0;
    };
}
