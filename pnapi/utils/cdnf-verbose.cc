#include <cstdarg>
#include <cstdlib>
#include <cerrno>
#include <cstring>
#include <string>
#include <fstream>
#include "cdnf-cmdline.h" // TODO: find proper solution for different commandline parsers
#include "verbose.h"

extern gengetopt_args_info args_info;


/***************************************************************************\
 * syslog functionalities (to be enabled with "configure --enable-syslog") *
\***************************************************************************/
#ifdef USE_SYSLOG
#include <syslog.h>

extern std::string invocation;

/// dummy class to have global constructor and destructor
namespace st {
    class Logger {
        public:
            Logger() {
                openlog(PACKAGE, LOG_PID, LOG_USER);
                syslog(LOG_NOTICE, "--> starting %s", PACKAGE_STRING);
            }

            ~Logger() {
                syslog(LOG_NOTICE, "<-- done: %s", invocation.c_str());
                closelog();
            }
    };

    /// dummy object living in global namespace
    Logger myLogger;
}
#endif


/*!
 \param format  the status message formatted as printf string
 
 \note use this function rather sparsely in order not to spam the output
*/
void message(const char* format, ...) {
    fprintf(stderr, "%s: ", _ctool_(PACKAGE));

    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);

#ifdef USE_SYSLOG
    va_start(args, format);
    vsyslog(LOG_NOTICE, format, args);
    va_end(args);
#endif

    fprintf(stderr, "\n");
}


/*!
 \param format  the status message formatted as printf string
*/
void status(const char* format, ...) {
    if (not args_info.verbose_flag) {
        return;
    }

    fprintf(stderr, "%s: ", _ctool_(PACKAGE));

    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);

#ifdef USE_SYSLOG
    va_start(args, format);
    vsyslog(LOG_NOTICE, format, args);
    va_end(args);
#endif

    fprintf(stderr, "\n");
}


/*!
 \param code    the error code
 \param format  the error message formatted as printf string

 \note The codes should be documented in the manual.
*/
void abort(unsigned short code, const char* format, ...) {
    fprintf(stderr, "%s: %s", _ctool_(PACKAGE), _bold_);

    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);

#ifdef USE_SYSLOG
    va_start(args, format);
    vsyslog(LOG_ERR, format, args);
    va_end(args);
#endif

    fprintf(stderr, "%s -- %saborting [#%02d]%s\n", _c_, _cR_, code, _c_);
    status("see manual for a documentation of this error");

    if (errno != 0) {
        status("last error message: %s", strerror(errno));
    }

    exit(EXIT_FAILURE);
}



void displayFileError(char* filename, int lineno, char* token) {
    std::ifstream f(filename);
    std::string line;
    for (int i = 0; i < lineno; ++i) {
        getline(f, line);
    }
    size_t firstpos(line.find_first_of(token));
    std::string format = line.replace(firstpos, strlen(token), std::string(_cbad_(token)));
    fprintf(stderr, "  %s\n", line.c_str());
    fprintf(stderr, "  ");
    for (unsigned int i = 0; i < firstpos; ++i) {
        fprintf(stderr, "  ");
    }
    for (unsigned int i = 0; i < strlen(token); ++i) {
        fprintf(stderr, "^");
    }
    fprintf(stderr, "\n");
}
