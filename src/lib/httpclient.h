#pragma once

#include "util/containers.h"
#include "util/string.h"

namespace mvc
{
    Option<String> http_req(String const& url, Option<String> const& bearer);
}
