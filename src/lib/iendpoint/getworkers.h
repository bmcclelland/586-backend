#pragma once

#include "iendpoint.h"
#include "idatabase/idatabase.h"
#include "data/data.h"

namespace mvc::endpoints
{
    class GetWorkers : public IEndpoint
    {
        public:
            struct PathArgs {};

            GetWorkers(Unique<IDatabase>, PathArgs);
            Requirements requirements() const override;
            Option<EndpointOutput> call(EndpointInput const&) override;

        private:
            Unique<IDatabase> _db;
            PathArgs _path;
    };
}
