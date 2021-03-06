#pragma once

#include "iendpoint.h"
#include "data/data.h"
#include "idatabase/idatabase.h"

namespace mvc::endpoints
{
    class GetUsers : public IEndpoint
    {
        public:
            struct PathArgs {};

            GetUsers(Unique<IDatabase>, PathArgs);
            Requirements requirements() const override;
            Option<EndpointOutput> call(EndpointInput const&) override;
            Vec<User> get_users();

        private:
            Unique<IDatabase> _db;
            PathArgs _path;
    };
}
