/*!
\file Handlers.cc
\author Niels
\status new

\ingroup g_frontend

\brief Implementation of the frontend error reporting function.
*/

#include <config.h>
#include <libgen.h>
#include <cstdarg>
#include <cstdio>
#include <Parser/error.h>
#include <InputOutput/Reporter.h>
#include <InputOutput/InputOutput.h>

extern Input* netFile;

/*!
This generic error reporting function is used in all lexers and parsers. It
displays an error message together with the location of the error token. If the
input was read from a file, the error is further printed with some context to
help the user locate and better understand the error message.

\param[in] token   The token that was last read. Hopefully close to the error.
\param[in] loc     The location of the read token in the error file.
\param[in] format  An error message in printf format.

\note Assumes that #netFile is the file that causes this error. If #netFile is
NULL, the parser was called from a string (command-line parameter) and no
detailed error location can be printed.
\post Errors with EXIT_ERROR.

\ingroup g_frontend
*/
void yyerrors(const char* token, YYLTYPE loc, const char* format, ...) __attribute__((noreturn));
void yyerrors(const char* token, YYLTYPE loc, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    char* errormessage = NULL;
    const int res = vasprintf(&errormessage, format, args);
    assert(res != -1);
    rep->status(errormessage);
    free(errormessage);
    va_end(args);

    // only print filename and excerpt if we read from a file/stdin
    if (netFile)
    {
        rep->status("%s:%d:%d - error near '%s'",
            rep->markup(MARKUP_FILE, basename((char*)netFile->getFilename())).str(),
            loc.first_line, loc.first_column, token);

        netFile->printExcerpt(loc.first_line, loc.first_column,
                              loc.last_line, loc.last_column);
    }
    else
    {
        rep->status("%d:%d - error near '%s'",
            loc.first_line, loc.first_column, token);
    }

    rep->abort(ERROR_SYNTAX);
    exit(EXIT_ERROR); // needed to corrently recognize noreturn since rep->abort is virtual
}
