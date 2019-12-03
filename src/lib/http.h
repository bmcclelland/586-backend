#pragma once

#include "util/newtype.h"
#include "util/string.h"
#include "util/json.h"
#include <cpprest/http_listener.h>
#include <cpprest/http_msg.h>

namespace mvc
{
    using HttpRequest  = web::http::http_request;
    using HttpResponse = web::http::http_response;
    using HttpMethod   = web::http::method;
    using HttpMethods  = web::http::methods;

    NEW_TYPE(unsigned short, HttpStatusCode);
    
    Json extract_req_json(HttpRequest&);
    HttpResponse json_response(HttpStatusCode, Json const&);
    HttpResponse json_response_ok(HttpStatusCode, Json const&);
    HttpResponse json_response_err(HttpStatusCode, Json const&);
}
