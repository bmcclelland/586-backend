#include "auth.h"

namespace mvc
{
    AuthConfig::AuthConfig(Config const& config)
        : _public_key(config.get_string("auth_public_key"))
        , _issuer(config.get_string("auth_issuer"))
        , _default_role{config.get_string("auth_default_role")}
        , _admin_role{config.get_string("auth_admin_role")}
        , _bootstrap_admin_user{config.get_string("auth_bootstrap_admin_user")}
    {}

    AuthPublicKey AuthConfig::public_key() const
    {
        return _public_key;
    }

    AuthIssuer AuthConfig::issuer() const
    {
        return _issuer;
    }

    RoleID AuthConfig::default_role() const
    {
        return _default_role;
    }

    RoleID AuthConfig::admin_role() const
    {
        return _admin_role;
    }
    
    UserID AuthConfig::bootstrap_admin_user() const
    {
        return _bootstrap_admin_user;
    }
}
