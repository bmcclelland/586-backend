#include "http.h"

#include "app/error.h"
#include "util/newtype.h"
#include "util/string.h"
#include "util/print.h"
#include <cpprest/http_listener.h>
#include <cpprest/http_msg.h>

namespace mvc
{
    Json extract_req_json(HttpRequest& req)
    {
        try
        {
            pplx::task<web::json::value> body = req.extract_json();
            body.wait();
            std::stringstream stream;
            stream << body.get();
//            println("streamed ", stream.str());
            return Json::parse(stream.str());
        }
        catch (web::json::json_exception& e)
        {
            println("Failed to extract body: ", e.what());
            throw UserError("Invalid JSON input");
        }
        catch (Json::exception& e)
        {
            println("Failed to reparse body: ", e.what());
            throw UserError("Invalid JSON input");
        }
    }
    
    HttpResponse json_response(HttpStatusCode code, Json const& body)
    {
        HttpResponse rsp;
        rsp.set_status_code(code.val());
        rsp.set_body(body.dump(), "application/json; charset=utf-8");
        rsp.headers().add("Access-Control-Allow-Origin", "*");
        return rsp;
    }
        
    HttpResponse json_response_ok(HttpStatusCode code, Json const& body)
    {
        return json_response(code, Json{{"Ok", body}});
    }
    
    HttpResponse json_response_err(HttpStatusCode code, Json const& body)
    {
        return json_response(code, Json{{"Err", body}});
    }     //TODO
    
}
