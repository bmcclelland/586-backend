#include "addworker.h"
    
namespace mvc::endpoints
{
    AddWorker::AddWorker(Unique<IDatabase> db, PathArgs path)
        : _db(std::move(db))
        , _path(path)
    {
        println(">AddWorker");
    }
    
    Requirements AddWorker::requirements() const
    {
        return make_reqs(Perm::create_worker);
    }

    Option<EndpointOutput> AddWorker::call(EndpointInput const& input)
    {
        WorkerName name;
        input.payload.at("name").get_to(name.val);

        auto tx = _db->transaction();
        Worker w(name);
        WorkerID id{ tx.persist(w) };
        tx.commit();
        return EndpointOutput(id);
    }
}
