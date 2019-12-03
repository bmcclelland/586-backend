#include "sqlite.h"

namespace mvc
{
    SqliteConfig::SqliteConfig(Config const& config)
        : _db_path(config.get_string("sqlite_db_path"))
    {}

    SqliteDbPath SqliteConfig::db_path() const
    {
        return _db_path;
    }
}
