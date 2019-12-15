#include "httpclient.h"
#include "http.h"
#include <cpprest/http_client.h>
#include "util/print.h"

namespace mvc
{
    Option<String> http_req(String const& url, Option<String> const& token)
    {
        using HttpClient = web::http::client::http_client;

        HttpRequest req;
        req.set_request_uri("/");
        
        if (token)
        {
            req.headers().add("Authorization", "Bearer " + *token);
        }

        HttpClient client(
            web::uri::encode_uri(url)
            );

        pplx::task req_task = 
            client.request(req)
            .then([=](HttpResponse response) {
//                println(url, " -> ", response.status_code());
                return response.extract_string();
            });

        try {
            return req_task.get();
        }
        catch (std::exception const& e) {
            println("FAILURE TO GET ", url, ": ", e.what());
            return nullopt;
        }
    }
}
