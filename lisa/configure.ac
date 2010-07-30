AC_INIT([Lisa], 1.0-unreleased, lisa@service-technology.org, lisa, http://service-technology.org/lisa)

AC_MSG_NOTICE([ _     _           ])
AC_MSG_NOTICE([| |   (_)___  __ _ ])
AC_MSG_NOTICE([| |   | / __|/ _` |])
AC_MSG_NOTICE([| |___| \__ \ (_| |])
AC_MSG_NOTICE([|_____|_|___/\__,_|])
AC_MSG_NOTICE([Linear programming based structural analysis])
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
AC_ST_AUTOTEST
AC_ST_PNAPI([4.02])
AC_ST_VERBOSE

# search for optional tools
AC_PROG_LIBTOOL

# check for dlopen/ftime (required by lp_solve)
AC_SEARCH_LIBS([dlopen], [dl zrldsrl])
AC_SEARCH_LIBS([ftime], [compat])

# check for required functions and die if they are not found
AC_CHECK_FUNCS([popen pclose],,AC_MSG_ERROR([cannot use required functions]))

# write files
AC_CONFIG_FILES([
	Makefile libs/pnapi/Makefile libs/lp_solve/Makefile src/Makefile
	man/Makefile maintainer/Makefile doc/Makefile doc/Doxyfile
	maintainer/debian/changelog.template maintainer/debian/info
	maintainer/debian/rules maintainer/macports/Portfile tests/Makefile
	tests/package.m4])
AC_CONFIG_FILES([tests/cover.sh], [chmod +x tests/cover.sh])
AC_OUTPUT

