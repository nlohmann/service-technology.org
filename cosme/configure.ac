AC_INIT(Cosme, 0.7, andreas.lehmann@uni-rostock.de, cosme)

AC_CONFIG_SRCDIR([src/og-lexic.ll])
AC_CONFIG_HEADERS([src/config.h])

AM_INIT_AUTOMAKE
AM_PROG_CC_C_O
AC_PROG_CXX
AM_PROG_LEX
AC_PROG_YACC
AC_PROG_RANLIB
AC_PROG_SED
AC_PROG_GREP
AC_PROG_AWK

# check for standard programs needed to compile
AM_MISSING_PROG(GENGETOPT, gengetopt)

# store the SVN revision number
AC_SUBST(VERSION_SVN, "`svnversion -n 2> /dev/null`")
AC_DEFINE_UNQUOTED(VERSION_SVN, "${VERSION_SVN}", [The SVN revision.])

# allow the configure script to control assertions (just include config.h)
AC_HEADER_ASSERT
AH_BOTTOM([#ifdef __cplusplus
#include <cassert>
#else
#include <assert.h>
#endif])

# search for required tools
AC_PATH_PROG(LOLA, [lola-statespace], [])

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
    AC_MSG_WARN([Using shipped version from libs/pnapi.])
  fi
  AM_CONDITIONAL(COMPILE_PNAPI, [test "${ac_cv_lib_pnapi_main}" = "no"])
else
  AM_CONDITIONAL(COMPILE_PNAPI, [true])
fi

# check for functions
AC_CHECK_FUNCS([popen pclose],,AC_MSG_WARN([cannot use pipes -- expect bad performance]))
AC_CHECK_FUNCS([mktemp vfprintf],,AC_MSG_ERROR([cannot use required functions]))

# check for exact unsigned types
AC_TYPE_UINT8_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T

AC_CONFIG_FILES([Makefile libs/pnapi/Makefile src/Makefile maintainer/Makefile doc/Doxyfile])

AC_OUTPUT

# display a warning in case a required tool has not been found
if test "${LOLA}" = ""; then
  echo
  echo "WARNING: Not all required tools have been found. Please read file"
  echo "         REQUIREMENTS and edit the file src/wendy.conf before"
  echo "         using or installing Wendy."
  echo
fi
