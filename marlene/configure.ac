AC_INIT(Marlene, 0.5-unreleased, christian.gierds@service-technology.org)

AC_CONFIG_SRCDIR([src/marlene.cc])
AC_CONFIG_HEADERS([src/config.h])

AM_INIT_AUTOMAKE([gnits color-tests dist-bzip2 silent-rules])

# look up canonical build name and write it to the config header
AC_CANONICAL_BUILD
AC_DEFINE_UNQUOTED([CONFIG_BUILDSYSTEM], ["$build"], [The platform.])

# check if this package was checkedout from a repository
AM_CONDITIONAL(FROMSVN,test -d ".svn")

CXXFLAGS=""
CFLAGS=""

if test -d ".svn" 
  then 
    configured_CXXFLAGS="-O0 -g -Wall -pedantic"
    configured_CFLAGS="-O0 -g -Wall -pedantic"
  else
    configured_CXXFLAGS="-O3 -DNDEBUG"
    configured_CFLAGS="-O3 -DNDEBUG"
fi
configured_LDFLAGS=""

# check for programs needed to compile
AC_PROG_YACC
AC_PROG_CXX
AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_LEX
AC_PROG_MAKE_SET
AC_PROG_RANLIB
AC_PROG_SED
AM_MISSING_PROG(GENGETOPT, gengetopt)
AM_MISSING_PROG(HELP2MAN, help2man)

# set up autotest
AM_MISSING_PROG(AUTOM4TE, autom4te)
AC_CONFIG_TESTDIR(tests)

# check for required tools
#AC_PATH_PROG(LOLA_FULL, [lola-full], [])
#AC_PATH_PROG(LOLA_FULL1, [lola-full1], [${LOLA_FULL}])
AC_PATH_PROG(WENDY, [wendy], [])
AC_PATH_PROGS(PETRIFY, [petrify4.2 petrify4.1 petrify4.0 petrify], not found)
if test "$PETRIFY" = "not found"; then
  AC_MSG_WARN([Petrify was not found. See file README.])
  AC_DEFINE(CONFIG_PETRIFY, "not found", [Petrify was not found.])
else
  AC_DEFINE_UNQUOTED(CONFIG_PETRIFY, "${PETRIFY}", [Petrify can be used.])
fi
AC_SUBST(PETRIFY)


# needed for the Petri Net API
#AC_DEFINE_UNQUOTED(CONFIG_PETRIFY, "not found", [Petrify is not used.])

# only use installed Petri Net API if this was configured with --enable-pnapi
AC_ARG_ENABLE(pnapi,
  AS_HELP_STRING([--enable-pnapi],[use installed Petri Net API (disabled by default)]),
  enable_pnapi=${enableval}, enable_pnapi="no")
AC_MSG_CHECKING([whether to use installed Petri Net API])
AC_MSG_RESULT($enable_pnapi)
if test "$enable_pnapi" = "yes"; then
  # check if the Petri Net API is already installed
  AM_MISSING_PROG(PKG_CONFIG, pkg-config)
  AC_CHECK_LIB(pnapi, main)
  if test "${ac_cv_lib_pnapi_main}" = "no"; then
    AC_MSG_WARN([Petri Net API was not found!])
    AC_MSG_WARN([Using shiped version from /libs/pnapi.])
  fi
  AM_CONDITIONAL(COMPILE_PNAPI, [test "${ac_cv_lib_pnapi_main}" = "no"])
else
  AM_CONDITIONAL(COMPILE_PNAPI, [true])
fi

# Checks for header files.
AC_FUNC_ALLOCA
AC_CHECK_HEADERS([inttypes.h libintl.h malloc.h stddef.h stdlib.h string.h unistd.h])
AC_HEADER_ASSERT

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT8_T
AC_TYPE_SIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([memset strchr strcspn strdup strspn strtol])
AC_CHECK_FUNCS([popen pclose],,AC_MSG_WARN([Marlene cannot use UNIX pipes.]))
AC_CHECK_FUNCS([mkstemp])



#############################################################################
# anything else
#############################################################################

