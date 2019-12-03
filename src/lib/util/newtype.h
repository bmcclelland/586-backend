#pragma once

#include <utility>

namespace mvc 
{
    template <typename T, typename Tag>
    class NewType
    {
        public:
            explicit NewType(T const& t)
                : _val(t)
            {}
            
            explicit NewType(T&& t)
                : _val(std::move(t))
            {}

            T const& val() const
            {
                return _val;
            }

            operator T const&() const
            {
                return _val;
            }

            bool operator==(NewType const& b) const
            {
                return val() == b.val();
            }

            bool operator!=(NewType const& b) const
            {
                return !(*this == b);
            }

            bool operator<(NewType const& b) const
            {
                return val() < b.val();
            }

        private:
            T _val;
    };

    struct NewTypeTag {};

    template <typename T>
    auto const& unwrap_newtype(T const& x)
    {
        if constexpr (std::is_base_of<NewTypeTag, T>::value)
        {
            return x.val();
        }
        else
        {
            return x;
        }
    }
}

#define NEW_TYPE(T, Tag)\
    class Tag : public NewTypeTag, public NewType<T,Tag>\
    {\
        public:\
        using NewTypeType = T;\
        using NewType<T,Tag>::NewType;\
    }\
// END NEW_TYPE
