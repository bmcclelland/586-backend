#pragma once

#include "util/newtype.h"
#include "util/string.h"
#include "data/new.h"
#include "config.h"

namespace mvc
{
    NEW_TYPE(String, AuthPublicKey);
    NEW_TYPE(String, AuthManagementJwt);
    NEW_TYPE(String, AuthIssuer);
    NEW_TYPE(String, AuthSubject);
    NEW_TYPE(size_t, AuthJwtLeeway);

    class AuthConfig
    {
        public:
            explicit AuthConfig(Config const&);
            AuthPublicKey public_key() const;
            AuthManagementJwt management_jwt() const;
            AuthIssuer issuer() const;
            AuthJwtLeeway jwt_leeway_secs() const;

        private:
            AuthPublicKey _public_key;
            AuthManagementJwt _management_jwt;
            AuthIssuer _issuer;
            AuthJwtLeeway _jwt_leeway_secs;
    };
}
