#include "getusers.h"

namespace mvc::endpoints
{
    GetUsers::GetUsers(Unique<IDatabase> db, PathArgs path)
        : _db(std::move(db))
        , _path(path)
    {
        println(">GetUsers");
    }

    Requirements GetUsers::requirements() const
    {
        return make_reqs();
    }

    Option<EndpointOutput> GetUsers::call(EndpointInput const&)
    {
        auto const tx = _db->transaction();
        return EndpointOutput(
            tx.collect<User>()
            );
    }
}
