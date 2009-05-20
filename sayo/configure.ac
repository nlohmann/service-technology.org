AC_INIT(sayo, 0.2, sayo@service-technology.org, sayo)

# the first line should be everything you will ever need to touch
#############################################################################

AC_PREREQ(2.59)
AM_INIT_AUTOMAKE(gnits)

# where to look for source code and how the config header should be called
AC_CONFIG_SRCDIR(src/main.cc)
AC_CONFIG_HEADERS(src/config.h)

# check for generic programs
AC_PROG_CXX
AC_PROG_CPP
AM_PROG_CC_C_O
AC_PROG_SED
AC_PROG_GREP

# flags set by the configure script
configured_CFLAGS=""
configured_CXXFLAGS=""
configured_LDFLAGS=""

# check for functions
AC_CHECK_FUNCS([popen pclose])

# check for additional programs
AM_MISSING_PROG(GENGETOPT, gengetopt)
AM_MISSING_PROG(HELP2MAN, help2man)

# set up autotest
AM_MISSING_PROG(AUTOM4TE, autom4te)
AC_CONFIG_TESTDIR(tests)


#############################################################################
# search for Flex and Bison
#############################################################################
AC_PROG_LEX
if test "$LEX" != "flex"; then
  AC_MSG_WARN([Flex was not found. You will not be able to compile from scratch. See file README.])
  AM_MISSING_PROG(LEX, flex)
fi

AC_PROG_YACC
if test "$YACC" = "yacc"; then
  AC_MSG_WARN([Bison was not found. You will not be able to compile from scratch. See file README.])
  AM_MISSING_PROG(YACC, bison)
fi


#############################################################################
# In the following, command line paraemters for the configure script are
# defined. Each parameter is also written into a Makefile conditional to
# later set compliler flags accordingly. Finally, the result is also written
# into the configuration header "config.h".
#############################################################################

# parameter "--enable-64bit"
AC_ARG_ENABLE(64bit,
  AS_HELP_STRING([--enable-64bit],[build for a 64 bit architecture such as x86_64 or ppc64 (disabled by default)]),
  enable_64bit=${enableval}, enable_64bit="no")
AC_MSG_CHECKING([whether to build for a 64 bit architecture])
AC_MSG_RESULT($enable_64bit)

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

if test "$enable_universal" = "yes"; then
  if test "$enable_dependency_tracking" != no ; then
    AC_MSG_ERROR([Please re-run configure with these options:
  `--disable-dependency-tracking --enable-universal'])
  fi
  configured_CFLAGS=$configured_CFLAGS" -isysroot /Developer/SDKs/MacOSX10.5.sdk -arch ppc -arch i386"
  configured_CXXFLAGS=$configured_CXXFLAGS" -isysroot /Developer/SDKs/MacOSX10.5.sdk -arch ppc -arch i386"
  configured_LDFLAGS=$configured_LDFLAGS" -isysroot /Developer/SDKs/MacOSX10.5.sdk -arch ppc -arch i386 -mmacosx-version-min=10.4"
fi

# parameter "--enable-win32"
AC_ARG_ENABLE(win32,
  AS_HELP_STRING([--enable-win32],[build a Windows binary that is independent of a local Cygwin installation (disabled by default)]),
  enable_win32=${enableval}, enable_win32="no")
AC_MSG_CHECKING([whether to build Windows MinGW binary])
AC_MSG_RESULT($enable_win32)

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
# create configured files (Makefiles, scripts, etc.)
#############################################################################
AC_CONFIG_FILES([Makefile
                 doc/Makefile doc/Doxyfile
                 src/Makefile
                 maintainer/Makefile
                 tests/Makefile tests/package.m4
                 man/Makefile])
AC_OUTPUT
