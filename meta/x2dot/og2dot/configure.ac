AC_INIT(og2dot, 1.0, georgstraube@service-technology.org)

AC_CONFIG_SRCDIR([src/main.cc])
AC_CONFIG_SRCDIR([src/og-lexic.ll])
AC_CONFIG_HEADERS([src/config.h])

AM_INIT_AUTOMAKE

AC_PROG_CXX
AM_PROG_LEX
AC_PROG_YACC


AM_MISSING_PROG(GENGETOPT, gengetopt)
AM_MISSING_PROG(HELP2MAN, help2man)

AC_CONFIG_FILES([Makefile src/Makefile])

AC_OUTPUT
