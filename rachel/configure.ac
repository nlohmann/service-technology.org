AC_PREREQ(2.61)
AC_INIT(Rachel, 1.09, rachel@service-technology.org, rachel)
AC_CONFIG_SRCDIR([src/main.cc])
AC_CONFIG_HEADERS(src/config.h)

AM_INIT_AUTOMAKE

AC_PROG_CXX
AM_PROG_LEX
AC_PROG_YACC
AC_PROG_SED
AC_PROG_AWK
AC_PROG_GREP

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


# the configure script can take the parameter "--enable-aspects" which is
# late used in src/Makefile.am
# (http://www.gnu.org/software/libtool/manual/autoconf/Package-Options.html)
AC_ARG_ENABLE(aspects,
[  --enable-aspects   turn on aspects (standard; turn them off with --disable-aspects)],
[case "${enableval}" in
  yes) aspects=true ;;
  no)  aspects=false ;;
  *) AC_MSG_ERROR(bad value ${enableval} for --enable-aspects) ;;
esac],[aspects=true])
AM_CONDITIONAL(ASPECTS, test x$aspects = xtrue)
AC_MSG_CHECKING([whether aspects should be used])
AC_MSG_RESULT($aspects)


AC_CONFIG_FILES([Makefile src/Doxyfile src/Makefile man/Makefile doc/Makefile tests/Makefile exp/Makefile maintainer/Makefile maintainer/debian/changelog.template])

AC_OUTPUT