# Force @builddir@ substitution. Without this src/Makefile.in lacked @builddir@
# substitution on gruenau.informatik.hu-berlin.de with automake 1.9.6.
AC_SUBST(builddir)

#############################################################################
# In the following, command line paraemters for the configure script are
# defined. Each parameter is also written into a Makefile conditional to
# later set compliler flags accordingly. Finally, the result is also written
# into the configuration header "config.h".
#############################################################################

# parameter "--disable-assert"
AC_ARG_ENABLE(assert,
  AS_HELP_STRING([--disable-assert],[disable assertions and verbose debug messages (enabled by default)]),
  [enable_assert=${enableval}], [enable_assert=yes])
AC_MSG_CHECKING([whether to use assertions])
AC_MSG_RESULT($enable_assert)

if test "$enable_assert" = "no"; then
  AC_DEFINE(NDEBUG,1,[Turn off assertions and verbose debug messages.])
fi

# parameter "--enable-64bit"
AC_ARG_ENABLE(64bit,
  AS_HELP_STRING([--enable-64bit],[build for a 64 bit architecture such as x86_64 or ppc64 (disabled by default)]),
  enable_64bit=${enableval}, enable_64bit="no")
AC_MSG_CHECKING([whether to build for a 64 bit architecture])
AC_MSG_RESULT($enable_64bit)

if test "$enable_64bit" = "yes"; then
  configured_CFLAGS=${configured_CFLAGS}" -m64"
  configured_CXXFLAGS=${configured_CXXFLAGS}" -m64"
  configured_LDFLAGS=${configured_LDFLAGS}" -m64"
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
  '--disable-dependency-tracking --enable-universal'])
  fi
  configured_CFLAGS=${configured_CFLAGS}" -isysroot /Developer/SDKs/MacOSX10.5.sdk -arch ppc -arch i386"
  configured_CXXFLAGS=${configured_CXXFLAGS}" -isysroot /Developer/SDKs/MacOSX10.5.sdk -arch ppc -arch i386"
  configured_LDFLAGS=${configured_LDFLAGS}" -isysroot /Developer/SDKs/MacOSX10.5.sdk -arch ppc -arch i386 -mmacosx-version-min=10.4"
fi

# parameter "--enable-win32"
AC_ARG_ENABLE(win32,
  AS_HELP_STRING([--enable-win32],[build a Windows binary that is independent of a local Cygwin installation (disabled by default)]),
  enable_win32=${enableval}, enable_win32="no")
AC_MSG_CHECKING([whether to build Windows MinGW binary])
AC_MSG_RESULT($enable_win32)

if test "$enable_win32" = "yes"; then
  configured_CFLAGS=${configured_CFLAGS}" -mno-cygwin -U _WIN32"
  configured_CXXFLAGS=${configured_CXXFLAGS}" -mno-cygwin -U _WIN32"
  configured_LDFLAGS=${configured_LDFLAGS}" -mno-cygwin -U _WIN32"
fi

# substitute configure Flags in Makefiles
AC_SUBST(configured_CXXFLAGS)
AC_SUBST(configured_CFLAGS)
AC_SUBST(configured_LDFLAGS)

AC_CONFIG_FILES([Makefile
                 libs/pnapi/Makefile
                 src/marlene.conf
                 src/Makefile
                 tests/Makefile
                 tests/package.m4
                 doc/Doxyfile
                 doc/Makefile
                 man/Makefile])
AC_OUTPUT

#############################################################################
# Print summary of compile settings
#############################################################################
echo
echo "-------------- compile settings ---------------"
echo "compile for 64 bit...        $enable_64bit"
echo "compile for Win32...         $enable_win32"
echo "compile for Mac Universal... $enable_universal"
echo "enable assertions...         $enable_assert"
echo "CXXFLAGS... $configured_CXXFLAGS"
echo "CFLAGS...   $configured_CFLAGS"
echo "-----------------------------------------------"
echo

# display a warning in case a required tool has not been found
if test "${WENDY}" = ""; then
  echo
  echo "WARNING: Not all required tools have been found. Please read file"
  echo "         REQUIREMENTS and edit the file src/marlene.conf before"
  echo "         using or installing Marlene."
  echo
fi

