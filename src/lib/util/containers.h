#pragma once

#include <optional>
#include <vector>
#include <memory>
#include <tuple>
#include <set>

namespace mvc
{
    template <typename T>
    using Option = std::optional<T>;

    using std::nullopt;
    
    template <typename T>
    using Shared = std::shared_ptr<T>;

    template <typename T>
    using Unique = std::unique_ptr<T>;
    
    template <typename T>
    using Vec = std::vector<T>;

    template <typename... Ts>
    using Tuple = std::tuple<Ts...>;

    template <typename T>
    using Set = std::set<T>;
}
