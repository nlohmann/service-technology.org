AC_INIT(x2dot, 1.0, georgstraube@service-technology.org)

AM_INIT_AUTOMAKE
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

AC_CONFIG_SRCDIR([src/og2dot/main.cc])
AC_CONFIG_HEADERS([src/og2dot/config.h])

AC_PROG_CXX
AM_PROG_LEX
AC_PROG_YACC
AC_PROG_SED
AC_PROG_AWK
AC_PROG_GREP

AM_MISSING_PROG(GENGETOPT, gengetopt)
AM_MISSING_PROG(HELP2MAN, help2man)

# search for dot
AC_PATH_PROG(DOT, [dot], not found)
if test "$DOT" = "not found"; then
  AC_MSG_WARN([Graphviz Dot was not found. See file README.])
  AC_DEFINE_UNQUOTED(CONFIG_DOT, "not found", [Graphviz Dot was not found.])
else
  AC_DEFINE_UNQUOTED(CONFIG_DOT, "${DOT}", [Graphviz Dot can be used.])
fi
AC_SUBST(DOT)

AC_DEFINE_UNQUOTED(TOOL_AWK, "${AWK}", [awk])
AC_DEFINE_UNQUOTED(TOOL_GREP, "${GREP}", [grep])

# set up autotest
AM_MISSING_PROG(AUTOM4TE, autom4te)
AC_CONFIG_TESTDIR(tests)

# allow the configure script to control assertions (just include config.h)
AC_HEADER_ASSERT
AH_BOTTOM([#ifdef __cplusplus
#include <cassert>
#else
#include <assert.h>
#endif])

AC_CONFIG_FILES([Makefile src/og2dot/Makefile src/chor2dot/Makefile src/rg2dot/Makefile tests/Makefile tests/package.m4])
AC_CONFIG_FILES([tests/cover.sh], [chmod +x tests/cover.sh])

AC_OUTPUT
