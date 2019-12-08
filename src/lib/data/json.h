#pragma once

#include "util/containers.h"
#include "util/string.h"
#include "data/data.h"

namespace mvc 
{
    struct Unit {};

    // TODO
    template <typename T>
    Vec<T> realize(Vec<LazyWeak<T>> const& xs)
    {
        Vec<T> y;
        y.reserve(xs.size());

        for (auto&& x : xs)
        {
            y.push_back(*x.load());
        }

        return y;
    }
   
    template <typename T>
    void to_json(Json& j, Shared<T> const& x)
    {
        return to_json(j, *x);
    }

    template <typename T>
    void to_json(Json& j, Option<T> const& x)
    {
        if (x)
        {
            j = *x;
        }
        else
        {
            j = nullptr;
        }
    }

    inline
    void to_json(Json& j, ProjectName const& x)
    {
        j = x.val;
    }
    
    inline
    void to_json(Json& j, ProjectID const& x)
    {
        j = x.val;
    }
    
    inline
    void to_json(Json& j, TaskName const& x)
    {
        j = x.val;
    }
    
    inline
    void to_json(Json& j, TaskID const& x)
    {
        j = x.val;
    }
    
    inline
    void to_json(Json& j, WorkerName const& x)
    {
        j = x.val;
    }
    
    inline
    void to_json(Json& j, WorkerID const& x)
    {
        j = x.val;
    }

    // TODO
    inline
    void to_json(Json& j, Task const& x)
    {
        x.project.load();
        x.worker.load();

        j = Json{
            {"id", x.id},
            {"name", x.name},
            {"project", x.project->id},
        };

        if (x.worker)
        {
            j["worker"] = x.worker->id;
        }
        else
        {
            j["worker"] = nullptr;
        }
    }

    inline
    void to_json(Json& j, Role const& x)
    {
        j = Json{
            {"id",       x.name},
            {"perms",    x.perms}
        };
    }

    inline
    void to_json(Json& j, User const& x)
    {
        j = Json{
            {"id",       x.id},
            {"roles",    x.roles}
        };
    }

    inline
    void to_json(Json& j, Project const& x) 
    {
        j = Json{
            {"id",    x.id},
            {"name",  x.name},
            {"tasks", realize(x.tasks)}
        };
    }
    
    inline
    void to_json(Json& j, Worker const& x) 
    {
        j = Json{
            {"id",    x.id},
            {"name",  x.name},
            {"tasks", realize(x.tasks)}
        };
    }
    
    inline
    void to_json(Json& j, ProjectNameID const& x) 
    {
        j = Json{
            {"id",    x.id},
            {"name",  x.name},
        };
    }
    
    inline
    void to_json(Json& j, WorkerNameID const& x) 
    {
        j = Json{
            {"id",    x.id},
            {"name",  x.name},
        };
    }
    
    inline
    void to_json(Json& j, TaskDetails const& x) 
    {
        Json project{
            {"name", x.project_name},
            {"id",   x.project_id}
        };

        Json worker;

        if (x.worker_id && x.worker_name)
        {
            worker = Json{
                {"name", *x.worker_name},
                {"id",   *x.worker_id}
            };
        }
        
        j = Json{
            {"id",    x.id},
            {"name",  x.name},
            {"project", project },
            {"worker", worker }
        };
    }

    inline
    void to_json(Json& j, Unit)
    {
        j = Json{
            {"()", nullptr}
        };
    }

} // namespace ns
