AC_INIT(Rachel, 1.11-unreleased, rachel@service-technology.org, rachel)

#############################################################################
# check for prerequisites, programms, headers, and libraries
#############################################################################

# prerequisites for Autoconf and Automake
AC_PREREQ(2.61)
AM_INIT_AUTOMAKE(gnits)

# where to look for source code and how the config header should be called
AC_CONFIG_SRCDIR([src/main.cc])
AC_CONFIG_HEADERS(src/config.h)

# check for generic programs
AC_PROG_CXX
AM_PROG_CC_C_O
AM_PROG_LEX
AC_PROG_YACC
AC_PROG_SED
AC_PROG_AWK
AC_PROG_GREP

# check for additional programs
AC_CHECK_PROG(GENGETOPT, gengetopt, gengetopt)
AC_CHECK_PROG(DOT, dot, dot)
AC_CHECK_PROG(AGPP, ag++, ag++)
AC_CHECK_PROG(ACPP, ac++, ac++)
AC_DEFINE_UNQUOTED(HAVE_DOT, "$DOT", [whether dot was found by configure])
AM_MISSING_PROG(HELP2MAN, help2man)

# check for libraries
AC_CHECK_HEADERS(libgen.h)
AC_TYPE_SIZE_T

# look up canonical build name and write it to config.h
AC_CANONICAL_BUILD
AC_DEFINE_UNQUOTED([BUILDSYSTEM], ["$build"], [The platform.])

# flags set by the configure script
configured_CFLAGS=""
configured_CXXFLAGS=""
configured_LDFLAGS=""


#############################################################################
# In the following, command line paraemters for the configure script are
# defined. Each parameter is also written into a Makefile conditional to
# later set compliler flags accordingly. Finally, the result is also written
# into the configuration header "config.h".
#############################################################################

# parameter "--enable-assert"
AC_ARG_ENABLE(assert,
  AS_HELP_STRING([--disable-assert],[disable assertions and verbose debug messages (enabled by default)]),
  [enable_assert=${enableval}], [enable_assert=yes])
AC_MSG_CHECKING([whether to use assertions])
AC_MSG_RESULT($enable_assert)
AC_DEFINE_UNQUOTED(CONFIG_ENABLEASSERT, "$enable_assert", [whether to use assertions])

if test "$enable_assert" = "no"; then
  AC_DEFINE(NDEBUG,1,[Turn off assertions and verbose debug messages.])
fi

# parameter "--enable-aspects"
AC_ARG_ENABLE(aspects,
  AS_HELP_STRING([--enable-aspects],[enable AspectC++ aspects for debugging messages (disabled by default)]),
  enable_aspects=${enableval}, enable_aspects="no")
AC_MSG_CHECKING([whether to use aspects])
AC_MSG_RESULT($enable_aspects)
AM_CONDITIONAL(ENABLE_ASPECTS, test "$enable_aspects" = "yes")
AC_DEFINE_UNQUOTED(CONFIG_ENABLEASPECTS, "$enable_aspects", [whether to use aspects])

# parameter "--enable-64bit"
AC_ARG_ENABLE(64bit,
  AS_HELP_STRING([--enable-64bit],[build for a 64 bit architecture such as x86_64 or ppc64 (disabled by default)]),
  enable_64bit=${enableval}, enable_64bit="no")
AC_MSG_CHECKING([whether to build for a 64 bit architecture])
AC_MSG_RESULT($enable_64bit)
AC_DEFINE_UNQUOTED(CONFIG_ENABLE64BIT, "$enable_64bit", [whether to build a 64 Bit binary])

if test "$enable_64bit" = "yes"; then
  configured_CFLAGS=$configured_CFLAGS" -m64"
  configured_CXXFLAGS=$configured_CXXFLAGS" -m64"
  configured_LDFLAGS=$configured_LDFLAGS" -m64"
fi

# parameter "--enable-universal"
AC_ARG_ENABLE(universal,
  AS_HELP_STRING([--enable-universal],[build a Mac Universal binary which is executable on Intel and Power PC platforms (disabled by default)]),
  enable_universal=${enableval}, enable_universal="no")
AC_MSG_CHECKING([whether to build Mac universal binary])
AC_MSG_RESULT($enable_universal)
AC_DEFINE_UNQUOTED(CONFIG_ENABLEUNIVERSAL, "$enable_universal", [whether to build a Mac universal binary])

if test "$enable_universal" = "yes"; then
  if test "$enable_dependency_tracking" != no ; then
    AC_MSG_ERROR([Please re-run configure with these options:
  `--disable-dependency-tracking --enable-universal'])
  fi
  configured_CFLAGS=$configured_CFLAGS" -isysroot /Developer/SDKs/MacOSX10.4u.sdk -arch ppc -arch i386"
  configured_CXXFLAGS=$configured_CXXFLAGS" -isysroot /Developer/SDKs/MacOSX10.4u.sdk -arch ppc -arch i386"
  configured_LDFLAGS=$configured_LDFLAGS" -isysroot /Developer/SDKs/MacOSX10.4u.sdk -arch ppc -arch i386 -mmacosx-version-min=10.4"
fi

# parameter "--enable-win32"
AC_ARG_ENABLE(win32,
  AS_HELP_STRING([--enable-win32],[build a Windows binary that is independent of a local Cygwin installation (disabled by default)]),
  enable_win32=${enableval}, enable_win32="no")
AC_MSG_CHECKING([whether to build Windows MinGW binary])
AC_MSG_RESULT($enable_win32)
AC_DEFINE_UNQUOTED(CONFIG_ENABLEWIN32, "$enable_win32", [whether to build a Cygwin-independent Windows binary])

if test "$enable_win32" = "yes"; then
  configured_CFLAGS=$configured_CFLAGS" -mno-cygwin -U _WIN32"
  configured_CXXFLAGS=$configured_CXXFLAGS" -mno-cygwin -U _WIN32"
  configured_LDFLAGS=$configured_LDFLAGS" -mno-cygwin -U _WIN32"
fi

# export flags
AC_SUBST(configured_CFLAGS)
AC_SUBST(configured_CXXFLAGS)
AC_SUBST(configured_LDFLAGS)


#############################################################################
# create output files
#############################################################################

AC_CONFIG_FILES([Makefile src/Doxyfile src/Makefile man/Makefile doc/Makefile tests/Makefile exp/Makefile maintainer/Makefile maintainer/debian/changelog.template])

AC_OUTPUT
