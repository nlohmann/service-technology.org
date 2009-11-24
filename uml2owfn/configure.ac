AC_INIT(UML2oWFN, 2.10, uml2owfn@service-technology.org, uml2owfn, http://service-technology.org/uml2owfn)

# prerequisites for Autoconf and Automake
AC_PREREQ(2.59)
AM_INIT_AUTOMAKE

# Location of main() function and name of config header file.
AC_CONFIG_SRCDIR(src/uml2owfn.cc)
AC_CONFIG_HEADERS(src/config.h)

# Compute the canonical build-system type variable, build, and its three
# individual parts build_cpu, build_vendor, and build_os and write it to
# the config header file
AC_CANONICAL_BUILD
AC_DEFINE_UNQUOTED([BUILDSYSTEM], ["$build"], [The platform.])

# Check for C/C++.
AC_PROG_CXX
AM_PROG_CC_C_O

# set up autotest
AM_MISSING_PROG(AUTOM4TE, autom4te)
AC_CONFIG_TESTDIR(tests)

# flags set by the configure script
configured_CFLAGS=""
configured_CXXFLAGS=""
configured_LDFLAGS=""

############################################################################
# Check for programs.
############################################################################

# GNU parser generators
AC_PROG_YACC
AC_PROG_LEX

# *nix command-line tools for ./doc
AC_PROG_SED
AC_PROG_AWK
AC_PROG_GREP

# Dot
AC_PATH_PROG(DOT, [dot], not found)
if test "$DOT" = "not found"; then
 AC_MSG_WARN([Graphviz dot was not found.])
 AC_DEFINE(HAVE_DOT, 0, [Graphviz dot can not be used on this system.])
else
 AC_DEFINE(HAVE_DOT, 1, [Graphviz dot can be used on this system.])
fi

# Petri net API
AC_PROG_RANLIB


############################################################################
# further compiler flags and definitions.
############################################################################

#############################################################################
# In the following, command line parameters for the configure script are
# defined. Each parameter is also written into a Makefile conditional to
# later set compliler flags accordingly. Finally, the result is also written
# into the configuration header "config.h".
#############################################################################

# parameter "--enable-debug"
AC_ARG_ENABLE(debug,
  AS_HELP_STRING([--enable-debug],[enable verbose debug messages]),
  [enable_debug=${enableval}], [enable_debug="no"])
AC_MSG_CHECKING([whether to compile debug statements])
AC_MSG_RESULT($enable_debug)
AC_DEFINE_UNQUOTED(CONFIG_ENABLEDEBUG, "$enable_debug", [whether to compile debug statements])

if test "$enable_debug" = "yes"; then
  AC_DEFINE(DEBUG, 1, [Compiling with DEBUG statements.])
  configured_CFLAGS=$configured_CFLAGS" -g"
  configured_CXXFLAGS=$configured_CXXFLAGS" -g"
  configured_LDFLAGS=$configured_LDFLAGS" -g"
fi

# allow the configure script to control assertions (just include config.h)
AC_HEADER_ASSERT
AH_BOTTOM([#ifdef __cplusplus
#include <cassert>
#else
#include <assert.h>
#endif])

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

############################################################################
# Set files to generate.
############################################################################
AC_CONFIG_FILES([Makefile 
                 libs/pnapi/Makefile 
                 src/Makefile 
                 tests/Makefile 
                 utils/Makefile  
                 doc/Makefile
                 doc/Doxyfile
                 tests/package.m4])
AC_CONFIG_FILES([tests/cover.sh], [chmod +x tests/cover.sh])

AC_OUTPUT
