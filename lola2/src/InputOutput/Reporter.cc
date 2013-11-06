/*!
\file Reporter.cc
\author Niels
\status approved 25.01.2012
*/

#include <config.h>
#include <unistd.h>
#include <cstdio>
#include <cstdarg>
#include <cstdlib>
#include <cerrno>
#include <cstring>
#include <Core/Dimensions.h>
#include <InputOutput/Reporter.h>


// initialize error messages
const char *Reporter::error_messages[] =
{
    "syntax error",
    "command line error",
    "file input/output error",
    "network error",
    "thread error"
};


/*---------------------------------------------------------------------------*/

/*!
\param port     the port to use
\param[in] ip   the target IP address
\param verbose  whether to display verbose messages
*/
ReporterSocket::ReporterSocket(u_short port, const char *ip, bool verbose)
    : verbose(verbose), mySocket(Socket(port, ip))
{
}

/*!
\param[in] format  the status message formatted as printf string
*/
void ReporterSocket::message(const char *format, ...) const
{
    char buffer[UDP_BUFFER_SIZE];
    va_list args;
    va_start(args, format);
    vsprintf(buffer, format, args);
    mySocket.send(buffer);
    va_end(args);
}

/*!
\param[in] format  the status message formatted as printf string
\post The message is not printed unless the #verbose member is true.
*/
void ReporterSocket::status(const char *format, ...) const
{
    if (not verbose)
    {
        return;
    }

    char buffer[UDP_BUFFER_SIZE];
    va_list args;
    va_start(args, format);
    vsprintf(buffer, format, args);
    mySocket.send(buffer);
    va_end(args);
}

/*!
\param code    the error code
\param format  the error message formatted as printf string
\todo Handle premature termination with signals.
*/
__attribute__((noreturn)) void ReporterSocket::abort(errorcode_t code) const
{
    char buffer[UDP_BUFFER_SIZE];
    sprintf(buffer, "%s: %s -- aborting [#%02d]", PACKAGE, error_messages[code], code);
    mySocket.send(buffer);

    status("see manual for a documentation of this error");

    if (errno != 0)
    {
        status("last error message: %s", strerror(errno));
    }

    exit(EXIT_ERROR);
}

ReporterSocket::~ReporterSocket()
{
    status("done");
}

/*!
\param markup  how to markup the string (ignored here)
\param[in] format  the string to format
\post Passed string format is formatted according to markup.
\note Memory for res is released by Reporter::~String().
*/
String ReporterSocket::markup(markup_t markup, const char *format, ...) const
{
    va_list args;
    va_start(args, format);
    char *res = NULL;
    const int r = vasprintf(&res, format, args);
    assert(r != -1);
    va_end(args);
    return String(res);
}

/*---------------------------------------------------------------------------*/

/*!
\param verbose  whether to display verbose messages
\post All member variables for colors are initialited according to the value
      of #useColor.
*/
ReporterStream::ReporterStream(bool verbose) :
    verbose(verbose),
#if !defined(__MINGW32__)
    useColor(isatty(fileno(stderr))  &&(
                 !strcmp(getenv("TERM"), "linux") ||
                 !strcmp(getenv("TERM"), "cygwin") ||
                 !strcmp(getenv("TERM"), "xterm") ||
                 !strcmp(getenv("TERM"), "xterm-color") ||
                 !strcmp(getenv("TERM"), "xterm-256color"))),
#else
    useColor(false),
