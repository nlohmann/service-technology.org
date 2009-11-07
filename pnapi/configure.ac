AC_INIT(Petri Net API, 3.01-unreleased, pnapi@service-technology.org, pnapi)

# the first line should be everything you will ever need to touch
#############################################################################

AC_PREREQ(2.59)
AM_INIT_AUTOMAKE #(gnits)
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

# where to look for source code and how the config header should be called
AC_CONFIG_SRCDIR(src/petrinet.cc)
AC_CONFIG_HEADERS(src/config.h)

# check for generic programs
AC_PROG_CXX
AC_PROG_CPP
AC_PROG_GREP
AM_PROG_CC_C_O

# check if we can handle dynamic libs
AC_DISABLE_SHARED
AC_PROG_LIBTOOL
AC_CONFIG_MACRO_DIR([m4])

# check for functions
AC_CHECK_FUNCS([popen pclose])

# check for additional programs
AM_MISSING_PROG(GENGETOPT, gengetopt)
AM_MISSING_PROG(HELP2MAN, help2man)

# set up autotest
AM_MISSING_PROG(AUTOM4TE, autom4te)
AC_CONFIG_TESTDIR(tests)

#############################################################################
# search for Flex and Bison
#############################################################################
AC_PROG_LEX
if test "$LEX" != "flex"; then
  AC_MSG_WARN([Flex was not found. You will not be able to compile from scratch. See file README.])
  AM_MISSING_PROG(LEX, flex)
fi

AC_PROG_YACC
if test "$YACC" = "yacc"; then
  AC_MSG_WARN([Bison was not found. You will not be able to compile from scratch. See file README.])
  AM_MISSING_PROG(YACC, bison)
fi


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

AC_PATH_PROGS(PETRIFY, [petrify4.2 petrify4.1 petrify4.0 petrify], not found)
if test "$PETRIFY" = "not found"; then
  AC_MSG_WARN([Petrify was not found. See file README.])
  AC_DEFINE(CONFIG_PETRIFY, "not found", [Petrify was not found.])
else
  AC_DEFINE_UNQUOTED(CONFIG_PETRIFY, "${PETRIFY}", [Petrify can be used.])
fi
AC_SUBST(PETRIFY)

AC_PATH_PROGS(GENET, [genet], not found)
if test "$GENET" = "not found"; then
  AC_MSG_WARN([Genet was not found. See file README.])
  AC_DEFINE(CONFIG_GENET, "not found", [Genet was not found.])
else
  AC_DEFINE_UNQUOTED(CONFIG_GENET, "${GENET}", [Genet can be used.])
fi
AC_SUBST(GENET)

# check for Fiona (would be used in test cases)
AC_PATH_PROG(FIONA, [fiona], not found)
AC_SUBST(FIONA)


# organize assertions -- switch off by "--disable-assert"
AC_HEADER_ASSERT


#############################################################################
# create configured files (Makefiles, scripts, etc.)
#############################################################################
AC_CONFIG_FILES([Makefile
                 pnapi.pc
                 doc/Makefile
                 doc/Doxyfile
                 src/Makefile
                 man/Makefile
                 tests/Makefile
                 tests/package.m4
                 utils/Makefile utils/unfolder/Makefile])
AC_CONFIG_FILES([tests/cover.sh], [chmod +x tests/cover.sh])
AC_OUTPUT
