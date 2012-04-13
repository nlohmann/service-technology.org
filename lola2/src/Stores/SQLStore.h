/*!
\author Niels
\file SQLStore.h
\status new
*/

#pragma once
#include <sqlite3.h>
#include "Stores/Store.h"


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