#endif
    _cr_(useColor ? "\033[0;31m" : ""),
    _cg_(useColor ? "\033[0;32m" : ""),
    _cy_(useColor ? "\033[0;33m" : ""),
    _cb_(useColor ? "\033[0;34m" : ""),
    _cm_(useColor ? "\033[0;35m" : ""),
    _cc_(useColor ? "\033[0;36m" : ""),
    _cl_(useColor ? "\033[0;37m" : ""),
    _cr__(useColor ? "\033[0;4;31m" : ""),
    _cg__(useColor ? "\033[0;4;32m" : ""),
    _cy__(useColor ? "\033[0;4;33m" : ""),
    _cb__(useColor ? "\033[0;4;34m" : ""),
    _cm__(useColor ? "\033[0;4;35m" : ""),
    _cc__(useColor ? "\033[0;4;36m" : ""),
    _cl__(useColor ? "\033[0;4;37m" : ""),
    _cR_(useColor ? "\033[0;1;31m" : ""),
    _cG_(useColor ? "\033[0;1;32m" : ""),
    _cY_(useColor ? "\033[0;1;33m" : ""),
    _cB_(useColor ? "\033[0;1;34m" : ""),
    _cM_(useColor ? "\033[0;1;35m" : ""),
    _cC_(useColor ? "\033[0;1;36m" : ""),
    _cL_(useColor ? "\033[0;1;37m" : ""),
    _c_(useColor ? "\033[0m" : ""),
    _bold_(useColor ? "\033[1m" : ""),
    _underline_(useColor ? "\033[4m" : "")
{
}


/*!
\param[in] format  the status message formatted as printf string
*/
void ReporterStream::message(const char *format, ...) const
{
    fprintf(stderr, "%s: ", markup(MARKUP_TOOL, PACKAGE).str());

    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);

    fprintf(stderr, "\n");
}


/*!
\param[in] format  the status message formatted as printf string
\post The message is not printed unless the #verbose member is true.
*/
void ReporterStream::status(const char *format, ...) const
{
    if (not verbose)
    {
        return;
    }

    fprintf(stderr, "%s: ", markup(MARKUP_TOOL, PACKAGE).str());

    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);

    fprintf(stderr, "\n");
}


/*!
\param code    the error code
\todo Handle premature termination with signals.
*/
__attribute__((noreturn)) void ReporterStream::abort(errorcode_t code) const
{
    fprintf(stderr, "%s: %s%s%s%s -- aborting [#%02d]%s\n",
            markup(MARKUP_TOOL, PACKAGE).str(), _cR_, error_messages[code], _c_, _bold_, code, _c_);

    status("see manual for a documentation of this error");

    if (errno != 0)
    {
        status("last error message: %s", markup(MARKUP_IMPORTANT, strerror(errno)).str());
    }

    exit(EXIT_ERROR);
}

/*!
\param markup  how to markup the string
\param[in] format  the string to format
\post Passed string format is formatted according to markup.
\note Memory for res is released by ~String::String().
*/
String ReporterStream::markup(markup_t markup, const char *format, ...) const
{
    va_list args;
    va_start(args, format);
    char *message = NULL;
    const int r = vasprintf(&message, format, args);
    assert(r != -1);
    va_end(args);

    char *res = NULL;
    int bytes = -1;

    switch (markup)
    {
    case (MARKUP_TOOL):
        bytes = asprintf(&res, "%s%s%s", _cm_, message, _c_);
        break;

    case (MARKUP_FILE):
        bytes = asprintf(&res, "%s%s%s", _cb__, message, _c_);
        break;

    case (MARKUP_OUTPUT):
        bytes = asprintf(&res, "%s%s%s", _cB_, message, _c_);
        break;

    case (MARKUP_GOOD):
        bytes = asprintf(&res, "%s%s%s", _cG_, message, _c_);
        break;

    case (MARKUP_BAD):
        bytes = asprintf(&res, "%s%s%s", _cR_, message, _c_);
        break;

    case (MARKUP_WARNING):
        bytes = asprintf(&res, "%s%s%s", _cY_, message, _c_);
        break;

    case (MARKUP_IMPORTANT):
        bytes = asprintf(&res, "%s%s%s", _bold_, message, _c_);
        break;

    case (MARKUP_PARAMETER):
        bytes = asprintf(&res, "%s%s%s", _cC_, message, _c_);
        break;

    case (MARKUP_UNIMPORTANT):
        bytes = asprintf(&res, "%s%s%s", _cl_, message, _c_);
        break;
    }

    assert(bytes != -1);
    assert(res);
    free(message);

    return String(res);
}
