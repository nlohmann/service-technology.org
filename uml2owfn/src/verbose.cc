/*****************************************************************************\
 UML2oWFN -- Translating UML2 Activity Diagrams to Petri nets

 Copyright (C) 2005, 2006, 2007  Niels Lohmann, Christian Gierds,
                                 Martin Znamirowski, and Dirk Fahland

 UML2oWFN is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 UML2oWFN is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with UML2oWFN.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/


#include <cstdarg>
#include <cstdlib>
#include <cerrno>
#include <cstring>
#include <config.h>

#include "debug.h"

//extern gengetopt_args_info args_info;
extern trace_level debug_level;

/*!
 \param format  the status message formatted as printf string

 \note use this function rather sparsely in order not to spam the output
*/
void message(const char* format, ...) {
    fprintf(stderr, "%s: ", PACKAGE);

    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);

    fprintf(stderr, "\n");
}


/*!
 \param format  the status message formatted as printf string
*/
void status(const char* format, ...) {
    if (debug_level <= 1) {
        return;
    }

    fprintf(stderr, "%s: ", PACKAGE);

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
__attribute__((noreturn)) void abort(unsigned short code, const char* format, ...) {
    fprintf(stderr, "%s: ", PACKAGE);

    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);

    fprintf(stderr, " -- aborting [#%02d]\n", code);

    status("see manual for a documentation of this error");

    if (errno != 0) {
        status("last error message: %s", strerror(errno));
    }

    exit(EXIT_FAILURE);
}
