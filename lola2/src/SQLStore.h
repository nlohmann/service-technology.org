#pragma once

#include <sqlite3.h>
#include "Store.h"

class SQLStore : public Store
{
    private:
        sqlite3* db;

    public:
        SQLStore();
        ~SQLStore();
        bool searchAndInsert();
        bool searchAndInsert(State**);
};
