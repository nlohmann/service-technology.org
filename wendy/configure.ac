AC_INIT([Wendy], 3.0-unreleased, wendy@service-technology.org, wendy, http://service-technology.org/wendy)

AC_MSG_NOTICE([__        __             _       ])
AC_MSG_NOTICE([\ \      / /__ _ __   __| |_   _ ])
AC_MSG_NOTICE([ \ \ /\ / / _ \ '_ \ / _` | | | |])
AC_MSG_NOTICE([  \ V  V /  __/ | | | (_| | |_| |])
AC_MSG_NOTICE([   \_/\_/ \___|_| |_|\__,_|\__, |])
AC_MSG_NOTICE([                           |___/ ])
AC_MSG_NOTICE([synthesizing partners for services])
AC_MSG_NOTICE([http://service-technology.org/wendy])
AC_MSG_NOTICE([version ${PACKAGE_VERSION}])
AC_MSG_NOTICE()

AC_CONFIG_SRCDIR([src/main.cc])
AC_CONFIG_HEADERS([src/config.h])

AM_INIT_AUTOMAKE(gnits)
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

# export tools needed to determine memory consumption
AC_DEFINE_UNQUOTED(TOOL_AWK, "${AWK}", [awk])
AC_DEFINE_UNQUOTED(TOOL_GREP, "${GREP}", [grep])

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

# look up canonical build name and write it to config.h
AC_CANONICAL_BUILD
AC_DEFINE_UNQUOTED([CONFIG_BUILDSYSTEM], ["$build"], [The platform.])

# search for optional tools needed by "make check"
AC_PATH_PROG(CNF2BITS, [wendyFormula2bits], not found)
AC_PATH_PROG(FIONA, [fiona], not found)
AC_PATH_PROG(MARLENE, [marlene], not found)
AC_PATH_PROG(MIA, [mia], not found)
AC_PATH_PROG(SA2SM, [sa2sm], not found)
AC_PATH_PROG(VALGRIND, [valgrind], not found)
AC_PATH_PROG(WENDY2FIONA, [wendy2fiona], not found)

# search for required tools
AC_PATH_PROG(LOLA, [lola-statespace], [])
if test "${LOLA}" = ""; then
  AC_MSG_WARN([LoLA was not found. Please read file REQUIREMENTS!])
else
AC_PATH_PROGS_FEATURE_CHECK([LOLAVERSION], [lola-statespace], [[ac_cv_path_LOLAVERSION=`$ac_path_LOLAVERSION --version`]])
AC_MSG_CHECKING([LoLA version])
AC_MSG_RESULT([${ac_cv_path_LOLAVERSION}])
if test "${ac_cv_path_LOLAVERSION}" != "LoLA 1.14"; then
  AC_MSG_WARN([You have ${ac_cv_path_LOLAVERSION}, but Wendy needs LoLA 1.14!])
  AC_MSG_WARN([See file REQUIREMENTS.])
fi
fi

# check for required functions and die if they are not found
AC_CHECK_FUNCS([popen pclose mktemp vfprintf],,AC_MSG_ERROR([cannot use required functions]))

# FreeBSD does not know log2()
AC_CHECK_FUNCS([log2])

# check and set exact unsigned types
AC_TYPE_UINT8_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_SIZE_T
AC_CHECK_SIZEOF([void*])

# syslogging can be configured with `--enable-syslog'
AC_ARG_ENABLE(syslog, AS_HELP_STRING([--enable-syslog], [whether to use syslog (default is off)]), AC_DEFINE([USE_SYSLOG], 1, [whether to use syslog]))

# write files
AC_CONFIG_FILES([
	Makefile libs/pnapi/Makefile src/Makefile src/wendy.conf doc/Makefile
	doc/Doxyfile doc/Doxydetails man/Makefile tests/Makefile
	tests/package.m4])
AC_CONFIG_FILES([tests/cover.sh], [chmod +x tests/cover.sh])
AC_OUTPUT
