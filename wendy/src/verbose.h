/*****************************************************************************\
 Wendy -- Synthesizing Partners for Services

 Copyright (c) 2009 Niels Lohmann, Christian Sura, and Daniela Weinberg

 Wendy is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Wendy is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Wendy.  If not, see <http://www.gnu.org/licenses/>. 
\*****************************************************************************/


#ifndef VERBOSE_H
#define VERBOSE_H

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <unistd.h>

/// unconditionally print a message
void message(const char* format, ...);

/// print a message if "--verbose" parameter was given
void status(const char* format, ...);

/// abort with an error message and an error code
void abort(unsigned short code, const char* format, ...);


/**************************************************************************\
 The following code organizes colored output on stderr. It defines several
 preprosessor directives to be used in printf. Each color has one macro to
 enable it (e.g., "_cg" for green). Another macro "_c_" resets the color.

        Example: fprintf(stderr, "this is %sgreen%s", _cg, _c_);

 The idea to check the "TERM" environment variable was inspired by the
 Google C++ testing framework (http://code.google.com/p/googletest/).
\**************************************************************************/

/// whether to use colored output
#ifndef __MINGW32__
const bool _useColor = isatty(fileno(stderr)) && (
    !strcmp(getenv("TERM"), "linux") ||
    !strcmp(getenv("TERM"), "cygwin") ||
    !strcmp(getenv("TERM"), "xterm") ||
    !strcmp(getenv("TERM"), "xterm-color") ||
    !strcmp(getenv("TERM"), "xterm-256color"));
#else
const bool _useColor = false;
#endif

/// set foreground color to red
#define _cr (_useColor ? "\033[0;31m" : "" )
/// set foreground color to green
#define _cg (_useColor ? "\033[0;32m" : "" )
/// set foreground color to yellow
#define _cy (_useColor ? "\033[0;33m" : "" )
/// set foreground color to blue
#define _cb (_useColor ? "\033[0;34m" : "" )
/// set foreground color to magenta
#define _cm (_useColor ? "\033[0;35m" : "" )
/// set foreground color to cyan
#define _cc (_useColor ? "\033[0;36m" : "" )

/// set foreground color to black (bold)
#define _c0 (_useColor ? "\033[0;1;30m" : "" )
/// set foreground color to red (bold)
#define _cR (_useColor ? "\033[0;1;31m" : "" )
/// set foreground color to green (bold)
#define _cG (_useColor ? "\033[0;1;32m" : "" )
/// set foreground color to yellow (bold)
#define _cY (_useColor ? "\033[0;1;33m" : "" )
/// set foreground color to blue (bold)
#define _cB (_useColor ? "\033[0;1;34m" : "" )
/// set foreground color to magenta (bold)
#define _cM (_useColor ? "\033[0;1;35m" : "" )
/// set foreground color to cyan (bold)
#define _cC (_useColor ? "\033[0;1;36m" : "" )

/// reset foreground color
#define _c_ (_useColor ? "\033[m" : "")

#endif
