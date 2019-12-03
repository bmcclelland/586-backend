#pragma once

#include "config/auth.h"
#include "idatabase/idatabase.h"
#include "util/string.h"
#include "util/containers.h"
#include "actor.h"

namespace mvc
{
    class Authenticator
    {
        public:
            Authenticator(Unique<IDatabase>, AuthConfig const&);
            Actor auth(String const& jwt) const;
        
        private:
            Set<Perm> get_perms(AuthSubject const&) const;
            Shared<Role> get_default_role() const;
            void bootstrap_admin() const;
            void bootstrap_default() const;

            Unique<IDatabase> _db;
            AuthConfig _auth_config;
    };
}
