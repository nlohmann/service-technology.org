AC_INIT(Safira, 2.0, safira@service-technology.org, safira, http://service-technology.org/safira)

AC_MSG_NOTICE([ ____         __ _           ])
AC_MSG_NOTICE([/ ___|  __ _ / _(_)_ __ __ _ ])
AC_MSG_NOTICE([\___ \ / _` | |_| | '__/ _` |])
AC_MSG_NOTICE([ ___) | (_| |  _| | | | (_| |])
AC_MSG_NOTICE([|____/ \__,_|_| |_|_|  \__,_|])
AC_MSG_NOTICE([implementing set algebra for service behavior])
AC_MSG_NOTICE([http://service-technology.org/safira])
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
AC_ST_VERBOSE

# search for optional tools
AC_PROG_LN_S
AC_PROG_RANLIB

AC_CHECK_HEADERS([sys/time.h sys/resource.h],,AC_MSG_ERROR([cannot use required headers]))

AC_PATH_PROG(DOT, [dot], not found)
if test "$DOT" = "not found"; then
  AC_MSG_WARN([Graphviz Dot was not found. See file README.])
  AC_DEFINE_UNQUOTED(CONFIG_DOT, "not found", [Graphviz Dot was not found.])
else
  AC_DEFINE_UNQUOTED(CONFIG_DOT, "${DOT}", [Graphviz Dot can be used.])
fi
AC_SUBST(DOT)

AC_CONFIG_FILES([Makefile src/Makefile libs/minisat/Makefile tests/Makefile tests/package.m4 man/Makefile doc/Makefile doc/Doxyfile])
AC_CONFIG_FILES([tests/cover.sh], [chmod +x tests/cover.sh])
AC_OUTPUT
