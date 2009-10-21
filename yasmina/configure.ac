AC_INIT(Yasmina, 0.4-unreleased, olivia.oanea@service-technology.org)
AC_CONFIG_SRCDIR([src/main.cc])
AC_CONFIG_HEADERS([src/config.h])

AM_INIT_AUTOMAKE(gnits)

# Checks for programs.
AM_PROG_CC_C_O
AC_PROG_CXX
AC_PROG_LEX
AC_PROG_YACC
AC_PROG_SED
AC_PROG_GREP
#AC_PROG_RANLIB
AC_PROG_LIBTOOL
AC_CONFIG_MACRO_DIR([m4]) 

AM_MISSING_PROG(GENGETOPT, gengetopt)
AM_MISSING_PROG(HELP2MAN, help2man)

# export tools needed to determine memeory consumption
AC_DEFINE_UNQUOTED(TOOL_AWK, "${AWK}",[awk])
AC_DEFINE_UNQUOTED(TOOL_GREP, "${GREP}",[grep])

# set up autotest
AM_MISSING_PROG(AUTOM4TE, autom4te)
AC_CONFIG_TESTDIR(tests)

# check for dlopen/ftime (required by lp_solve)
AC_SEARCH_LIBS([dlopen],[dl zrldsrl])
AC_SEARCH_LIBS([ftime],[compat])

# needed for the Petri Net API
AC_DEFINE_UNQUOTED(CONFIG_PETRIFY, "not found", [Petrify is not used.])

# allow the configure script to control assertions (just include config.h)
AC_HEADER_ASSERT
AH_BOTTOM([#ifdef __cplusplus
#include <cassert>
#else
#include <assert.h>
#endif])


# check for dlopen library (required by lp_solve)
  	 AC_SEARCH_LIBS([dlopen], [dl zrldsrl])

# check for functions
AC_CHECK_FUNCS([popen pclose],,AC_MSG_WARN([cannot use popen or pclose]))

# check for exact unsigned types
AC_TYPE_UINT8_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_INT64_T

AC_CONFIG_FILES([Makefile libs/pnapi/Makefile libs/lp_solve/Makefile  src/Makefile doc/Makefile doc/Doxyfile tests/Makefile tests/package.m4 man/Makefile])
AC_CONFIG_FILES([tests/cover.sh], [chmod +x tests/cover.sh])
AC_OUTPUT
