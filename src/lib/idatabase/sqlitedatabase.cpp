#include "sqlitedatabase.h"

#include "idatabase.h"
#include "util/string.h"
#include "util/containers.h"
#include "util/print.h"
#include "config/sqlite.h"

namespace mvc
{
    static 
    Unique<odb::database> make_db(String const& path)
    {
        return std::make_unique<odb::sqlite::database>(
            path,
            SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE
        );
    }
    
    SqliteDatabase::SqliteDatabase(SqliteConfig const& config)
        : _db(make_db(config.db_path().val()))
    {
        println("SqliteDatabase::SqliteDatabase");

        odb::schema_version v (_db->schema_version());
        println("SqliteDatabase: Schema version = ", v);

        if (v == 0)
        {
            transact([&]
            {
                odb::schema_catalog::create_schema(*_db);
            });

            println("SqliteDatabase: Created schema");
        }
    }

    odb::database& SqliteDatabase::get()
    {
        return *_db;
    }
}
