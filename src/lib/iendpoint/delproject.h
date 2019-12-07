#pragma once

#include "iendpoint.h"
#include "idatabase/idatabase.h"
#include "data/data.h"
    
namespace mvc::endpoints
{
    class DelProject : public IEndpoint
    {
        public:
            struct PathArgs {};

            DelProject(Unique<IDatabase>, PathArgs);
            Option<EndpointOutput> call(EndpointInput const&);
            Requirements requirements() const;

        private:
            Unique<IDatabase> _db;
            PathArgs _path;
    };
}
