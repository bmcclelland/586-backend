#include "assigntask.h"
    
namespace mvc::endpoints
{
    AssignTask::AssignTask(Unique<IDatabase> db, PathArgs path)
        : _db(std::move(db))
        , _path(path)
    {
        println(">AssignTask");
    }
    
    Requirements AssignTask::requirements() const
    {
        return make_reqs(perms::modify_tasks);
    }

    Option<EndpointOutput> AssignTask::call(EndpointInput const& input)
    {
        TaskID task_id;
        WorkerID worker_id;
        input.payload.at("task_id").get_to(task_id.val);
        input.payload.at("worker_id").get_to(worker_id.val);
        
        auto tx = _db->transaction();
        Shared<Task>   task   = tx.find_shared(task_id);
        Shared<Worker> worker = tx.find_shared(worker_id);

        if (!task || !worker)
        {
            return nullopt;
        }
        
        task->worker = worker;
        tx.update(task);
        tx.commit();
        return EndpointOutput(Unit{});
    }
}
