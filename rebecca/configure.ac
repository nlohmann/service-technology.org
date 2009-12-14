AC_INIT([Rebecca], 1.0-unreleased, rebecca@service-technology.org, rebecca, http://service-technology.org/rebecca)

AC_MSG_NOTICE([ ____      _                        ])
AC_MSG_NOTICE([|  _ \ ___| |__   ___  ___ ___ __ _ ])
AC_MSG_NOTICE([| |_) / _ \ '_ \ / _ \/ __/ __/ _` |])
AC_MSG_NOTICE([|  _ <  __/ |_) |  __/ (_| (_| (_| |])
AC_MSG_NOTICE([|_| \_\___|_.__/ \___|\___\___\__,_|])
AC_MSG_NOTICE([realizing service choreographies])
AC_MSG_NOTICE([http://service-technology.org/rebecca])
AC_MSG_NOTICE([version ${PACKAGE_VERSION}])
AC_MSG_NOTICE()

AC_CONFIG_SRCDIR([src/main.cc])
AC_CONFIG_HEADERS([src/config.h])

AM_INIT_AUTOMAKE(gnits)
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

# check for programs needed to compile
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

# set up autotest
AM_MISSING_PROG(AUTOM4TE, autom4te)
AC_CONFIG_TESTDIR(tests)

# store the SVN revision number
AC_DEFINE_UNQUOTED(VERSION_SVN, "`svnversion -n 2> /dev/null`", [The SVN revision.])

# allow the configure script to control assertions
AC_HEADER_ASSERT

# look up canonical build name and write it to config.h
AC_CANONICAL_BUILD
AC_DEFINE_UNQUOTED([CONFIG_BUILDSYSTEM], ["$build"], [The platform.])

# check for functions
AC_CHECK_FUNCS([vfprintf],,AC_MSG_WARN([cannot use required functions]))

# write files
AC_CONFIG_FILES([
	Makefile src/Makefile man/Makefile tests/Makefile doc/Makefile doc/Doxyfile tests/package.m4])
AC_CONFIG_FILES([tests/cover.sh], [chmod +x tests/cover.sh])
AC_OUTPUT
