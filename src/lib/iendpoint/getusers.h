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

            GetUsers(Unique<IDatabase> db, PathArgs path)
                : _db(std::move(db))
                , _path(path)
            {
                println("GetUsers::GetUsers");
            }

            Requirements requirements() const override
            {
                return make_reqs();
            }

            Option<EndpointOutput> call(EndpointInput const&) override
            {
                return EndpointOutput(get_users());
            }

            Vec<User> get_users()
            {
                return _db->collect<User>();
            }

        private:
            Unique<IDatabase> _db;
            PathArgs _path;
    };
}
