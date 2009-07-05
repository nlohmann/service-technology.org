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

# check for required tools
AC_PATH_PROG(LOLA_FULL, [lola-full], [])
AC_PATH_PROG(LOLA_FULL1, [lola-full1], [${LOLA_FULL}])
AC_PATH_PROG(WENDY, [wendy], [])

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
AC_CHECK_FUNCS([mkstemp])

AC_CONFIG_FILES([Makefile libs/pnapi/Makefile src/Makefile src/mia.conf doc/Doxyfile doc/Makefile man/Makefile tests/Makefile tests/package.m4 maintainer/Makefile maintainer/debian/changelog.template maintainer/debian/info maintainer/debian/README.Debian.template maintainer/debian/rules maintainer/macports/Portfile])
AC_OUTPUT

# display a warning in case a required tool has not been found
if test "${LOLA_FULL}" = "" -o "${WENDY}" = ""; then
  echo
  echo "WARNING: Not all required tools have been found. Please read file"
  echo "         REQUIREMENTS and edit the file src/mia.conf before"
  echo "         using or installing Mia."
  echo
fi
