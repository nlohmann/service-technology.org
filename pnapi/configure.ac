AC_INIT(Petri Net API, 1.00, pnapi@service-technology.org, pnapi)

AM_INIT_AUTOMAKE
AC_PREREQ(2.59)

AC_CONFIG_SRCDIR(src/petrinet.cc)
AC_CONFIG_HEADERS(src/config.h)

AC_PROG_CXX
AC_PROG_CPP
AC_PROG_RANLIB
AC_PROG_LEX
AC_PROG_YACC

AC_CHECK_PROG(DOT, dot, dot, not found)
if test "$DOT" = "not found"; then
 AC_MSG_WARN([Graphviz dot was not found.])
 AC_DEFINE(HAVE_DOT, 0, [Dot can not be used on this system.])
else
 AC_DEFINE(HAVE_DOT, 1, [Dot can be used on this system.])
fi

AC_CONFIG_FILES([Makefile
                 Doxyfile
                 src/Makefile
                 tests/Makefile])
AC_OUTPUT
