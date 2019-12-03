#pragma once

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wshadow"
#pragma GCC diagnostic ignored "-Wnon-virtual-dtor"
#include <odb/lazy-ptr.hxx>
#pragma GCC diagnostic pop

namespace mvc 
{
    template <typename T>
    using LazyShared = odb::lazy_shared_ptr<T>;

    template <typename T>
    using LazyWeak = odb::lazy_weak_ptr<T>;
}
