#include "unassigntask.h"
    
namespace mvc::endpoints
{
    UnassignTask::UnassignTask(Unique<IDatabase> db, PathArgs path)
        : _db(std::move(db))
        , _path(path)
    {
        println(">UnassignTask");
    }
    
    Requirements UnassignTask::requirements() const
    {
        return make_reqs(perms::modify_tasks);
    }

    Option<EndpointOutput> UnassignTask::call(EndpointInput const& input)
    {
        TaskID task_id;
        input.payload.at("task_id").get_to(task_id.val);
        
        auto tx = _db->transaction();
        Shared<Task> task = tx.find_shared(task_id);

        if (!task)
        {
            return nullopt;
        }
    
        task->worker = nullptr;
        tx.update(task);
        tx.commit();
        return EndpointOutput(Unit{});
    }
}
