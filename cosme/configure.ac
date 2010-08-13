AC_INIT([Cosme], 1.0, cosme@service-technology.org, cosme, http://service-technology.org/cosme)

AC_MSG_NOTICE([  ____                         ])
AC_MSG_NOTICE([ / ___|___  ___ _ __ ___   ___ ])
AC_MSG_NOTICE([| |   / _ \\/ __| '_ ` _ \\ / _ \\])
AC_MSG_NOTICE([| |__| (_) \__ \ | | | | |  __/])
AC_MSG_NOTICE([ \____\___/|___/_| |_| |_|\___|])
AC_MSG_NOTICE([checking simulation, matching, and equivalence])
AC_MSG_NOTICE([${PACKAGE_URL}])
AC_MSG_NOTICE([version ${PACKAGE_VERSION}])
AC_MSG_NOTICE()

AC_CONFIG_SRCDIR([src/main.cc])
AC_CONFIG_HEADERS([src/config.h])
AC_CONFIG_AUX_DIR([build-aux])

AM_INIT_AUTOMAKE(gnits)
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

# check for standard programs needed to compile
AC_PROG_CXX
AM_PROG_CC_C_O
AC_PROG_LEX
AC_PROG_YACC

# use the service-technology.org macros
AC_ST_BASIC
AC_ST_LOLA([1.14])
AC_ST_AUTOTEST
AC_ST_PNAPI([4.02])
AC_ST_VERBOSE

# search for optional tools needed by "make check"
AC_PATH_PROG(VALGRIND,    [valgrind], not found)

# check for required functions and die if they are not found
AC_CHECK_FUNCS([popen pclose],,AC_MSG_ERROR([cannot use required functions]))

# write files
AC_CONFIG_FILES([Makefile libs/pnapi/Makefile src/Makefile doc/Doxyfile tests/Makefile tests/package.m4])
AC_CONFIG_FILES([tests/cover.sh], [chmod +x tests/cover.sh])
AC_OUTPUT
