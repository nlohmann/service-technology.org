#include <string>
#include <cstdio>
#include <cstdlib>
#include <sstream>

#include "SQLStore.h"
#include "Net.h"
#include "Place.h"
#include "Marking.h"
#include "Reporter.h"

extern Reporter* rep;

static int callback(void* NotUsed, int argc, char** argv, char** azColName)
{
    return 0;
}

SQLStore::SQLStore()
{
    remove("lola.sqlite");

    int res = sqlite3_open("lola.sqlite", &db);
    if (UNLIKELY(res != SQLITE_OK))
    {
        rep->status("could not create data base %s", rep->markup(MARKUP_FILE, "lola.sqlite").str());
        rep->abort(ERROR_FILE);
    }

    std::ostringstream s;
    s << Net::Card[PL];

    std::string statement = "create table store (m varchar(" + s.str() + "))";
    char* zErrMsg = NULL;
    res = sqlite3_exec(db, statement.c_str(),  callback, 0, &zErrMsg);
    if (UNLIKELY(res != SQLITE_OK))
    {
        rep->status("error executing query %s", rep->markup(MARKUP_PARAMETER, statement.c_str()).str());
        rep->status("SQL error: %s", zErrMsg);
        sqlite3_free(zErrMsg);
        rep->abort(ERROR_FILE);
    }

    rep->status("created and set up data base %s", rep->markup(MARKUP_FILE, "lola.sqlite").str());
}

SQLStore::~SQLStore()
{
    int res = sqlite3_close(db);
    if (UNLIKELY(res != SQLITE_OK))
    {
        rep->status("could not close data base %s", rep->markup(MARKUP_FILE, "lola.sqlite").str());
        rep->abort(ERROR_FILE);
    }

    res = remove("lola.sqlite");
    if (UNLIKELY(res != 0))
    {
        rep->status("could not delete %s", rep->markup(MARKUP_FILE, "lola.sqlite").str());
        rep->abort(ERROR_FILE);
    }

    rep->status("closed and deleted data base");
}

bool SQLStore::searchAndInsert()
{
    ++calls;

    std::string m = "";
    for (index_t p = 0; p < Net::Card[PL]; ++p)
    {
        std::ostringstream s;
        s << Marking::Current[p];
        m += s.str();
    }

    bool seen_marking = false;

    std::string statement = "select * from store where m = '" + m + "'";
    char* zErrMsg = NULL;
    int res = sqlite3_exec(db, statement.c_str(),  callback, 0, &zErrMsg);
    if (UNLIKELY(res != SQLITE_OK))
    {
        rep->status("error executing query %s", rep->markup(MARKUP_PARAMETER, statement.c_str()).str());
        rep->status("SQL error: %s", zErrMsg);
        sqlite3_free(zErrMsg);
        rep->abort(ERROR_FILE);
    }

    if (seen_marking)
    {
        return true;
    }

    ++markings;

    statement = "insert into store (m) values ('" + m + "')";
    res = sqlite3_exec(db, statement.c_str(),  callback, 0, &zErrMsg);
    if (UNLIKELY(res != SQLITE_OK))
    {
        rep->status("error executing query %s", rep->markup(MARKUP_PARAMETER, statement.c_str()).str());
        rep->status("SQL error: %s", zErrMsg);
        sqlite3_free(zErrMsg);
        rep->abort(ERROR_FILE);
    }

    return false;
}

bool SQLStore::searchAndInsert(State**)
{
    assert(false);
    return false;
}
