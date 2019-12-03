#pragma once

#include "iendpoint.h"
#include "util/containers.h"
#include "util/string.h"
#include "data/data.h"
#include "idatabase/idatabase.h"

namespace mvc::endpoints
{
    class GetProject : public IEndpoint
    {
        public:
            struct PathArgs
            {
                PathArgs(int id)
                    : project_id{id}
                {}

                ProjectID project_id;
            };
            
            GetProject(Unique<IDatabase> db, PathArgs path)
                : _db(std::move(db))
                , _path(path)
            {
                println("GetProject::GetProject");
            }
            
            Requirements requirements() const override
            {
                return make_reqs(Perm::view_project);
            }

            Option<EndpointOutput> call(EndpointInput const&) override
            {
                return EndpointOutput(get_project());
            }

            Option<Project> get_project()
            {
                return _db->find_value(_path.project_id);
            }

        private:
            Unique<IDatabase> _db;
            PathArgs _path;
    };
}
