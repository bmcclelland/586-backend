#pragma once

#include "iendpoint.h"
#include "idatabase/idatabase.h"
#include "util/containers.h"
#include "util/string.h"
#include "data/data.h"
#include "idatabase/idatabase.h"

namespace mvc::endpoints
{
    class Debug : public IEndpoint
    {
        public:
            struct PathArgs {};

            Debug(Unique<IDatabase> db, PathArgs)
                : _db(std::move(db))
            {
                println("Debug::Debug");
            }
            
            Requirements requirements() const override
            {
                return make_reqs();
            }

            Option<EndpointOutput> call(EndpointInput const&) override
            {
                ProjectName name{
                    "test_project"
                };

                (void) _db->transact<ProjectID>([&]
                {
                    Project p;
                    p.name = name.val;
                    return ProjectID{
                        _db->persist(p)
                    };
                });
                
                return EndpointOutput("debug-ok");
            }

        private:
            Unique<IDatabase> _db;
    };
}
