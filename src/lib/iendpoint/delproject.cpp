#include "delproject.h"
    
namespace mvc::endpoints
{
    DelProject::DelProject(Unique<IDatabase> db, PathArgs path)
        : _db(std::move(db))
        , _path(path)
    {
        println(">DelProject");
    }
    
    Requirements DelProject::requirements() const
    {
        return make_reqs(Perm::delete_project);
    }

    Option<EndpointOutput> DelProject::call(EndpointInput const& input)
    {
        ProjectID id;
        input.payload.at("id").get_to(id.val);

        auto tx = _db->transaction();
        tx.erase(id);
        tx.commit();
        return EndpointOutput(Unit{});
    }
}
