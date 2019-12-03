#include "http.h"
#include "util/string.h"
#include "util/containers.h"

namespace mvc
{
    Option<String> get_jwt(HttpRequest const&);
}
