#pragma once

#include "util/newtype.h"
#include "util/string.h"
#include "data/new.h"
#include "config.h"

namespace mvc
{
    NEW_TYPE(String, AuthPublicKey);
    NEW_TYPE(String, AuthIssuer);
    NEW_TYPE(String, AuthSubject);

    class AuthConfig
    {
        public:
            AuthConfig(Config const&);
            AuthPublicKey public_key() const;
            AuthIssuer issuer() const;
            RoleID default_role() const;
            RoleID admin_role() const;
            UserID bootstrap_admin_user() const;

        private:
            AuthPublicKey _public_key;
            AuthIssuer    _issuer;
            RoleID        _default_role;
            RoleID        _admin_role;
            UserID        _bootstrap_admin_user;
    };
}
