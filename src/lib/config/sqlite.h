#pragma once

#include "util/newtype.h"
#include "util/string.h"
#include "config.h"

namespace mvc
{
    NEW_TYPE(String, SqliteDbPath);
    
    class SqliteConfig
    {
        public:
            SqliteConfig(Config const&);
            SqliteDbPath db_path() const;

        private:
            SqliteDbPath _db_path;
    };
}
