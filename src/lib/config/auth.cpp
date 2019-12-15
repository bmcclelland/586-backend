#include "auth.h"

namespace mvc
{
    AuthConfig::AuthConfig(Config const& config)
        : _public_key(config.get_string("auth_public_key"))
        , _management_jwt(config.get_string("auth_management_jwt"))
        , _issuer(config.get_string("auth_issuer"))
        , _jwt_leeway_secs{static_cast<size_t>(config.get_int("auth_jwt_leeway_secs"))}
    {}

    AuthPublicKey AuthConfig::public_key() const
    {
        return _public_key;
    }

    AuthManagementJwt AuthConfig::management_jwt() const
    {
        return _management_jwt;
    }

    AuthIssuer AuthConfig::issuer() const
    {
        return _issuer;
    }

    AuthJwtLeeway AuthConfig::jwt_leeway_secs() const
    {
        return _jwt_leeway_secs;
    }
}
