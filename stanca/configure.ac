AC_INIT(Stanca, 0.1-unreleased, stanca@service-technology.org, stanca, http://service-technology.org/stanca)

AC_CONFIG_SRCDIR([src/main.cc])
AC_CONFIG_HEADERS([src/config.h])
AC_CONFIG_AUX_DIR([build-aux])

AC_PREREQ([2.63])
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
AC_ST_PNAPI
AC_ST_VERBOSE

# search for optional tools
AC_PROG_LN_S

AC_PATH_PROG(DOT, [dot], not found)
if test "$DOT" = "not found"; then
  AC_MSG_WARN([Graphviz Dot was not found. See file README.])
  AC_DEFINE_UNQUOTED(CONFIG_DOT, "not found", [Graphviz Dot was not found.])
else
  AC_DEFINE_UNQUOTED(CONFIG_DOT, "\"${DOT}\"", [Graphviz Dot can be used.])
fi
AC_SUBST(DOT)

AC_CONFIG_FILES([Makefile src/Makefile libs/pnapi/Makefile libs/minisat/Makefile tests/Makefile tests/package.m4 maintainer/Makefile man/Makefile doc/Makefile doc/Doxyfile])
AC_CONFIG_FILES([tests/cover.sh], [chmod +x tests/cover.sh])
AC_OUTPUT
