AC_INIT([Yasmina], 2.0-unreleased, yasmina@service-technology.org, yasmina, http://service-technology.org/yasmina)

AC_MSG_NOTICE([__   __                  _             ])
AC_MSG_NOTICE([\ \ / /_ _ ___ _ __ ___ (_)_ __   __ _ ])
AC_MSG_NOTICE([ \ V / _` / __| '_ ` _ \| | '_ \ / _` |])
AC_MSG_NOTICE([  | | (_| \__ \ | | | | | | | | | (_| |])
AC_MSG_NOTICE([  |_|\__,_|___/_| |_| |_|_|_| |_|\__,_|])
AC_MSG_NOTICE([checking compatibility using integer linear programming])
AC_MSG_NOTICE([http://service-technology.org/yasmina])
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
AC_PROG_RANLIB

# use the service-technology.org macros
AC_ST_BASIC
AC_ST_AUTOTEST
AC_ST_PNAPI([4.02])
AC_ST_VERBOSE

# check if we can handle dynamic libs
AC_DISABLE_SHARED
AC_PROG_LIBTOOL
AC_CONFIG_MACRO_DIR([m4])

# check and set exact unsigned types
AC_TYPE_UINT8_T
AC_TYPE_INT64_T

# search for optional tools needed by "make check"
AC_PATH_PROG(DIFF, [diff], not found)

# needed for the Petri Net API
AC_DEFINE_UNQUOTED(CONFIG_PETRIFY, "not found", [Petrify is not used.])

# check for dlopen/ftime (required by lp_solve)
AC_SEARCH_LIBS([dlopen], [dl zrldsrl])
AC_SEARCH_LIBS([ftime], [compat])

# check for functions
AC_CHECK_FUNCS([popen pclose],,AC_MSG_WARN([cannot use popen or pclose]))

AC_CONFIG_FILES([Makefile 
		libs/pnapi/Makefile 
		libs/lp_solve/Makefile 
		src/Makefile 
		doc/Makefile doc/Doxyfile
		man/Makefile
		tests/Makefile tests/package.m4])
AC_CONFIG_FILES([tests/cover.sh], [chmod +x tests/cover.sh])


AC_OUTPUT
