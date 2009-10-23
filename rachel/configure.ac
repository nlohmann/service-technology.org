AC_INIT(Rachel, 1.14-unreleased, rachel@service-technology.org, rachel, http://service-technology.org/rachel)

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
AM_MISSING_PROG(GENGETOPT, gengetopt)
AC_DEFINE_UNQUOTED(CONFIG_DOT, "$DOT", [whether dot was found by configure])
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

# allow the configure script to control assertions (just include config.h)
AC_HEADER_ASSERT
AH_BOTTOM([#ifdef __cplusplus
#include <cassert>
#else
#include <assert.h>
#endif])

# look up canonical build name and write it to config.h
AC_CANONICAL_BUILD
AC_DEFINE_UNQUOTED([CONFIG_BUILDSYSTEM], ["$build"], [The platform.])



#############################################################################
# search for Dot and Petrify and write results to file "src/config.h"
#############################################################################
AC_PATH_PROG(DOT, [dot], not found)
if test "$DOT" = "not found"; then
  AC_MSG_WARN([Graphviz Dot was not found. See file README.])
  AC_DEFINE_UNQUOTED(CONFIG_DOT, "not found", [Graphviz Dot was not found.])
else
  AC_DEFINE_UNQUOTED(CONFIG_DOT, "${DOT}", [Graphviz Dot can be used.])
fi
AC_SUBST(DOT)


#############################################################################
# create output files
#############################################################################

AC_CONFIG_FILES([Makefile
                 doc/Doxyfile doc/Doxydetails doc/Makefile
                 exp/Makefile
                 maintainer/Makefile maintainer/debian/changelog.template maintainer/debian/info maintainer/debian/README.Debian.template maintainer/debian/rules maintainer/macports/Portfile
                 man/Makefile
                 src/Makefile
                 tests/Makefile tests/package.m4])
AC_CONFIG_FILES([tests/cover.sh], [chmod +x tests/cover.sh])
AC_OUTPUT
