#include "gettask.h"

namespace mvc::endpoints
{
    GetTask::GetTask(Unique<IDatabase> db, GetTask::PathArgs path)
        : _db(std::move(db))
        , _path(path)
    {
        println("GetTask::GetTask");
    }
    
    Requirements GetTask::requirements() const
    {
        return make_reqs(Perm::view_task);
    }

    Option<EndpointOutput> GetTask::call(EndpointInput const&)
    {
        auto const tx = _db->transaction();
        Option<Task> task = tx.find_value(_path.task_id);

        if (!task)
        {
            return nullopt;
        }

        return EndpointOutput(*task);
    }
}
