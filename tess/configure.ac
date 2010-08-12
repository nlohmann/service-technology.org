AC_INIT(Tess, 1.0-unreleased, tess@service-technology.org, tess, http://service-technology.org/tess)

AC_MSG_NOTICE([ _____             ])
AC_MSG_NOTICE([|_   _|__  ___ ___ ])
AC_MSG_NOTICE([  | |/ _ \/ __/ __|])
AC_MSG_NOTICE([  | |  __/\\__ \\__ \\])
AC_MSG_NOTICE([  |_|\\___||___/___/])
AC_MSG_NOTICE([selecting test cases for services])
AC_MSG_NOTICE([http://service-technology.org/tess])
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

AC_PATH_PROG(DOT, [dot], not found)
if test "$DOT" = "not found"; then
  AC_MSG_WARN([Graphviz Dot was not found. See file README.])
  AC_DEFINE_UNQUOTED(CONFIG_DOT, "not found", [Graphviz Dot was not found.])
else
  AC_DEFINE_UNQUOTED(CONFIG_DOT, "${DOT}", [Graphviz Dot can be used.])
fi
AC_SUBST(DOT)

AC_CONFIG_FILES([Makefile src/Makefile tests/Makefile tests/package.m4 man/Makefile doc/Makefile doc/Doxyfile])
AC_CONFIG_FILES([tests/cover.sh], [chmod +x tests/cover.sh])
AC_OUTPUT
