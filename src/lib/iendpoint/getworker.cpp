#include "getworker.h"

namespace mvc::endpoints
{
    GetWorker::GetWorker(Unique<IDatabase> db, GetWorker::PathArgs path)
        : _db(std::move(db))
        , _path(path)
    {
        println(">GetWorker");
    }
    
    Requirements GetWorker::requirements() const
    {
        return make_reqs(Perm::view_worker);
    }

    Option<EndpointOutput> GetWorker::call(EndpointInput const&)
    {
        auto const tx = _db->transaction();

        Option<Worker> worker = tx.find_value(_path.worker_id);

        if (!worker)
        {
            return nullopt;
        }

        return EndpointOutput(*worker);
    }
}
