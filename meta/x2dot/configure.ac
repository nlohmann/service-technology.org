AC_INIT(og2dot, 1.0, georgstraube@service-technology.org)

AM_INIT_AUTOMAKE
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

AC_CONFIG_SRCDIR([src/og2dot/main.cc])
AC_CONFIG_HEADERS([src/og2dot/config.h])

AC_PROG_CXX
AM_PROG_LEX
AC_PROG_YACC
AC_PROG_SED
AC_PROG_AWK

AM_MISSING_PROG(GENGETOPT, gengetopt)
AM_MISSING_PROG(HELP2MAN, help2man)

# allow the configure script to control assertions (just include config.h)
AC_HEADER_ASSERT
AH_BOTTOM([#ifdef __cplusplus
#include <cassert>
#else
#include <assert.h>
#endif])

AC_CONFIG_FILES([Makefile src/og2dot/Makefile])

AC_OUTPUT
