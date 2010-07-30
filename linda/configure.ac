AC_INIT([Linda], 0.6-unreleased, linda@service-technology.org, linda, http://service-technology.org/linda)

AC_MSG_NOTICE([ _     _           _       ])
AC_MSG_NOTICE([| |   (_)_ __   __| | __ _ ])
AC_MSG_NOTICE([| |   | | '_ \ / _` |/ _` |])
AC_MSG_NOTICE([| |___| | | | | (_| | (_| |])
AC_MSG_NOTICE([|_____|_|_| |_|\__,_|\__,_|])
AC_MSG_NOTICE([computing communication fingerprints for a service])
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
