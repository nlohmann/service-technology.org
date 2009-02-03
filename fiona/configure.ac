AC_INIT(Fiona, 3.1, fiona@service-technology.org, fiona)
# remember to also change the version number in `doc/ChangeLog.texi',
# because otherwise `make dist' will fail with `NEWS not updated' message

# the first line should be everything you will ever need to touch
#############################################################################

AC_PREREQ(2.59)
AM_INIT_AUTOMAKE(gnits)

# where to look for source code and how the config header should be called
AC_CONFIG_SRCDIR(src/main.cc)
AC_CONFIG_HEADERS(src/config.h)

# check for generic programs
AC_PROG_CXX
AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_CPP
AC_PROG_EGREP
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_SED
AM_MISSING_PROG(HELP2MAN, help2man)

# look up canonical build name and write it to the config header
AC_CANONICAL_BUILD
AC_DEFINE_UNQUOTED([CONFIG_BUILDSYSTEM], ["$build"], [The platform.])

# needed to compile CUDD on x86_64 machines
AC_CHECK_SIZEOF([void*])

# flags set by the configure script
configured_CFLAGS=""
configured_CXXFLAGS=""
configured_LDFLAGS=""
CUDD_XFLAGS=""


#############################################################################
# check whether to use assertions using the configure script parameter
# "--enable-assert" (standard) and "--disable-assert"
#############################################################################
AC_ARG_ENABLE(assert,
  AS_HELP_STRING([--disable-assert],[disable assertions and verbose debug output (enabled by default)]),
  [enable_assert=${enableval}], [enable_assert=yes])
AC_MSG_CHECKING([whether to use assertions])
AC_MSG_RESULT($enable_assert)
AC_DEFINE_UNQUOTED(CONFIG_ENABLEASSERT, "$enable_assert", [whether to use assertions])

if test "$enable_assert" = "no"; then
  AC_DEFINE(NDEBUG,1,[Turn off assertions and verbose debug messages.])
fi


#############################################################################
# check whether to build for a 64 bit architecture using the configure
# script parameter "--enable-64bit" (standard: off)
#############################################################################
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

# In case we compile for 64 bit (either explicitly with "--using-64bit" or
# implicitly on a x86_64 system), we have to set a preprocessor definition
# "SIZEOF_VOID_P" for CUDD.
# (the variable "$ac_cv_sizeof_voidp" is set by "AC_CHECK_SIZEOF([void*])")
if test "$ac_cv_sizeof_voidp" = "8" -o "$enable_64bit" = "yes"; then
  CUDD_XFLAGS=$CUDD_XFLAGS" -DSIZEOF_VOID_P=8"
fi


#############################################################################
# check whether to build a Mac Universal binary using the configure
# script parameter "--enable-universal" (standard: off)
#############################################################################
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
  configured_CFLAGS=$configured_CFLAGS" -isysroot /Developer/SDKs/MacOSX10.5.sdk -arch ppc -arch i386"
  configured_CXXFLAGS=$configured_CXXFLAGS" -isysroot /Developer/SDKs/MacOSX10.5.sdk -arch ppc -arch i386"
  configured_LDFLAGS=$configured_LDFLAGS" -isysroot /Developer/SDKs/MacOSX10.5.sdk -arch ppc -arch i386 -mmacosx-version-min=10.4"
fi


#############################################################################
# check whether to build a cygwin-independent Win32 binary using the
# configure script parameter "--enable-win32" (standard: off)
#############################################################################
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


#############################################################################
# search for Flex and Bison
#############################################################################
AC_PROG_LEX
if test "$LEX" != "flex"; then
  AC_MSG_WARN([Flex was not found. You will not be able to compile Fiona from scratch. See file README.])
fi

AC_PROG_YACC
if test "$YACC" = "yacc"; then
  AC_MSG_WARN([Bison was not found. You will not be able to compile Fiona from scratch. See file README.])
fi


#############################################################################
# search for Dot and Petrify and write results to file "src/config.h"
#############################################################################
AC_CHECK_PROG(DOT, dot, dot, not found)
if test "$DOT" = "not found"; then
  AC_MSG_WARN([Graphviz Dot was not found. See file README.])
  AC_DEFINE_UNQUOTED(CONFIG_DOT, "not found", [Graphviz Dot was not found.])
else
  AC_DEFINE_UNQUOTED(CONFIG_DOT, "${DOT}", [Graphviz Dot can be used.])
fi
AC_SUBST(DOT)

AC_CHECK_PROGS(PETRIFY, [petrify4.2 petrify4.1 petrify4.0 petrify], not found)
if test "$PETRIFY" = "not found"; then
  AC_MSG_WARN([Petrify was not found. See file README.])
  AC_DEFINE(CONFIG_PETRIFY, "not found", [Petrify was not found.])
else
  AC_DEFINE_UNQUOTED(CONFIG_PETRIFY, "${PETRIFY}", [Petrify can be used.])
fi
AC_SUBST(PETRIFY)


#############################################################################
# check for the "ranlib" tool and signal an error if it is not found
#############################################################################
AC_PROG_RANLIB
if test "$RANLIB" = ":"; then
  AC_MSG_ERROR([ranlib was not found!])
fi
AC_CHECK_PROG(AR, ar, ar, not found)
if test "$AR" = "not found"; then
  AC_MSG_ERROR([ar was not found!])
fi


#############################################################################
# export the configured flags for subsequent Makefiles
#############################################################################
AC_SUBST(CUDD_XFLAGS)
AC_SUBST(configured_CFLAGS)
AC_SUBST(configured_CXXFLAGS)
AC_SUBST(configured_LDFLAGS)


#############################################################################
# anything else
#############################################################################

# Force @builddir@ substitution. Without this src/Makefile.in lacked @builddir@
# substitution on gruenau.informatik.hu-berlin.de with automake 1.9.6.
AC_SUBST(builddir)


#############################################################################
# create configured files (Makefiles, scripts, etc.)
#############################################################################
AC_CONFIG_FILES([Makefile
                 Doxyfile
                 src/Makefile
                 doc/Makefile
                 tests/Makefile
		 tests/regression/Makefile
                 libs/Makefile
                 libs/cudd/Makefile
                 libs/pnapi/Makefile
		 man/Makefile
		 maintainer/Makefile])
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
echo "dot:                         $DOT"
echo "petrify:                     $PETRIFY"
echo "-----------------------------------------------"
echo
echo
echo "Run \"make\" to build fiona (cudd package is built automatically)."
echo "Run \"make win32\" to build a fiona executable that is independent of cygwin1.dll."
echo
