#include "server/apipath.h"

namespace mvc {

using namespace web::http;

static
Vec<String> split_request_path(HttpRequest const& request)
{
    return uri::split_path(
        uri::decode(request.relative_uri().path())
        );
}

static
Vec<ApiSegment> split_path(HttpRequest const& request)
{
#ifdef _WIN32
    static_assert(false, "WIN32");
//    auto path16 = uri::split_path(
//        uri::decode(request.relative_uri().path())
//        );
//    
//    Vec<ApiSegment> path;
//    
//    for (auto const& s : path16) 
//    {
//        path.push_back(from_wide(s));
//    }
//    
//    return path;
#else
    Vec<ApiSegment> segments;

    for (String& s : split_request_path(request))
    {
        segments.push_back(ApiSegment(std::move(s)));
    }

    return segments;
#endif
}

ApiPath::ApiPath(HttpRequest const& request)
    : _segments(split_path(request))
{}

ApiPath::ApiPath(std::initializer_list<ApiSegment> const& segments)
    : _segments(segments)
{}
        
size_t ApiPath::size() const
{
    return _segments.size();
}

ApiPath::Iter ApiPath::begin() const
{
    return _segments.begin();
}

ApiPath::Iter ApiPath::end() const
{
    return _segments.end();
}

} // namespace mvc
