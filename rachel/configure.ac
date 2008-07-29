AC_PREREQ(2.61)
AC_INIT(Rachel, 1.07, rachel@service-technology.org, rachel)
AC_CONFIG_SRCDIR([src/main.cc])
AC_CONFIG_HEADERS(src/config.h)

AM_INIT_AUTOMAKE

AC_PROG_CXX
AM_PROG_LEX
AC_PROG_YACC
AC_PROG_SED

AC_TYPE_SIZE_T

AC_HEADER_ASSERT

AC_CANONICAL_BUILD
AC_DEFINE_UNQUOTED([BUILDSYSTEM], ["$build"], [The platform.])

AC_CHECK_PROG(GENGETOPT, gengetopt, gengetopt)
AC_CHECK_PROG(DOT, dot, dot)
AC_CHECK_PROG(AGPP, ag++, ag++)
AC_CHECK_PROG(ACPP, ac++, ac++)
AC_DEFINE_UNQUOTED(HAVE_DOT, "$DOT", [whether dot was found by configure])
AM_MISSING_PROG(HELP2MAN, help2man)

AC_CONFIG_FILES([Makefile src/Doxyfile src/Makefile man/Makefile doc/Makefile tests/Makefile exp/Makefile])

AC_OUTPUT
