#pragma once

#include "config/auth.h"
#include "util/string.h"
#include "util/containers.h"
#include "actor.h"

namespace mvc
{
    class Authenticator
    {
        public:
            Authenticator(AuthConfig const&);
            Actor auth(String const& jwt) const;
        
        private:
            Set<Perm> get_perms(AuthSubject const&) const;
            AuthConfig _auth_config;
    };
}
