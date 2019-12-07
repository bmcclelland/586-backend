#pragma once

#include "iendpoint.h"
#include "idatabase/idatabase.h"
#include "data/data.h"

namespace mvc::endpoints
{
    class GetWorker : public IEndpoint
    {
        public:
            struct PathArgs
            {
                PathArgs(int id)
                    : worker_id{id}
                {}

                WorkerID worker_id;
            };
            
            GetWorker(Unique<IDatabase>, PathArgs);
            Requirements requirements() const override;
            Option<EndpointOutput> call(EndpointInput const&) override;

        private:
            Unique<IDatabase> _db;
            PathArgs _path;
    };
}
