#include "getworkers.h"

namespace mvc::endpoints
{
    GetWorkers::GetWorkers(Unique<IDatabase> db, GetWorkers::PathArgs path)
        : _db(std::move(db))
        , _path(path)
    {
        println(">GetWorkers");
    }

    Requirements GetWorkers::requirements() const
    {
        return make_reqs(perms::read_workers);
    }

    Option<EndpointOutput> GetWorkers::call(EndpointInput const&)
    {
        auto const tx = _db->transaction();
        return EndpointOutput(
            tx.collect<WorkerNameID>()
            );
    }
}
