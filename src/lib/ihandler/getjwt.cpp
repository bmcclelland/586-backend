#include "getjwt.h"
#include "app/error.h"
#include "util/jwt.h"
#include "util/print.h"
#include <cctype>

namespace mvc
{
    static
    auto split_auth_header(String const& header)
    {
        struct AuthHeader
        {
            String scheme;
            String token;
        };

        AuthHeader auth;
//        println("split_auth_header(", header, ")");
            
        enum class State { scheme, token };
        State state = State::scheme;
       
        for (char c : header)
        {
            switch (state)
            {
                case State::scheme:
                    if (c == ' ')
                    {
                        state = State::token;
                    }
                    else
                    {
                        auth.scheme += static_cast<char>(std::tolower(c));
                    }
                    break;
                case State::token:
                    auth.token += c;
                    break;
            }
        }

        return auth;
    }
    
    Option<String> get_jwt(HttpRequest const& req)
    {
        auto const& headers = req.headers();
        auto const it = headers.find("authorization");

        if (it == headers.end())
        {
            println("No auth header");
            return nullopt;
        }

        auto const auth = split_auth_header(it->second);

        if (auth.scheme != "bearer")
        {
            println("Not bearer");
            return nullopt;
        }

        return auth.token;
    }
}
