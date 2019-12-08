#include "getproject.h"

namespace mvc::endpoints
{
    GetProject::GetProject(Unique<IDatabase> db, GetProject::PathArgs path)
        : _db(std::move(db))
        , _path(path)
    {
        println(">GetProject");
    }
    
    Requirements GetProject::requirements() const
    {
        return make_reqs(Perm::view_project);
    }

    Option<EndpointOutput> GetProject::call(EndpointInput const&)
    {
        auto const tx = _db->transaction();
        Option<Project> project = tx.find_value(_path.project_id);

        if (!project)
        {
            return nullopt;
        }

        return EndpointOutput(*project);
    }
}
