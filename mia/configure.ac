AC_INIT(Mia, 1.1-unreleased, [mia@service-technology.org])
AC_CONFIG_SRCDIR([src/main.cc])
AC_CONFIG_HEADERS([src/config.h])

AM_INIT_AUTOMAKE(gnits)

# Checks for programs.
AM_PROG_CC_C_O
AC_PROG_CXX
AC_PROG_LEX
AC_PROG_GREP
AC_PROG_SED
AC_PROG_YACC
AC_PROG_RANLIB

# check for additional programs
AM_MISSING_PROG(GENGETOPT, gengetopt)
AM_MISSING_PROG(HELP2MAN, help2man)

# set up autotest
AM_MISSING_PROG(AUTOM4TE, autom4te)
AC_CONFIG_TESTDIR(tests)

AC_HEADER_ASSERT

# check for Wendy
AC_PATH_PROG(WENDY, [wendy], not found)
if test "$WENDY" = "not found"; then
  AC_MSG_ERROR([Wendy was not found, but is needed by Mia.])
else
  AC_MSG_CHECKING([Wendy version])
  WENDY_VERSION=`$WENDY --version | $SED -e 's/wendy //'`
  WENDY_VERSION_REQ="1.4"
  AC_MSG_RESULT($WENDY_VERSION)
  if test "$WENDY_VERSION" \< "$WENDY_VERSION_REQ"; then
    AC_MSG_ERROR([Wendy version $WENDY_VERSION_REQ required, but $WENDY_VERSION found.])
  else
    AC_DEFINE_UNQUOTED(BINARY_WENDY, "${WENDY}", [Wendy binary.])
  fi
fi

# check for LoLA
AC_PATH_PROG(LOLA, [lola-full], not found)
if test "$LOLA" = "not found"; then
  AC_MSG_ERROR([LoLA was not found, but is needed by Mia.])
else
  AC_DEFINE_UNQUOTED(BINARY_LOLA, "${LOLA}", [LoLA binary.])
fi

# needed for the Petri Net API
AC_DEFINE_UNQUOTED(CONFIG_PETRIFY, "not found", [Petrify is not used.])

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

# check for functions
AC_CHECK_FUNCS([popen pclose],,AC_MSG_WARN([cannot use popen or pclose]))

AC_CONFIG_FILES([Makefile libs/pnapi/Makefile src/Makefile doc/Doxyfile doc/Makefile man/Makefile tests/Makefile tests/package.m4 maintainer/Makefile])
AC_OUTPUT
