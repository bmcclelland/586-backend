#include "addtask.h"
    
namespace mvc::endpoints
{
    AddTask::AddTask(Unique<IDatabase> db, PathArgs path)
        : _db(std::move(db))
        , _path(path)
    {
        println("AddTask::AddTask");
    }
    
    Requirements AddTask::requirements() const
    {
        return make_reqs(Perm::create_task);
    }

    Option<EndpointOutput> AddTask::call(EndpointInput const& input)
    {
        // TODO better..defined?
        TaskName name;
        ProjectID project_id;
        input.payload.at("name").get_to(name.val);
        input.payload.at("project_id").get_to(project_id.val);

        auto tx = _db->transaction();
        Shared<Project> project = tx.find_shared(project_id);
        
        if (!project)
        {
            return nullopt;
        }

        Task t(name, project);
        TaskID task_id{ tx.persist(t) };
        tx.commit();
        return EndpointOutput(task_id);
    }
}
