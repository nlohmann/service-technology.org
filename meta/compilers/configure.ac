AC_INIT(compilers, 1.00-unreleased, compilers@service-technology.org, [fiona2wendy], http://service-technology.org/compilers)

# the first line should be everything you will ever need to touch
#############################################################################

AC_PREREQ(2.59)
AM_INIT_AUTOMAKE(gnits)
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

# where to look for source code and how the config header should be called
AC_CONFIG_SRCDIR(src/main.cc)
AC_CONFIG_HEADERS(src/config.h)

# check for generic programs
AC_PROG_CXX
AM_PROG_CC_C_O
AC_PROG_LEX
AC_PROG_YACC
AC_PROG_SED
AC_PROG_GREP

# allow the configure script to control assertions (just include config.h)
AC_HEADER_ASSERT
AH_BOTTOM([#ifdef __cplusplus
#include <cassert>
#else
#include <assert.h>
#endif])

# check for additional programs
AM_MISSING_PROG(GENGETOPT, gengetopt)
AM_MISSING_PROG(HELP2MAN, help2man)


# set up autotest
AM_MISSING_PROG(AUTOM4TE, autom4te)
AC_CONFIG_TESTDIR(tests)

# search for Fiona and Wendy (nice to have for "make check")
AC_PATH_PROG(FIONA, [fiona], not found)
AC_PATH_PROG(WENDY, [wendy], not found)

#############################################################################
# create configured files (Makefiles, scripts, etc.)
#############################################################################
AC_CONFIG_FILES([Makefile
                 src/Makefile
                 maintainer/Makefile maintainer/debian/changelog.template maintainer/debian/info maintainer/debian/README.Debian.template maintainer/debian/rules maintainer/macports/Portfile
                 tests/Makefile tests/package.m4
                 man/Makefile])
AC_CONFIG_FILES([tests/cover.sh], [chmod +x tests/cover.sh])
AC_OUTPUT

