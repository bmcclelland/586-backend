#pragma once

#include "util/json.h"
#include "util/string.h"

namespace mvc
{
    class Config
    {
        public:
            Config(String const& path);
            int get_int(String const& key) const;
            String get_string(String const& key) const;

        private:
            Json _json;
    };
}
