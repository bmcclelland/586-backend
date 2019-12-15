#include "authenticator.h"
#include "app/error.h"
#include "util/jwt.h"
#include "http.h"
#include "util/print.h"

#include "httpclient.h"

namespace mvc
{
    static
    auto decode_jwt(String const& token)
    {
        try
        {
            return jwt::decode(token);
        }
        catch (std::exception& e)
        {
            throw jwt::token_verification_exception(e.what());
        }
    }
    
    static
    AuthSubject verify_auth(String const& token, AuthConfig const& auth_config)
    {
        try
        {
            auto const decoded_jwt = decode_jwt(token);

            auto const verifier = jwt::verify()
                .leeway(
                    auth_config.jwt_leeway_secs().val()
                )
                .allow_algorithm(
                    jwt::algorithm::rs256(auth_config.public_key().val())
                )
                .with_issuer(
                    auth_config.issuer().val()
                );
            verifier.verify(decoded_jwt);
            
            return AuthSubject{
                decoded_jwt.get_subject()
            };
        }
        catch (jwt::token_verification_exception& e)
        {
            println("JWT verification failed: ", e.what());
            throw AuthError(e.what());
        }
        catch (std::exception& e)
        {
            println("verify_auth: ", e.what());
            throw AuthError(e.what());
        }
    }
    
    Authenticator::Authenticator(AuthConfig const& auth_config)
        : _auth_config(auth_config)
    {}

    Actor Authenticator::auth(String const& jwt) const
    {
        auto const subject = verify_auth(jwt, _auth_config);
//        println("Subject is '", subject.val(), "'");
       
        auto const perms = get_perms(subject);
        return Actor(subject, perms);
    }
       
    Set<Perm> Authenticator::get_perms(AuthSubject const& subject) const
    {
        Set<Perm> perms;
        String const token = _auth_config.management_jwt().val();
        String const url = _auth_config.issuer().val()
                         + "api/v2/users/" 
                         + subject.val()
                         + "/permissions";
        Option<String> const response = http_req(url, token);

        if (response)
        {
            Json const perms_json = Json::parse(*response);

            for (auto const& p : perms_json)
            {
                perms.insert(
                    p.at("permission_name").get<String>()
                    );
            }
        }
        else
        {
            println("Failed to get permissions from API");
        }

        return perms;
    }
}
