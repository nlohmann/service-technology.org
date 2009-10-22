AC_INIT([Marlene],[1.2-unreleased],[christian.gierds@service-technology.org])

AC_CONFIG_SRCDIR([src/marlene.cc])
AC_CONFIG_HEADERS([src/config.h])

AM_INIT_AUTOMAKE([gnits])

# look up canonical build name and write it to the config header
AC_CANONICAL_BUILD
AC_DEFINE_UNQUOTED([CONFIG_BUILDSYSTEM], ["$build"], [The platform.])

# check if this package was checkedout from a repository
AM_CONDITIONAL(FROMSVN,test -d ".svn")

# check for programs needed to compile
AC_PROG_YACC
AC_PROG_CXX
AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_LEX
AC_PROG_MAKE_SET
AC_PROG_RANLIB
AC_PROG_SED
AC_PROG_GREP
AC_PROG_AWK

# check for additional programs
AM_MISSING_PROG(GENGETOPT, gengetopt)
AM_MISSING_PROG(HELP2MAN, help2man)

# export tools needed to determine memory consumption
AC_DEFINE_UNQUOTED(TOOL_AWK, "${AWK}", [awk])
AC_DEFINE_UNQUOTED(TOOL_GREP, "${GREP}", [grep])

# set up autotest
AM_MISSING_PROG(AUTOM4TE, autom4te)
AC_CONFIG_TESTDIR(tests)

# store the SVN revision number
AC_SUBST(VERSION_SVN, "`svnversion -n 2> /dev/null`")
AC_DEFINE_UNQUOTED(VERSION_SVN, "${VERSION_SVN}", [The SVN revision.])

# check for required tools
AC_PATH_PROG(LOLA_STATESPACE, [lola-statespace], [])
#AC_PATH_PROG(LOLA_FULL1, [lola-full1], [${LOLA_FULL}])
AC_PATH_PROG(WENDY, [wendy], [])
AC_PATH_PROG(CANDY, [candy], [])
AC_PATH_PROG(VALGRIND, [valgrind], not found)
AC_PATH_PROG(DIFF, [diff], not found)
AC_PATH_PROGS(PETRIFY, [petrify4.2 petrify4.1 petrify4.0 petrify], "not_found")
if test "$PETRIFY" = "not found"; then
  AC_MSG_WARN([Petrify was not found. See file README.])
  AC_DEFINE(CONFIG_PETRIFY, "not found", [Petrify was not found.])
else
  AC_DEFINE_UNQUOTED(CONFIG_PETRIFY, "${PETRIFY}", [Petrify can be used.])
fi
AC_SUBST(PETRIFY)


AC_PATH_PROGS(GENET, [genet], not_found)
if test "$GENET" = "not_found"; then
  AC_MSG_WARN([GENET not found. See file README.])
  AC_DEFINE(CONFIG_GENET, "not_found", [Genet was not found.])
else
  AC_DEFINE_UNQUOTED(CONFIG_GENET, "${GENET}", [Genet can be used.])
fi
AC_SUBST(GENET)


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

# enable Windows temp files
AC_ARG_ENABLE(windowstemp,
              AS_HELP_STRING([--enable-windowstemp],[sets standard temp file for Marlene to %TEMP%/marlene-XXXXXX (should only set when compiling for pure Win32 binary)]),
              enable_windowstemp=${enableval}, enable_windowstemp="no")
AC_MSG_CHECKING([whether to use Windows temp folder])
AC_MSG_RESULT($enable_windowstemp)
if test "$enable_windowstemp" = "yes" ; then
    TEMPFOLDER=`cygpath --windows $TEMP`
else
    TEMPFOLDER=/tmp/marlene-XXXXXX
fi

AC_SUBST(TEMPFOLDER)

AC_MSG_CHECKING([weather to be sunny])
AC_MSG_RESULT([better should be])

# Checks for header files.
AC_FUNC_ALLOCA
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS([inttypes.h libintl.h malloc.h stddef.h stdlib.h string.h unistd.h])

# allow the configure script to control assertions (just include config.h)
AC_HEADER_ASSERT
AH_BOTTOM([#ifdef __cplusplus
#include <cassert>
#else
#include <assert.h>
#endif
])


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
AC_CHECK_FUNCS([mkstemp vfprintf],,AC_MSG_WARN([cannot use required functions]))



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
                 man/Makefile
                 maintainer/Makefile])
AC_CONFIG_FILES([tests/cover.sh], [chmod +x tests/cover.sh])
AC_OUTPUT


# display a warning in case a required tool has not been found
if test "${WENDY}" = ""; then
  echo
  echo "WARNING: Not all required tools have been found. Please read file"
  echo "         REQUIREMENTS and edit the file src/marlene.conf before"
  echo "         using or installing Marlene."
  echo
fi

