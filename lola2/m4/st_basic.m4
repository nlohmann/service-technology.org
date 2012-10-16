# serial 1
AC_DEFUN([AC_ST_BASIC],[

# store the SVN revision number
AC_SUBST(VERSION_SVN, "`svnversion -n 2> /dev/null`")
AC_DEFINE_UNQUOTED([VERSION_SVN], ["${VERSION_SVN}"], [The SVN revision.])

# store the hostname
AC_DEFINE_UNQUOTED([CONFIG_HOSTNAME], ["${ac_hostname}"], [The hostname.])

# look up canonical build name and write it to config.h
AC_CANONICAL_BUILD
AC_DEFINE_UNQUOTED([CONFIG_BUILDSYSTEM], ["${build}"], [The platform.])

# check for basic tools (also important for GNU Autotest)
AC_PROG_SED
AC_PROG_GREP
AC_PROG_AWK

# export tools needed to determine memory consumption
AC_DEFINE_UNQUOTED(TOOL_AWK, "\"${AWK}\"", [awk])
AC_DEFINE_UNQUOTED(TOOL_GREP, "\"${GREP}\"", [grep])

# check for additional programs needed to compile
AM_MISSING_PROG(GENGETOPT, gengetopt)
AM_MISSING_PROG(HELP2MAN, help2man)

# allow the configure script to control assertions (just include config.h)
AC_HEADER_ASSERT
AH_BOTTOM([#ifdef __cplusplus
#include <cassert>
#else
#include <assert.h>
#endif
])

# functions to influence branch prediction
AC_HEADER_ASSERT
AH_BOTTOM([/*!
\brief tell compiler that condition is very likely true
\sa http://stackoverflow.com/questions/109710/likely-unlikely-macros-in-the-linux-kernel
*/
#define LIKELY(x)       (__builtin_expect((x),1))

/*!
\brief tell compliler that condition is very likely false
\sa http://stackoverflow.com/questions/109710/likely-unlikely-macros-in-the-linux-kernel
*/
#define UNLIKELY(x)     (__builtin_expect((x),0))
])

#
# add a Doxygen comment
AH_TOP([/*!
\file config.h
\brief preprocessor directives set by the configure script
*/
])

])
