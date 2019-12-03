#pragma once

#include "mapbox/variant.hpp"

namespace mvc 
{
    template <typename... Ts>
    using Variant = mapbox::util::variant<Ts...>;
}
