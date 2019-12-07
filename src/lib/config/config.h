#pragma once

#include "util/json.h"
#include "util/string.h"
#include "util/newtype.h"

namespace mvc
{
    NEW_TYPE(String, ConfigPath);

    class Config
    {
        public:
            explicit Config(ConfigPath const& path);
            int get_int(String const& key) const;
            String get_string(String const& key) const;

        private:
            Json _json;
    };
}
