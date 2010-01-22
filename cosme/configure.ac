AC_INIT(Cosme, 0.7, cosme@service-technology.org, cosme, http://service-technology.org/cosme)

AC_CONFIG_SRCDIR([src/main.cc])
AC_CONFIG_HEADERS([src/config.h])

AM_INIT_AUTOMAKE #(gnits)
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

# check for standard programs needed to compile
AC_PROG_CXX
AM_PROG_CC_C_O
AC_PROG_LEX
AC_PROG_YACC
AC_PROG_RANLIB
AC_PROG_SED
AC_PROG_GREP
AC_PROG_AWK

# check for additional programs needed to compile
AM_MISSING_PROG(GENGETOPT, gengetopt)
AM_MISSING_PROG(HELP2MAN, help2man)

# set up autotest
AM_MISSING_PROG(AUTOM4TE, autom4te)
AC_CONFIG_TESTDIR(tests)

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

# check for required functions and die if they are not found
AC_CHECK_FUNCS([popen pclose mktemp vfprintf],,AC_MSG_ERROR([cannot use required functions]))

# check and set exact unsigned types
AC_TYPE_UINT8_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T

AC_CONFIG_FILES([Makefile libs/pnapi/Makefile src/Makefile doc/Doxyfile tests/Makefile tests/package.m4])
AC_CONFIG_FILES([tests/cover.sh], [chmod +x tests/cover.sh])
AC_OUTPUT

# display a warning in case a required tool has not been found
if test "${LOLA}" = ""; then
  echo
  echo "WARNING: Not all required tools have been found. Please read file"
  echo "         REQUIREMENTS and edit the file src/wendy.conf before"
  echo "         using or installing Wendy."
  echo
fi
