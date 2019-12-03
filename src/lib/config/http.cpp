#include "http.h"

namespace mvc
{
    HttpConfig::HttpConfig(Config const& config)
        : _listen_addr(config.get_string("http_listen_addr"))
    {}

    HttpListenAddr HttpConfig::listen_addr() const
    {
        return _listen_addr;
    }
}
