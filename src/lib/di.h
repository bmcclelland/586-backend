#pragma once

namespace mvc
{
    class AppDiPolicy;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic ignored "-Wshadow"
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wsign-conversion"
//#define BOOST_DI_CFG mvc::AppDiPolicy
#include <boost-experimental/di.hpp>
#pragma GCC diagnostic pop

#include "util/containers.h"
#include <functional>

namespace mvc
{
    namespace di = boost::di;

    class AppDiPolicy : public di::config
    {
        public:
            static auto policies(...) noexcept
            {
                using namespace di::policies;
                using namespace di::policies::operators;

                return di::make_policies(constructible(is_bound<di::_>{}));
            }
    };

    class IServer;
    class IHandler;
    class IRouter;
    class IDatabase;
    
    using Injector = di::injector<
        Unique<IServer>,
        Unique<IHandler>,
        Unique<IRouter>,
        Unique<IDatabase>
    >;

    class DI
    {
        public:
            static inline std::function<Injector()> make_injector;
    };
}
