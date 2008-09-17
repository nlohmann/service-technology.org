# Process this file with autoconf to produce a configure script.
AC_PREREQ(2.59)
AC_INIT(Petri Net API, 1.00, stephan.mennicke@uni-rostock.de, pnapi)

AC_CONFIG_SRCDIR(src/petrinet.cc)
AC_CONFIG_HEADERS(src/config.h)

AM_INIT_AUTOMAKE

# Checks for programs.
AC_PROG_CXX
AC_PROG_CPP
AC_PROG_RANLIB

AC_CHECK_PROG(DOT, dot, dot, not found)
if test "$DOT" = "not found"; then
 AC_MSG_WARN([Graphviz dot was not found.])
 AC_DEFINE(HAVE_DOT, 0, [Dot can not be used on this system.])
else
 AC_DEFINE(HAVE_DOT, 1, [Dot can be used on this system.])
fi

# search for Petrify
AC_CHECK_PROGS(PETRIFY, [petrify4.2 petrify4.1 petrify4.0 petrify], not found)
if test "$PETRIFY" = "not found"; then
 AC_MSG_WARN([Petrify was not found.])
 AC_DEFINE(HAVE_PETRIFY, "not found", [Petrify can not be used on this system.])
else
 AC_DEFINE_UNQUOTED(HAVE_PETRIFY, "${PETRIFY}", [Petrify can be used on this system.])
fi

AC_CONFIG_FILES([Makefile
                 src/Makefile
                 tests/Makefile])
AC_OUTPUT
