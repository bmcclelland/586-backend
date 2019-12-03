#pragma once

#include "iendpoint.h"
#include "util/containers.h"
#include "util/string.h"
#include "data/data.h"
#include "data/json.h"
#include "idatabase/idatabase.h"
#include <util/print.h>
    
namespace mvc::endpoints
{
    class AddProject : public IEndpoint
    {
        public:
            struct PathArgs {};

            AddProject(Unique<IDatabase> db, PathArgs path)
                : _db(std::move(db))
                , _path(path)
            {
                println("AddProject::AddProject");
            }

            Option<EndpointOutput> call(EndpointInput const& input) override
            {
                // TODO better..defined?
                ProjectName name;
                input.payload.at("name").get_to(name.val);
                return EndpointOutput(add_project(name));
            }
            
            Requirements requirements() const override
            {
                return make_reqs(Perm::create_project);
            }

            ProjectID add_project(ProjectName const& name)
            {
                return _db->transact<ProjectID>([&]
                {
                    Project p;
                    p.name = name.val;
                    return ProjectID{
                        _db->persist(p)
                    };
                });
            }

        private:
            Unique<IDatabase> _db;
            PathArgs _path;
    };
}
