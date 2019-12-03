#pragma once

#include "util/containers.h"
#include "server/apipath.h"
#include "iendpoint/iendpoint.h"

namespace mvc
{
    class IRouter
    {
        public:
            virtual ~IRouter() = default;
            virtual Option<EndpointOutput> route(ApiPath const&, EndpointInput const&) = 0;
    };
}
