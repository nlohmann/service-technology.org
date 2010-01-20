AC_INIT(Sayo, 1.2-unreleased, sayo@service-technology.org, sayo, http://service-technology.org/sayo)

# the first line should be everything you will ever need to touch
#############################################################################

AC_PREREQ(2.59)
AM_INIT_AUTOMAKE(gnits)
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

# where to look for source code and how the config header should be called
AC_CONFIG_SRCDIR(src/main.cc)
AC_CONFIG_HEADERS(src/config.h)

# check for generic programs
AC_PROG_CXX
AM_PROG_CC_C_O
AC_PROG_LEX
AC_PROG_YACC
AC_PROG_SED
AC_PROG_GREP

# allow the configure script to control assertions (just include config.h)
AC_HEADER_ASSERT
AH_BOTTOM([#ifdef __cplusplus
#include <cassert>
#else
#include <assert.h>
#endif])

# search for Fiona, sa2sm and Wendy (nice to have for "make check")
AC_PATH_PROG(FIONA, [fiona], not found)
AC_PATH_PROG(SA2SM, [sa2sm], not found)
AC_PATH_PROG(WENDY, [wendy], not found)
AC_PATH_PROG(WENDY2FIONA, [wendy2fiona], not found)

# check for additional programs
AM_MISSING_PROG(GENGETOPT, gengetopt)
AM_MISSING_PROG(HELP2MAN, help2man)

# export tools needed to determine memory consumption
AC_DEFINE_UNQUOTED(TOOL_AWK, "${AWK}", [awk])
AC_DEFINE_UNQUOTED(TOOL_GREP, "${GREP}", [grep])

# set up autotest
AM_MISSING_PROG(AUTOM4TE, autom4te)
AC_CONFIG_TESTDIR(tests)

#############################################################################
# create configured files (Makefiles, scripts, etc.)
#############################################################################
AC_CONFIG_FILES([Makefile
                 doc/Makefile doc/Doxyfile
                 src/Makefile
                 tests/Makefile tests/package.m4
                 man/Makefile])
AC_CONFIG_FILES([src/publicview.sh], [chmod +x src/publicview.sh])
AC_CONFIG_FILES([tests/cover.sh], [chmod +x tests/cover.sh])
AC_OUTPUT

