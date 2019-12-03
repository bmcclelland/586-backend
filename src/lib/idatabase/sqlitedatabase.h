#pragma once

#include "idatabase.h"
#include "util/string.h"
#include "util/containers.h"
#include "util/print.h"
#include "config/sqlite.h"

namespace mvc
{
    class SqliteDatabase : public IDatabase
    {
        public:
            SqliteDatabase(SqliteConfig const&);
            odb::database& get() override;

        private:
            Unique<odb::database> _db;
    };
}
