AC_INIT(Anastasia, 1.0-unreleased, anastasia@service-technology.org, anastasia, http://service-technology.org/anastasia)

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

# search for optional tools needed by "make check"
AC_PROG_LIBTOOL

# check for dlopen/ftime (required by lp_solve)
AC_SEARCH_LIBS([dlopen], [dl zrldsrl])
AC_SEARCH_LIBS([ftime], [compat])

# check for required functions and die if they are not found
AC_CHECK_FUNCS([popen pclose],,AC_MSG_ERROR([cannot use pipes]))

# write files
AC_CONFIG_FILES([
	Makefile libs/pnapi/Makefile libs/minisat/Makefile libs/lp_solve/Makefile libs/sara/Makefile src/Makefile src/anastasia.conf doc/Makefile
	doc/Doxyfile man/Makefile tests/Makefile tests/package.m4])
# maintainer/Makefile
#	maintainer/debian/changelog.template maintainer/debian/info
#	maintainer/debian/rules maintainer/macports/Portfile 

AC_CONFIG_FILES([tests/cover.sh], [chmod +x tests/cover.sh])
AC_OUTPUT

