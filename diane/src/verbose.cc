/*****************************************************************************\
 Diane -- Decomposition of Petri nets.

 Copyright (C) 2009  Stephan Mennicke <stephan.mennicke@uni-rostock.de>

 Diane is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Diane is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Diane.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/


#include <cstdarg>
#include <cstdlib>

#include "config.h"
#include "cmdline.h"

extern gengetopt_args_info args_info;

void status(const char *format, ...) {
    if (args_info.verbose_flag == 0) {
        return;
    }

    fprintf(stderr, "%s: ", PACKAGE);

    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end (args);

    fprintf(stderr, "\n");
}


/*!
 \brief abort with an error message and an error code

 The codes are documented in Diane's manual.

 \param code    the error code
 \param format  the error message formatted as printf string
*/
void abort(unsigned int code, const char *format, ...) {
    fprintf(stderr, "%s: ", PACKAGE);

    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end (args);

    fprintf(stderr, " -- aborting [#%02d]\n", code);

    status("see manual for a documentation of this error");

    exit(EXIT_FAILURE);
}