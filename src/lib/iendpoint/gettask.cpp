#include "gettask.h"

namespace mvc::endpoints
{
    GetTask::GetTask(Unique<IDatabase> db, GetTask::PathArgs path)
        : _db(std::move(db))
        , _path(path)
    {
        println(">GetTask");
    }
    
    Requirements GetTask::requirements() const
    {
        return make_reqs(perms::read_tasks);
    }

    Option<EndpointOutput> GetTask::call(EndpointInput const&)
    {
        auto const tx = _db->transaction();
        using Q = odb::query<TaskDetails>;

        auto query = tx.query<TaskDetails>(
            Q::Task::id == _path.task_id.val
            );

        for (auto&& result : query)
        {
            return EndpointOutput(result);
        }

        return nullopt;
    }
}
