#pragma once

#include "iendpoint.h"
#include "idatabase/idatabase.h"
#include "data/data.h"

namespace mvc::endpoints
{
    class GetTask : public IEndpoint
    {
        public:
            struct PathArgs
            {
                PathArgs(int id)
                    : task_id{id}
                {}

                TaskID task_id;
            };
            
            GetTask(Unique<IDatabase>, PathArgs);
            Requirements requirements() const override;
            Option<EndpointOutput> call(EndpointInput const&) override;

        private:
            Unique<IDatabase> _db;
            PathArgs _path;
    };
}
