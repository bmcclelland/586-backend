#pragma once

#include <boost/core/demangle.hpp>

namespace mvc 
{
    template <typename T>
    std::string const type_name = boost::core::demangle(typeid(T).name());
}
