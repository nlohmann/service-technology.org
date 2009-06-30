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

# only use installed Petri Net API if this was configured with --enable-pnapi
AC_ARG_ENABLE(pnapi,
  AS_HELP_STRING([--enable-pnapi],[use installed Petri Net API (disabled by default)]),
  enable_pnapi=${enableval}, enable_pnapi="no")
AC_MSG_CHECKING([whether to use installed Petri Net API])
AC_MSG_RESULT($enable_pnapi)
if test "$enable_pnapi" = "yes"; then
  # check if the Petri Net API is already installed
  AM_MISSING_PROG(PKG_CONFIG, pkg-config)
  AC_CHECK_LIB(pnapi, main)
  if test "${ac_cv_lib_pnapi_main}" = "no"; then
    AC_MSG_WARN([Petri Net API was not found!])
    AC_MSG_WARN([Using shiped version from /libs/pnapi.])
  fi
  AM_CONDITIONAL(COMPILE_PNAPI, [test "${ac_cv_lib_pnapi_main}" = "no"])
else
  AM_CONDITIONAL(COMPILE_PNAPI, [true])
fi

AC_CONFIG_FILES([Makefile libs/pnapi/Makefile src/Makefile doc/Doxyfile maintainer/Makefile])
AC_OUTPUT
