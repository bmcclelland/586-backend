#pragma once

#include <memory>

namespace mvc {

template <typename... Ts>
class EnvT : public std::enable_shared_from_this<EnvT<Ts...>>, public Ts...
{
    public:
        EnvT()
            : Ts(this->shared_from_this())...
        {}
};

} // namespace mvc
