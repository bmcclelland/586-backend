#pragma once

#include "iendpoint.h"
#include "idatabase/idatabase.h"
#include "util/containers.h"
#include "util/string.h"
#include "data/data.h"
#include "idatabase/idatabase.h"

namespace mvc::endpoints
{
    class GetProjects : public IEndpoint
    {
        public:
            struct PathArgs {};

            GetProjects(Unique<IDatabase> db, PathArgs path)
                : _db(std::move(db))
                , _path(path)
            {
                println("GetProjects::GetProjects");
            }

            Requirements requirements() const override
            {
                return make_reqs(Perm::view_project);
            }

            Option<EndpointOutput> call(EndpointInput const&) override
            {
                return EndpointOutput(get_projects());
            }

            Vec<Project> get_projects()
            {
                return _db->collect<Project>();
            }

        private:
            Unique<IDatabase> _db;
            PathArgs _path;
    };
}
