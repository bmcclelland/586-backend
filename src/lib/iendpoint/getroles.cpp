#include "getroles.h"

namespace mvc::endpoints
{
    GetRoles::GetRoles(Unique<IDatabase> db, GetRoles::PathArgs path)
        : _db(std::move(db))
        , _path(path)
    {
        println(">GetRoles");
    }

    Requirements GetRoles::requirements() const
    {
        return make_reqs(Perm::view_role);
    }

    static
    Vec<Role> get_roles(IDatabase& db)
    {
        auto const tx = db.transaction();
        return tx.collect<Role>();
    }

    Option<EndpointOutput> GetRoles::call(EndpointInput const&)
    {
        return EndpointOutput(
            get_roles(*_db)
            );
    }
}
