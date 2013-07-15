#include <config.h>

#include <cstdio>
#include <cstdarg>
#include <cstdlib>
#include <iostream>
#include <ctime>
#include <unistd.h>
#include <new>
#include <csignal>
#include <cerrno>

#include "Runtime.h"

Runtime *Runtime::_rt = NULL;

#if !defined(__MINGW32__) && !defined(USE_SYSLOG)
bool Runtime::_useColor = isatty(fileno(stderr)) && (
    !strcmp(getenv("TERM"), "linux") ||
    !strcmp(getenv("TERM"), "cygwin") ||
    !strcmp(getenv("TERM"), "xterm") ||
    !strcmp(getenv("TERM"), "xterm-color") ||
    !strcmp(getenv("TERM"), "xterm-256color"));
#else
bool Runtime::_useColor = false;
#endif

//std::string Runtime::_red     = (_useColor ? "\033[0;31m" : "");
//std::string Runtime::_green   = (_useColor ? "\033[0;32m" : "");
//std::string Runtime::_yellow  = (_useColor ? "\033[0;33m" : "");
//std::string Runtime::_blue    = (_useColor ? "\033[0;34m" : "");
//std::string Runtime::_magenta = (_useColor ? "\033[0;35m" : "");
//std::string Runtime::_cyan    = (_useColor ? "\033[0;36m" : "");
//std::string Runtime::_grey    = (_useColor ? "\033[0;37m" : "");


Runtime::Runtime() {
    _rt = this;

    // listen to segmentation violation (SIGSEGV)
    signal(SIGSEGV, Runtime::signalHandler);
    // listen to software termination signal (kill)
    signal(SIGTERM, Runtime::signalHandler);
    // listen to interrupt program (ctrl+c)
    signal(SIGINT, Runtime::signalHandler);
    // listen to user-defined signal 1 (used for remote termination)
    signal(SIGUSR1, Runtime::signalHandler);
    // listen to user-defined signal 2 (used for self termination)
    signal(SIGUSR2, Runtime::signalHandler);

    // install new handler
    std::set_new_handler(Runtime::newHandler);

    // set the function to call on normal termination
    std::atexit(Runtime::exitHandler);

    // store the timestamp of the call
    time(&start_time);

    // initialize log
#ifdef CONFIG_HOSTNAME
    _log["hostname"] = CONFIG_HOSTNAME;
#endif
#ifdef PACKAGE_NAME
    _log["tool"]["name"] = PACKAGE_NAME;
#endif
#ifdef PACKAGE_VERSION
    _log["tool"]["package_version"] = PACKAGE_VERSION;
#endif
#ifdef VERSION_SVN
    _log["tool"]["svn_version"] = VERSION_SVN;
#endif
#ifdef CONFIG_BUILDSYSTEM
    _log["tool"]["build_system"] = CONFIG_BUILDSYSTEM;
#endif
    _log["tool"]["architecture"] = static_cast<int>(sizeof(void*) * 8);
#ifdef __cplusplus
    _log["tool"]["cpp_standard"] = static_cast<int>(__cplusplus);
#endif
#ifdef __VERSION__
    _log["tool"]["compiler_version"] = __VERSION__;
#endif
    _log["runtime"]["start_time"] = static_cast<int>(start_time);
}

void Runtime::arguments(int argc, char** argv) {
    _log["call"]["binary"] = argv[0];
    for (int i = 1; i < argc; ++i) {
        _log["call"]["parameters"] += argv[i];
    }
}

Runtime::~Runtime() {
    _rt = NULL;

    // record runtime
    time_t end_time;
    time(&end_time);
    _log["runtime"]["end_time"] = static_cast<int>(end_time);
    _log["runtime"]["seconds"] = difftime(end_time, start_time);

    // record last error
    if (errno != 0) {
        _log["error"] = strerror(errno);
    }

    std::cout << _log << '\n';
}

__attribute__((noreturn)) void Runtime::signalHandler(int signum) {
    if (_rt) {
        _rt->_log["exit"]["signal"] = strsignal(signum);
        _rt->_log["exit"]["code"] = EXIT_FAILURE;
    }

    exit(EXIT_FAILURE);
}

__attribute__((noreturn)) void Runtime::newHandler() {
    if (_rt) {
        _rt->_log["exit"]["code"] = EXIT_FAILURE;
        _rt->_log["error"] = "memory allocation failed";
    }

    exit(EXIT_FAILURE);
}

void Runtime::exitHandler() {
    std::cout << "That's it" << std::endl;
}

/*!
 \param format  the status message formatted as printf string
 
 \note use this function rather sparsely in order not to spam the output
*/
void Runtime::message(const char* format, ...) {
    va_list args;
    va_start(args, format);
    vsprintf(_buffer, format, args);
    va_end(args);

    JSON entry(JSON::object);
    entry["time"] = static_cast<int>(std::time(0));
    entry["message"] = _buffer;
    _log["messages"].push_back(entry);

    std::cerr << _buffer << '\n';
}

/*!
 \param format  the status message formatted as printf string
*/
void Runtime::status(const char* format, ...) {
    va_list args;
    va_start(args, format);
    vsprintf(_buffer, format, args);
    va_end(args);

    JSON entry(JSON::object);
    entry["time"] = static_cast<int>(std::time(0));
    entry["message"] = _buffer;
    _log["messages"].push_back(entry);

    if (verbose) {
        std::cerr << _buffer << '\n';
    }
}

/*!
 \param code    the error code
 \param format  the error message formatted as printf string

 \note The codes should be documented in the manual.
*/
void Runtime::error(int code, const char* format, ...) {
    va_list args;
    va_start(args, format);
    vsprintf(_buffer, format, args);
    va_end(args);

    std::cerr << _buffer << '\n';

    //fprintf(stderr, "%s -- %saborting [#%02d]%s\n", _c_, _cR_, code, _c_);
    status("see manual for a documentation of this error");

    if (errno != 0) {
        status("last error message: %s", strerror(errno));
    }

    std::exit(0);
}
