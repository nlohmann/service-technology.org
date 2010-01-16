AC_INIT([GNU BPEL2oWFN],[2.04-unreleased],[bpel2owfn@service-technology.org],[bpel2owfn],[http://service-technology.org/bpel2owfn])

AC_MSG_NOTICE([ ____  ____  _____ _     ____    __        _______ _   _ ])
AC_MSG_NOTICE([| __ )|  _ \| ____| |   |___ \ __\ \      / /  ___| \ | |])
AC_MSG_NOTICE([|  _ \| |_) |  _| | |     __) / _ \ \ /\ / /| |_  |  \| |])
AC_MSG_NOTICE([| |_) |  __/| |___| |___ / __/ (_) \ V  V / |  _| | |\  |])
AC_MSG_NOTICE([|____/|_|   |_____|_____|_____\___/ \_/\_/  |_|   |_| \_|])
AC_MSG_NOTICE([translating WS-BPEL processes into Petri net models])
AC_MSG_NOTICE([http://service-technology.org/bpel2owfn])
AC_MSG_NOTICE([http://www.gnu.org/software/bpel2owfn])
AC_MSG_NOTICE([version ${PACKAGE_VERSION}])
AC_MSG_NOTICE()


#############################################################################
# check for prerequisites, programms, headers, and libraries
#############################################################################

AM_INIT_AUTOMAKE(gnits)
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

# where to look for source code and how the config header should be called
AC_CONFIG_SRCDIR(src/main.cc)
AC_CONFIG_HEADERS(src/config.h)

# check for generic programs
AC_PROG_CXX
AC_PROG_YACC
AC_PROG_LEX
AC_PROG_SED
AM_PROG_CC_C_O

# check for additional programs
AM_MISSING_PROG(HELP2MAN, help2man)
AM_MISSING_PROG(GENGETOPT, gengetopt)
AM_MISSING_PROG(KC, kc++)
AC_PATH_PROG(DOT, [dot], not found)
if test "$DOT" = "not found"; then
 AC_DEFINE(HAVE_DOT, 0, [Graphviz dot can not be used on this system.])
else
 AC_DEFINE(HAVE_DOT, 1, [Graphviz dot can be used on this system.])
fi

# set up autotest
AM_MISSING_PROG(AUTOM4TE, autom4te)
AC_CONFIG_TESTDIR(tests)

# store the SVN revision number
AC_DEFINE_UNQUOTED(VERSION_SVN, "`svnversion -n 2> /dev/null`", [The SVN revision.])

# check for header to color console output
AC_CHECK_HEADERS([windows.h])

# look up canonical build name and write it to config.h
AC_CANONICAL_BUILD
AC_DEFINE_UNQUOTED([BUILDSYSTEM], ["$build"], [The platform.])

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


#############################################################################
# create output files
#############################################################################

# Set files to generate.
AC_CONFIG_FILES([Makefile doc/Makefile src/Makefile doc/Doxyfile man/Makefile tests/Makefile tests/package.m4 gui/static_release.sh maintainer/Makefile maintainer/debian/changelog.template maintainer/debian/info maintainer/debian/README.Debian.template maintainer/debian/rules maintainer/macports/Portfile maintainer/gnu/Makefile])
AC_CONFIG_FILES([tests/cover.sh], [chmod +x tests/cover.sh])
AC_OUTPUT

# fix permissions of generated shell scripts
chmod a+x gui/static_release.sh
