#include "httphandler.h"
#include "getjwt.h"
#include "app/error.h"
#include "util/jwt.h"
#include <cctype>

namespace mvc
{
    static
    Option<Actor> verify_auth(HttpRequest const& req, Authenticator const& authenticator)
    {
        Option<String> token = get_jwt(req);

        if (token)
        {
            return authenticator.auth(*token);
        }
        else
        {
            println("No auth token");
            return nullopt;
        }
    }

    HttpHandler::HttpHandler(
        Unique<IRouter> router, 
        Unique<Authenticator> authenticator
        )
        : _router(std::move(router))
        , _authenticator(std::move(authenticator))
    {
        println("HttpHandler::HttpHandler");
    }

    HttpResponse HttpHandler::handle(HttpRequest& req)
    {
        println("Httphandler::handle ",
            req.method(),
            " ",
            req.relative_uri().to_string()
        );

            
           if (req.method() == HttpMethods::OPTIONS)
        {
            HttpResponse rsp;
            rsp.set_status_code(HttpStatusCode(200));
            rsp.set_body("", "text/plain");
            rsp.headers().add("Access-Control-Allow-Origin", "*");
            rsp.headers().add("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
            rsp.headers().add("Access-Control-Allow-Headers", "Content-Type, Authorization");
            return rsp;
        }
        else
        {
            try
            {
                Option<Actor> const actor = verify_auth(req, *_authenticator);

                if (actor)
                {
                    println("Actor: ", actor->unique_id());
                }

                ApiPath const path(req);
                EndpointInput input(Json{}, actor);

                if (req.method() == HttpMethods::POST)
                {
                    input.payload = extract_req_json(req);
                    println("Input: ", input.payload.dump());
                }

                Option<EndpointOutput> result = _router->route(path, input);

                if (result)
                {
                    return json_response_ok(HttpStatusCode(200), result->payload);
                }
                else
                {
                    println("EndpointOutput = nullopt");
                    return json_response_err(HttpStatusCode(200), Json{":("});
                }
            }
            catch (UserError& ex)
            {
                println("UserError: ", ex.what());
                return json_response_err(HttpStatusCode(400), Json{ex.what()});
            }
            catch (AuthError& ex)
            {
                println("AuthError: ", ex.what());
                return json_response_err(HttpStatusCode(400), Json{ex.what()});
            }
            catch (std::exception& ex) // TODO granularity
            {
                println("Exception: ", ex.what());
                return json_response_err(HttpStatusCode(500), Json{});
            }
        }
    }
}
