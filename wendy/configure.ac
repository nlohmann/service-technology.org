AC_INIT(Wendy, 1.6-unreleased, wendy@service-technology.org)
AC_CONFIG_SRCDIR([src/main.cc])
AC_CONFIG_HEADERS([src/config.h])

AM_INIT_AUTOMAKE(gnits)

# check for programs needed to compile
AC_PROG_CXX
AM_PROG_CC_C_O
AC_PROG_LEX
AC_PROG_YACC
AC_PROG_RANLIB
AC_PROG_SED
AC_PROG_GREP
AM_MISSING_PROG(GENGETOPT, gengetopt)
AM_MISSING_PROG(HELP2MAN, help2man)

# set up autotest
AM_MISSING_PROG(AUTOM4TE, autom4te)
AC_CONFIG_TESTDIR(tests)

# look up canonical build name and write it to config.h
AC_CANONICAL_BUILD
AC_DEFINE_UNQUOTED([CONFIG_BUILDSYSTEM], ["$build"], [The platform.])

# search for LoLA
AC_PATH_PROG(LOLA, [lola-full], [])

# search for Fiona (nice to have for "make check")
AC_PATH_PROG(FIONA, [fiona], not found)

AC_HEADER_ASSERT

# check for functions
AC_CHECK_FUNCS([popen pclose],,AC_MSG_WARN([Wendy cannot use UNIX pipes.]))
AC_CHECK_FUNCS([mkstemp])

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

# check for exact unsigned types
AC_TYPE_UINT8_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T

AC_CONFIG_FILES([
	Makefile libs/pnapi/Makefile src/Makefile src/wendy.conf doc/Makefile
	doc/Doxyfile man/Makefile maintainer/Makefile utils/Makefile
	maintainer/debian/changelog.template maintainer/debian/info
	maintainer/debian/README.Debian.template maintainer/debian/rules
	maintainer/macports/Portfile tests/Makefile tests/package.m4
])
AC_OUTPUT

# display a warning in case a required tool has not been found
if test "${LOLA}" = ""; then
  echo
  echo "WARNING: Not all required tools have been found. Please read file"
  echo "         REQUIREMENTS and edit the file src/wendy.conf before"
  echo "         using or installing Wendy."
  echo
fi
