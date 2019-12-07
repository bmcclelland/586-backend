#include "getprojects.h"

namespace mvc::endpoints
{
    GetProjects::GetProjects(Unique<IDatabase> db, GetProjects::PathArgs path)
        : _db(std::move(db))
        , _path(path)
    {
        println("GetProjects::GetProjects");
    }

    Requirements GetProjects::requirements() const
    {
        return make_reqs(Perm::view_project);
    }

    static
    Vec<ProjectNameID> get_projects(IDatabase& db)
    {
        auto const tx = db.transaction();
        return tx.collect<ProjectNameID>();
    }

    Option<EndpointOutput> GetProjects::call(EndpointInput const&)
    {
        return EndpointOutput(
            get_projects(*_db)
            );
    }
}
