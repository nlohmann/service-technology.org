/*!
\file Reporter.cc
\status approved 25.01.2012
*/

#include <config.h>
#include <cstdio>
#include <cstdarg>
#include <cstdlib>
#include <cerrno>
#include <cstring>
#include <string>

#include "Reporter.h"
#include "Dimensions.h"

ReporterSocket::ReporterSocket(u_short port, const char* ip)
    : mySocket(Socket(port, ip))
{
}



void ReporterSocket::message(const char* format, ...)
{
    char buffer[UDP_BUFFER_SIZE];
    va_list args;
    va_start(args, format);
    vsprintf(buffer, format, args);
    mySocket.send(buffer);
    va_end(args);
}

void ReporterSocket::status(const char* format, ...)
{
    char buffer[UDP_BUFFER_SIZE];
    va_list args;
    va_start(args, format);
    vsprintf(buffer, format, args);
    mySocket.send(buffer);
    va_end(args);
}

__attribute__((noreturn)) void ReporterSocket::abort(uint8_t code, const char* format, ...)
{
    char buffer[UDP_BUFFER_SIZE];
    va_list args;
    va_start(args, format);
    vsprintf(buffer, format, args);
    mySocket.send(buffer);
    va_end(args);

    exit(EXIT_FAILURE);
}

ReporterSocket::~ReporterSocket()
{
    status("done");
}




ReporterStream::ReporterStream() :
#if !defined(__MINGW32__)
    useColor(isatty(fileno(stderr)) && (
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


const char* ReporterStream::_ctool_(const char* s)
{
    return (std::string(_cm_) + s + _c_).c_str();
}

const char* ReporterStream::_cfilename_(const char* s)
{
    return (std::string(_cb__) + s + _c_).c_str();
}


const char* ReporterStream::_coutput_(const char* s)
{
    return (std::string(_cB_) + s + _c_).c_str();
}


const char* ReporterStream::_cgood_(const char* s)
{
    return (std::string(_cG_) + s + _c_).c_str();
}


const char* ReporterStream::_cbad_(const char* s)
{
    return (std::string(_cR_) + s + _c_).c_str();
}


const char* ReporterStream::_cwarning_(const char* s)
{
    return (std::string(_cY_) + s + _c_).c_str();
}


const char* ReporterStream::_cimportant_(const char* s)
{
    return (std::string(_bold_) + s + _c_).c_str();
}


const char* ReporterStream::_cparameter_(const char* s)
{
    return (std::string(_cC_) + s + _c_).c_str();
}





/*!
 \param format  the status message formatted as printf string
*/
void ReporterStream::message(const char* format, ...)
{
    fprintf(stderr, "%s: ", _ctool_(PACKAGE));

    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);

    fprintf(stderr, "\n");
}

/*!
 \param format  the status message formatted as printf string
*/
void ReporterStream::status(const char* format, ...)
{
    fprintf(stderr, "%s: ", _ctool_(PACKAGE));

    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);

    fprintf(stderr, "\n");
}


/*!
 \param code    the error code
 \param format  the error message formatted as printf string

 \note The codes should be documented in the manual.
*/
__attribute__((noreturn)) void ReporterStream::abort(uint8_t code, const char* format, ...)
{
    fprintf(stderr, "%s: %s", _ctool_(PACKAGE), _bold_);

    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);

    fprintf(stderr, "%s -- %saborting [#%02d]%s\n", _c_, _cR_, code, _c_);

    status("see manual for a documentation of this error");
    if (errno != 0)
    {
        status("last error message: %s", strerror(errno));
    }

    exit(EXIT_FAILURE);
}

ReporterStream::~ReporterStream()
{
    status("done");
}
