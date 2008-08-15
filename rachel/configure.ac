AC_PREREQ(2.61)
AC_INIT(Rachel, 1.10-unreleased, rachel@service-technology.org, rachel)
AC_CONFIG_SRCDIR([src/main.cc])
AC_CONFIG_HEADERS(src/config.h)

AM_INIT_AUTOMAKE

AC_PROG_CXX
AM_PROG_CC_C_O
AM_PROG_LEX
AC_PROG_YACC
AC_PROG_SED
AC_PROG_AWK
AC_PROG_GREP

AC_HEADER_ASSERT
AC_CHECK_HEADERS(pthread.h)
AC_TYPE_SIZE_T

AC_CANONICAL_BUILD
AC_DEFINE_UNQUOTED([BUILDSYSTEM], ["$build"], [The platform.])

AC_CHECK_PROG(GENGETOPT, gengetopt, gengetopt)
AC_CHECK_PROG(DOT, dot, dot)
AC_CHECK_PROG(AGPP, ag++, ag++)
AC_CHECK_PROG(ACPP, ac++, ac++)
AC_DEFINE_UNQUOTED(HAVE_DOT, "$DOT", [whether dot was found by configure])
AM_MISSING_PROG(HELP2MAN, help2man)


# the configure script can take the parameter "--enable-aspects" which is
# later used in src/Makefile.am
# (http://www.gnu.org/software/libtool/manual/autoconf/Package-Options.html)
AC_ARG_ENABLE(aspects,
[  --enable-aspects   turn on aspects (standard; turn them off with --disable-aspects)],
[case "${enableval}" in
  yes) aspects=yes ;;
  no)  aspects=no ;;
  *) AC_MSG_ERROR(bad value ${enableval} for --enable-aspects) ;;
esac],[aspects=no])
AM_CONDITIONAL(ASPECTS, test x$aspects = xyes)
AC_MSG_CHECKING([whether to use aspects])
AC_MSG_RESULT($aspects)
AC_DEFINE_UNQUOTED(CONFIG_ASPECTS, "$aspects", [whether to use aspects])


# the configure script can take the parameter "--enable-universal" which is
# later used in src/Makefile.am
# (http://www.gnu.org/software/libtool/manual/autoconf/Package-Options.html)
AC_ARG_ENABLE(universal,
[  --enable-universal   build Mac universal binary],
[case "${enableval}" in
  yes) universal=yes
       AM_INIT_AUTOMAKE(no-dependencies) ;;
  no)  universal=no ;;
  *) AC_MSG_ERROR(bad value ${enableval} for --enable-universal) ;;
esac],[universal=no])
AM_CONDITIONAL(UNIVERSAL, test x$universal = xyes)
AC_MSG_CHECKING([whether to build Mac universal binary])
AC_MSG_RESULT($universal)
AC_DEFINE_UNQUOTED(CONFIG_UNIVERSAL, "$universal", [whether to build Mac universal binary])


# the configure script can take the parameter "--enable-win32" which is
# later used in src/Makefile.am
# (http://www.gnu.org/software/libtool/manual/autoconf/Package-Options.html)
AC_ARG_ENABLE(win32,
[  --enable-win32   build Windows MinGW binary],
[case "${enableval}" in
  yes) win32=yes ;;
  no)  win32=no ;;
  *) AC_MSG_ERROR(bad value ${enableval} for --enable-win32) ;;
esac],[win32=no])
AM_CONDITIONAL(WIN32, test x$win32 = xyes)
AC_MSG_CHECKING([whether to build Windows MinGW binary])
AC_MSG_RESULT($win32)
AC_DEFINE_UNQUOTED(CONFIG_WIN32, "$win32", [whether to build Windows MinGW binary])


# the configure script can take the parameter "--enable-use64bit" which is
# later used in src/Makefile.am
# (http://www.gnu.org/software/libtool/manual/autoconf/Package-Options.html)
AC_ARG_ENABLE(use64bit,
[  --enable-use64bit   build a 64 Bit binary],
[case "${enableval}" in
  yes) use64bit=yes ;;
  no)  use64bit=no ;;
  *) AC_MSG_ERROR(bad value ${enableval} for --enable-use64bit) ;;
esac],[use64bit=no])
AM_CONDITIONAL(USE64BIT, test x$use64bit = xyes)
AC_MSG_CHECKING([whether to build a 64 Bit binary])
AC_MSG_RESULT($use64bit)
AC_DEFINE_UNQUOTED(CONFIG_USE64BIT, "$use64bit", [whether to build a 64 Bit binary])


AC_CONFIG_FILES([Makefile src/Doxyfile src/Makefile man/Makefile doc/Makefile tests/Makefile exp/Makefile maintainer/Makefile maintainer/debian/changelog.template])

AC_OUTPUT
