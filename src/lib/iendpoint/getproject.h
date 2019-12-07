#pragma once

#include "iendpoint.h"
#include "idatabase/idatabase.h"
#include "data/data.h"

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
            
            GetProject(Unique<IDatabase>, PathArgs);
            Requirements requirements() const override;
            Option<EndpointOutput> call(EndpointInput const&) override;

        private:
            Unique<IDatabase> _db;
            PathArgs _path;
    };
}
