AC_PREREQ(2.60)
AC_INIT([oWFN2BPEL],[4.9-unreleased],[owfn2bpel@service-technology.org],[owfn2bpel])

AC_MSG_NOTICE([    __        _______ _   _ ____  ____  ____  _____ _     ])
AC_MSG_NOTICE([  __\ \      / /  ___| \ | |___ \| __ )|  _ \| ____| |    ])
AC_MSG_NOTICE([ / _ \ \ /\ / /| |_  |  \| | __) |  _ \| |_) |  _| | |    ])
AC_MSG_NOTICE([| (_) \ V  V / |  _| | |\  |/ __/| |_) |  __/| |___| |___ ])
AC_MSG_NOTICE([ \___/ \_/\_/  |_|   |_| \_|_____|____/|_|   |_____|_____|])
AC_MSG_NOTICE([Translates open nets to BPEL processes])
AC_MSG_NOTICE([${PACKAGE_URL}])
AC_MSG_NOTICE([version ${PACKAGE_VERSION} ])
AC_MSG_NOTICE()

AM_INIT_AUTOMAKE #(gnits)
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

# Set directories.
AC_CONFIG_SRCDIR(src/main.cc)
AC_CONFIG_HEADERS(src/config.h)

# Checks for C++ details.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_YACC
AC_PROG_LEX
AC_PROG_SED

# allow the configure script to control assertions (just include config.h)
AC_HEADER_ASSERT
AH_BOTTOM([#ifdef __cplusplus
#include <cassert>
#else
#include <assert.h>
#endif])

# Announce the build system
AC_CANONICAL_BUILD
AC_DEFINE_UNQUOTED([BUILDSYSTEM], ["$build"], [The platform.])
AC_DEFINE_UNQUOTED([BUILDOS], ["$build_os"], [The OS.])

# The name of the LoLA source distribution. The Makefiles will look for a file
# called "src/LOLANAME.tar.gz" to compile LoLA.
AC_SUBST(LOLANAME, lola-1.15-unreleased)

# Checks for GNU gengetopt and help2man
AM_MISSING_PROG(GENGETOPT, gengetopt)
AM_MISSING_PROG(HELP2MAN, help2man)

# Set files to generate.
AC_CONFIG_FILES([Makefile src/Makefile src/Doxyfile man/Makefile doc/Makefile doc/Doxyfile tests/Makefile])

AC_OUTPUT
