AC_INIT([Cora], 0.3-unreleased, cora@service-technology.org, cora, http://service-technology.org/cora)

AC_MSG_NOTICE([  ____                ])
AC_MSG_NOTICE([ / ___|___  _ __ __ _ ])
AC_MSG_NOTICE([| |   / _ \| '__/ _` |])
AC_MSG_NOTICE([| |__| (_) | | | (_| |])
AC_MSG_NOTICE([ \____\___/|_|  \__,_|])
AC_MSG_NOTICE([COverability graphs for Reachability Analysis])
AC_MSG_NOTICE([${PACKAGE_URL}])
AC_MSG_NOTICE([version ${PACKAGE_VERSION}])
AC_MSG_NOTICE()

AC_CONFIG_SRCDIR([src/main.cc])
AC_CONFIG_HEADERS([src/config.h])
AC_CONFIG_AUX_DIR([build-aux])

AM_INIT_AUTOMAKE#(gnits)
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

# check for standard programs needed to compile
AC_PROG_CXX
AM_PROG_CC_C_O
AC_PROG_LEX
AC_PROG_YACC

# use the service-technology.org macros
AC_ST_BASIC
AC_ST_AUTOTEST
AC_ST_PNAPI([4.02])
AC_ST_VERBOSE

# check for functions
AC_CHECK_FUNCS([popen pclose],,AC_MSG_WARN([cannot use pipes -- expect bad performance]))

# check for exact unsigned types
AC_TYPE_UINT8_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T

# write files
AC_CONFIG_FILES([
	Makefile libs/pnapi/Makefile src/Makefile tests/Makefile tests/package.m4 doc/Makefile doc/Doxyfile
	])
AC_CONFIG_FILES([tests/cover.sh], [chmod +x tests/cover.sh])
AC_OUTPUT
