# initial data for autoconf
AC_INIT([Candy], [1.0-unreleased], [candy@service-technology.org], candy,
http://service-technology.org/candy)

AC_MSG_NOTICE([  ____                 _       ])
AC_MSG_NOTICE([ / ___| __ _ _ __   __| |_   _ ])
AC_MSG_NOTICE([| |    / _` | '_ \ / _` | | | |])
AC_MSG_NOTICE([| |___| (_| | | | | (_| | |_| |])
AC_MSG_NOTICE([ \____|\__,_|_| |_|\__,_|\__, |])
AC_MSG_NOTICE([                         |___/ ])
AC_MSG_NOTICE([synthesizing cost efficient partners for services])
AC_MSG_NOTICE([http://service-technology.org/candy])
AC_MSG_NOTICE([version ${PACKAGE_VERSION}])
AC_MSG_NOTICE()

################################################################################
# checking for programs, functions, etc. used by candy build
################################################################################

# check for autoconf version 2.59
AC_PREREQ(2.59)

# initialize automake without full verbose messages
AM_INIT_AUTOMAKE #(gnits)
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

# check for specific files (random sample)
AC_CONFIG_SRCDIR([src/main.cc])

# write test results as #defines into header file
AC_CONFIG_HEADERS([src/config.h])

# check whether to enable assertions in the style of assert.h
AC_HEADER_ASSERT

# check for programs necessary for candy
AC_PROG_CXX
AM_PROG_CC_C_O
AC_PROG_LEX
AC_PROG_YACC
AC_PROG_RANLIB
AC_PROG_SED
AC_PROG_GREP
AC_PROG_AWK

# check for additional programs
AM_MISSING_PROG(GENGETOPT, gengetopt)
AM_MISSING_PROG(HELP2MAN, help2man)

# export tools needed to determine memory consumption
AC_DEFINE_UNQUOTED([TOOL_AWK],  ["${AWK}"],  [awk])
AC_DEFINE_UNQUOTED([TOOL_GREP], ["${GREP}"], [grep])

# store the SVN revision number and write it to config.h
AC_SUBST(VERSION_SVN, "`svnversion -n 2> /dev/null`")
AC_DEFINE_UNQUOTED([VERSION_SVN], ["${VERSION_SVN}"], [The SVN revision.])

# look up canonical build name and write it to config.h
AC_CANONICAL_BUILD
AC_DEFINE_UNQUOTED([CONFIG_BUILDSYSTEM], ["$build"], [The platform.])

# allow the configure script to control assertions (just include config.h)
AC_HEADER_ASSERT
AH_BOTTOM([#ifdef __cplusplus
#include <cassert>
#else
#include <assert.h>
#endif])

# check for required functions and die if they are not found
AC_CHECK_FUNCS([popen pclose mktemp vfprintf],,AC_MSG_ERROR([cannot use required functions]))

# syslogging can be configured with `--enable-syslog'
AC_ARG_ENABLE(syslog, AS_HELP_STRING([--enable-syslog], [whether to use syslog (default is off)]), AC_DEFINE([USE_SYSLOG], 1, [whether to use syslog]))

################################################################################
# checking for programs, functions, etc. used by candy testsuite
################################################################################

# search for Wendy (nice to have for "make check")
AC_PATH_PROG(WENDY, [wendy], not found)

# set up autotest
AM_MISSING_PROG(AUTOM4TE, autom4te)
AC_CONFIG_TESTDIR(tests)

################################################################################
# create configured files (Makefiles, scripts, etc.)
################################################################################

# write files
AC_CONFIG_FILES([Makefile
                 src/Makefile
                 doc/Makefile doc/Doxyfile doc/Doxydetails
                 man/Makefile
                 tests/Makefile tests/package.m4])
AC_CONFIG_FILES([tests/cover.sh], [chmod +x tests/cover.sh])
AC_OUTPUT
