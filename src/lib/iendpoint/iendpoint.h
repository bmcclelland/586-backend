#pragma once

#include "util/containers.h"
#include "util/string.h"
#include "util/json.h"
#include "util/print.h"
#include "util/typename.h"
#include "auth/requirements.h"

namespace mvc
{
    class EndpointInput
    {
        public:
            EndpointInput(Json&& t, Option<Actor> a)
                : payload(std::move(t))
                , actor(a)
            {}

            Json payload;
            Option<Actor> actor;
    };

    class EndpointOutput
    {
        public:
            template <typename T>
            EndpointOutput(T&& t)
            try
                : payload(t)
            {}
            catch (Json::exception& ex)
            {
                println("EndpointOutput<", type_name<T>, ">: ", ex.what());
                throw;
            }
    
            Json payload;
    };
    
    class IEndpoint
    {
        public:
            virtual ~IEndpoint() = default;
            virtual Option<EndpointOutput> call(EndpointInput const&) = 0;
            virtual Requirements requirements() const = 0;
    };
}
