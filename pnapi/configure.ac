AC_INIT([Petri Net API], 4.02, pnapi@service-technology.org, pnapi, http://service-technology.org/pnapi)

AC_MSG_NOTICE([ ____      _        _            _        _    ____ ___ ])
AC_MSG_NOTICE([|  _ \ ___| |_ _ __(_)_ __   ___| |_     / \  |  _ \_ _|])
AC_MSG_NOTICE([| |_) / _ \ __| '__| | '_ \ / _ \ __|   / _ \ | |_) | | ])
AC_MSG_NOTICE([|  __/  __/ |_| |  | | | | |  __/ |_   / ___ \|  __/| | ])
AC_MSG_NOTICE([|_|   \___|\__|_|  |_|_| |_|\___|\__| /_/   \_\_|  |___|])
AC_MSG_NOTICE([encapsulating Petri net-related functions in a C++ API])
AC_MSG_NOTICE([http://service-technology.org/pnapi])
AC_MSG_NOTICE([version ${PACKAGE_VERSION}])
AC_MSG_NOTICE()

AC_CONFIG_SRCDIR(src/petrinet.cc)
AC_CONFIG_HEADERS(src/config.h)
AC_CONFIG_AUX_DIR([build-aux])

AM_INIT_AUTOMAKE(gnits)
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

# check for standard programs needed to compile
AC_PROG_CXX
AM_PROG_CC_C_O
AC_PROG_LEX
AC_PROG_YACC

# using YACC doesn't work, need to call bison this way
AC_PATH_PROG(BISON, [bison], not found)

# use the service-technology.org macros
AC_ST_BASIC
AC_ST_LOLA([1.14])
AC_ST_AUTOTEST
AC_ST_VERBOSE

# parameters for Libtool
AC_PROG_LIBTOOL
AC_LIBTOOL_WIN32_DLL
AC_CONFIG_MACRO_DIR([m4])

# check for tools to be used in test cases
AC_PATH_PROGS(WGET, [wget], not found)
AC_PATH_PROGS(MAKE, [gmake make], not found)
AC_PATH_PROGS(FIONA, [fiona], not found)
AC_PATH_PROGS(HEAD, [head], not found)
AC_PATH_PROGS(TAIL, [tail], not found)
AC_PATH_PROGS(VALGRIND, [valgrind], not found)

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

# check for required functions and die if they are not found
AC_CHECK_FUNCS([popen pclose],,AC_MSG_ERROR([cannot use required functions]))

#############################################################################
# create configured files (Makefiles, scripts, etc.)
#############################################################################
AC_CONFIG_FILES([Makefile src/libpnapi.pc
                 doc/Makefile doc/Doxyfile
                 src/Makefile src/pnapi.cc
                 man/Makefile
                 tests/Makefile tests/package.m4
                 utils/Makefile utils/unfolder/Makefile utils/petri.conf])
AC_CONFIG_FILES([src/bison-wrapper.sh], [chmod +x src/bison-wrapper.sh])
AC_CONFIG_FILES([tests/check_customer.sh], [chmod +x tests/check_customer.sh])
AC_CONFIG_FILES([tests/cover.sh], [chmod +x tests/cover.sh])
AC_CONFIG_FILES([utils/make_customer.sh], [chmod +x utils/make_customer.sh])
AC_OUTPUT
