#pragma once

#include <stdexcept>

namespace mvc
{
    class UserError : public std::runtime_error
    {
        public:
            using std::runtime_error::runtime_error;
    };
    
    class AuthError : public std::runtime_error
    {
        public:
            using std::runtime_error::runtime_error;
    };
}
