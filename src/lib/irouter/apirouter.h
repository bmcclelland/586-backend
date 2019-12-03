#pragma once

#include "irouter.h"

namespace mvc
{
    class ApiRouter : public IRouter
    {
        public:
            Option<EndpointOutput> route(ApiPath const&, EndpointInput const&) override;
    };
}
