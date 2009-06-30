AC_INIT(Diane, 0.9, stephan.mennicke@service-technology.org)
AC_CONFIG_SRCDIR([src/main.cc])
AC_CONFIG_HEADERS([src/config.h])

AM_INIT_AUTOMAKE

# Checks for programs.
AM_PROG_CC_C_O
AC_PROG_CXX
AC_PROG_LEX
AC_PROG_YACC
AC_PROG_RANLIB

AM_MISSING_PROG(GENGETOPT, gengetopt)
AM_MISSING_PROG(HELP2MAN, help2man)

# needed for the Petri Net API
AC_DEFINE_UNQUOTED(CONFIG_PETRIFY, "not found", [Petrify is not used.])

AC_HEADER_ASSERT

# check if the Petri Net API is already installed
AC_CHECK_LIB([pnapi], [main])
AM_CONDITIONAL(COMPILE_PNAPI, [test "${ac_cv_lib_pnapi_main}" = "no"])

AC_CONFIG_FILES([Makefile libs/pnapi/Makefile src/Makefile doc/Doxyfile maintainer/Makefile])
AC_OUTPUT
