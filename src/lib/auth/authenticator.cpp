#include "authenticator.h"
#include "app/error.h"
#include "util/jwt.h"
#include "http.h"
#include "util/print.h"

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
   //         println("JWT verification succeeded");
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
    
    Authenticator::Authenticator(
         Unique<IDatabase> db, 
         AuthConfig const& auth_config
         )
        : _db(std::move(db))
        , _auth_config(auth_config)
    {
        bootstrap_admin();
        bootstrap_default();
    }

    void Authenticator::bootstrap_admin() const
    {
        RoleID admin_role_id = _auth_config.admin_role();
        UserID admin_user_id = _auth_config.bootstrap_admin_user();

        auto tx = _db->transaction();
        Shared<Role> admin_role = tx.find_shared(admin_role_id);

        if (!admin_role)
        {
            Set<Perm> admin_perms{Perm::administrate};
            admin_role = std::make_shared<Role>(admin_role_id, admin_perms);
            tx.persist(admin_role);
            println("Generated admin role");
        }

        Shared<User> admin_user = tx.find_shared(admin_user_id);

        if (!admin_user)
        {
            admin_user = std::make_shared<User>(admin_user_id);
            admin_user->roles.insert(admin_role);
            tx.persist(admin_user);
            println("Generated admin user");
        }

        tx.commit();
    }
    
    void Authenticator::bootstrap_default() const
    {
        RoleID default_role_id = _auth_config.default_role();

        auto tx = _db->transaction();
        Shared<Role> default_role = tx.find_shared(default_role_id);

        if (!default_role)
        {
            Set<Perm> default_perms{}; // TODO defineable?
            default_role = std::make_shared<Role>(default_role_id, default_perms);
            tx.persist(default_role);
            tx.commit();
            println("Generated default role");
        }
    }

    Actor Authenticator::auth(String const& jwt) const
    {
        auto const subject = verify_auth(jwt, _auth_config);
//        println("Subject is '", subject.val(), "'");
        auto const perms = get_perms(subject);
        return Actor(subject, perms);
    }
       
    static
    Set<Perm> flatten_roles(Set<Shared<Role>> const& roles)
    {
        Set<Perm> perms;

        for (auto const& role: roles)
        {
            perms.insert(role->perms.begin(), role->perms.end());
        }

        return perms;
    }
   
    Shared<Role> Authenticator::get_default_role() const
    {
        RoleID const role_id = _auth_config.default_role();
        auto const tx = _db->transaction();
        Shared<Role> role = tx.find_shared(role_id);

        if (role)
        {
            return role;
        }
        else
        {
            throw std::runtime_error("Authenticator: default role '" + role_id.val + "' does not exist in database");
        }
    }

    Set<Perm> Authenticator::get_perms(AuthSubject const& subject) const
    {
        UserID const user_id{subject.val()};
        auto tx = _db->transaction();
        Option<User> const user = tx.find_value(user_id);

        if (user)
        {
//            println("User exists");
            return flatten_roles(user->roles);
        }
        else
        {
            // Add new user for this user with default perms.
            Shared<Role> default_role = get_default_role();
            User new_user(user_id);
            new_user.roles.insert(default_role);
            tx.persist(new_user);
            tx.commit();
            println("Added new user '", user_id.val, "'with default perms");
            return flatten_roles(new_user.roles);
        }
    }
}
