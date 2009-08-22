AC_PREREQ([2.63])
AC_INIT(Safira, 0.1, safira@service-technology.org, safira, http://service-technology.org/safira)
AM_INIT_AUTOMAKE

AC_CONFIG_SRCDIR([src/main.cc])
AC_CONFIG_HEADERS([src/config.h])

# Checks for programs.
AC_PROG_CXX
AM_PROG_CC_C_O
AC_PROG_LEX
AC_PROG_YACC
AC_PROG_SED
AC_PROG_GREP
AC_PROG_LN_S

AM_MISSING_PROG(GENGETOPT, gengetopt)
AM_MISSING_PROG(HELP2MAN, help2man)

# set up autotest
AM_MISSING_PROG(AUTOM4TE, autom4te)
AC_CONFIG_TESTDIR(tests)

AC_PATH_PROG(DOT, [dot], not found)
if test "$DOT" = "not found"; then
  AC_MSG_WARN([Graphviz Dot was not found. See file README.])
  AC_DEFINE_UNQUOTED(CONFIG_DOT, "not found", [Graphviz Dot was not found.])
else
  AC_DEFINE_UNQUOTED(CONFIG_DOT, "${DOT}", [Graphviz Dot can be used.])
fi
AC_SUBST(DOT)

# Minisat needs the Zlib library
AC_CHECK_LIB([z], [main], , AC_MSG_ERROR([Zlib was not found!]))

AC_CONFIG_FILES([Makefile src/Makefile minisat/Makefile tests/Makefile tests/package.m4 man/Makefile doc/Makefile doc/Doxyfile])
AC_CONFIG_FILES([tests/cover.sh], [chmod +x tests/cover.sh])
AC_OUTPUT
