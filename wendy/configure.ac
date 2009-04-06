AC_PREREQ([2.63])
AC_INIT(Wendy, 2.0, niels@service-technology.org)
AC_CONFIG_SRCDIR([src/main.cc])
AC_CONFIG_HEADERS([src/config.h])

AM_INIT_AUTOMAKE

# Checks for programs.
AC_PROG_CC
AC_PROG_CXX
AC_PROG_LEX
AC_PROG_YACC
AC_PROG_RANLIB

# needed for the Petri Net API
AC_DEFINE_UNQUOTED(CONFIG_PETRIFY, "not found", [Petrify is not used.])

AC_HEADER_ASSERT
#AC_CHECK_HEADERS([pthread.h])

# check for functions
AC_CHECK_FUNCS([popen pclose],,AC_MSG_ERROR([cannot use popen or pclose]))

AC_TYPE_UINT8_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T

AC_CONFIG_FILES([Makefile libs/Makefile src/Makefile doc/Doxyfile])
AC_OUTPUT
