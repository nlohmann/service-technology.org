AC_INIT([Petri Net API], 3.01-unreleased, pnapi@service-technology.org, pnapi, http://service-technology.org/pnapi)

# the first line should be everything you will ever need to touch
#############################################################################

AC_MSG_NOTICE([ ____      _        _            _        _    ____ ___ ])
AC_MSG_NOTICE([|  _ \ ___| |_ _ __(_)_ __   ___| |_     / \  |  _ \_ _|])
AC_MSG_NOTICE([| |_) / _ \ __| '__| | '_ \ / _ \ __|   / _ \ | |_) | | ])
AC_MSG_NOTICE([|  __/  __/ |_| |  | | | | |  __/ |_   / ___ \|  __/| | ])
AC_MSG_NOTICE([|_|   \___|\__|_|  |_|_| |_|\___|\__| /_/   \_\_|  |___|])
AC_MSG_NOTICE([encapsulating Petri net-related functions in a C++ API])
AC_MSG_NOTICE([http://service-technology.org/pnapi])
AC_MSG_NOTICE([version ${PACKAGE_VERSION}])
AC_MSG_NOTICE()

AM_INIT_AUTOMAKE(gnits)
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

# where to look for source code and how the config header should be called
AC_CONFIG_SRCDIR(src/petrinet.cc)
AC_CONFIG_HEADERS(src/config.h)

# check for generic programs
AC_PROG_CXX
AC_PROG_CPP
AC_PROG_SED
AC_PROG_GREP
AM_PROG_CC_C_O
AC_PROG_LEX
AC_PROG_YACC

# parameters for Libtool
AC_PROG_LIBTOOL
AC_LIBTOOL_WIN32_DLL
AC_CONFIG_MACRO_DIR([m4])

# check for functions
AC_CHECK_FUNCS([popen pclose])

# check for additional programs
AM_MISSING_PROG(GENGETOPT, gengetopt)
AM_MISSING_PROG(HELP2MAN, help2man)

# export tools needed to determine memory consumption
AC_DEFINE_UNQUOTED(TOOL_AWK, "${AWK}", [awk])
AC_DEFINE_UNQUOTED(TOOL_GREP, "${GREP}", [grep])

# check for tools to be used in test cases
AC_PATH_PROGS(WGET, [wget], not found)
AC_PATH_PROGS(MAKE, [gmake make], not found)
AC_PATH_PROGS(FIONA, [fiona], not found)
AC_PATH_PROGS(HEAD, [head], not found)
AC_PATH_PROGS(TAIL, [tail], not found)

# set up autotest
AM_MISSING_PROG(AUTOM4TE, autom4te)
AC_CONFIG_TESTDIR(tests)


#############################################################################
# search for Dot, Petrify, and Genet and write results to file "src/config.h"
#############################################################################
AC_PATH_PROG(DOT, [dot], not found)
if test "$DOT" = "not found"; then
  AC_MSG_WARN([Graphviz Dot was not found. See file README.])
  AC_DEFINE_UNQUOTED(CONFIG_DOT, "not found", [Graphviz Dot was not found.])
else
  AC_DEFINE_UNQUOTED(CONFIG_DOT, "\"${DOT}\"", [Graphviz Dot can be used.])
fi
AC_SUBST(DOT)

AC_PATH_PROGS(PETRIFY, [petrify4.2 petrify4.1 petrify4.0 petrify], not found)
if test "$PETRIFY" = "not found"; then
  AC_MSG_WARN([Petrify was not found! See file REQUIREMENTS.])
  AC_DEFINE(CONFIG_PETRIFY, "not found", [Petrify was not found.])
else
  AC_DEFINE_UNQUOTED(CONFIG_PETRIFY, "\"${PETRIFY}\"", [Petrify can be used.])
fi
AC_SUBST(PETRIFY)

AC_PATH_PROGS(GENET, [genet], not found)
if test "$GENET" = "not found"; then
  AC_MSG_WARN([Genet was not found! See file REQUIREMENTS.])
  AC_DEFINE(CONFIG_GENET, "not found", [Genet was not found.])
else
  AC_DEFINE_UNQUOTED(CONFIG_GENET, "\"${GENET}\"", [Genet can be used.])
fi
AC_SUBST(GENET)

# search for required tools
AC_PATH_PROG(LOLA, [lola-statespace], [])
if test "${LOLA}" = ""; then
  AC_MSG_WARN([LoLA was not found! See file REQUIREMENTS.])
else
AC_PATH_PROGS_FEATURE_CHECK([LOLAVERSION], [lola-statespace], [[ac_cv_path_LOLAVERSION=`$ac_path_LOLAVERSION --version | ${SED} -e 's/LoLA //'`]])
AC_MSG_CHECKING([LoLA version])
AC_MSG_RESULT([${ac_cv_path_LOLAVERSION}])
echo ${ac_cv_path_LOLAVERSION} | ${AWK} '{if ($1>=1.14) exit 0; else exit 1}' || AC_MSG_WARN([CDNF needs at least LoLA 1.14! See file REQUIREMENTS.])
fi

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
# create configured files (Makefiles, scripts, etc.)
#############################################################################
AC_CONFIG_FILES([Makefile src/libpnapi.pc
                 doc/Makefile doc/Doxyfile
                 src/Makefile src/pnapi.cc
                 man/Makefile
                 tests/Makefile tests/package.m4
                 utils/Makefile utils/unfolder/Makefile])
AC_CONFIG_FILES([tests/check_customer.sh], [chmod +x tests/check_customer.sh])
AC_CONFIG_FILES([tests/cover.sh], [chmod +x tests/cover.sh])
AC_CONFIG_FILES([utils/make_customer.sh], [chmod +x utils/make_customer.sh])
AC_OUTPUT

