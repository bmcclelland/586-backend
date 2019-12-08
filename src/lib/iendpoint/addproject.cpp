#include "addproject.h"
    
namespace mvc::endpoints
{
    AddProject::AddProject(Unique<IDatabase> db, PathArgs path)
        : _db(std::move(db))
        , _path(path)
    {
        println(">AddProject");
    }
    
    Requirements AddProject::requirements() const
    {
        return make_reqs(Perm::create_project);
    }

    Option<EndpointOutput> AddProject::call(EndpointInput const& input)
    {
        ProjectName name;
        input.payload.at("name").get_to(name.val);
        
        auto tx = _db->transaction();
        Project p(name);
        ProjectID id{ tx.persist(p) };
        tx.commit();
        return EndpointOutput(id);
    }
}
