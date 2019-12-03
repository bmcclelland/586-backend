#include "apirouter.h"
#include "server/apiparse.h"
#include "iendpoint/addproject.h"
#include "iendpoint/getproject.h"
#include "iendpoint/getprojects.h"
#include "iendpoint/getusers.h"
#include "iendpoint/debug.h"
#include "util/print.h"
#include "app/error.h"

namespace mvc
{
    namespace
    {
        using namespace mvc::api_parsers;
        using namespace mvc::endpoints;
              
        using Routes = Dyn<IEndpoint,
            Endpoint<GetProjects, Lit("get_projects")>,
            Endpoint<GetProject,  Lit("get_project"), Int>,
            Endpoint<AddProject,  Lit("add_project")>,
            Endpoint<GetUsers,    Lit("get_users")>,
            Endpoint<Debug,       Lit("debug")>
            >;

        using Api = Root<Lit("api"), Routes>;
    }

    auto ApiRouter::route(ApiPath const& path, EndpointInput const& input)
        -> Option<EndpointOutput>
    {
        println("ApiRouter::ApiRouter");
        static Api const api;

        Option<Shared<IEndpoint>> result = api.parse(path);

        if (result)
        {
            IEndpoint* endpoint = result->get();
            
            if (!endpoint->requirements().met_by(input.actor))
            {
                throw AuthError("nope");
            }

            return endpoint->call(input);
        }
        else
        {
            throw UserError("Bad route");
        }
    }
}
