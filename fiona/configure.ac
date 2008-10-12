AC_PREREQ(2.59)
AC_INIT(Fiona, 3.0-unreleased, fiona@service-technology.org, fiona)

AC_CONFIG_SRCDIR(src/main.cc)
AC_CONFIG_HEADERS(src/fiona.h)

AM_INIT_AUTOMAKE

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_CPP
AC_PROG_EGREP
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_SED


#############################################################################
# check whether to use assertions using the configure script parameter
# "--enable-assert" (standard) and "--disable-assert"
#############################################################################
AC_ARG_ENABLE(assert,
  AS_HELP_STRING([--disable-assert],[disable assertions and verbose debug messages (enabled by default)]),
  [assertions=${enableval}], [assertions=yes])
AC_MSG_CHECKING([whether to use assertions])
AC_MSG_RESULT($assertions)
if test "$assertions" = "no"; then
  AC_DEFINE(NDEBUG,1,[Turn off assertions and verbose debug messages.])
fi


#############################################################################
# check whether to build for a 64 bit architecture using the configure
# script parameter "--enable-64bit" (standard: off)
#############################################################################
AC_ARG_ENABLE(64bit,
  AS_HELP_STRING([--enable-64bit],[build for a 64 bit architecture such as x86_64 (disabled by default)]),
  enable_64bit=${enableval}, enable_64bit="no")
AC_MSG_CHECKING([whether to build for a 64 bit architecture])
AC_MSG_RESULT($enable_64bit)
AM_CONDITIONAL(ENABLE_64BIT, test "$enable_64bit" = "yes")


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
# search for Dot and Petrify and write results to file "src/fiona.h"
#############################################################################

AC_CHECK_PROG(DOT, dot, dot, not found)
if test "$DOT" = "not found"; then
  AC_MSG_WARN([Graphviz dot was not found.])
  AC_DEFINE(HAVE_DOT, 0, [Dot can not be used on this system.])
else
  AC_DEFINE(HAVE_DOT, 1, [Dot can be used on this system.])
fi

AC_CHECK_PROGS(PETRIFY, [petrify4.2 petrify4.1 petrify4.0 petrify], not found)
if test "$PETRIFY" = "not found"; then
  AC_MSG_WARN([Petrify was not found.])
  AC_DEFINE(HAVE_PETRIFY, "not found", [Petrify was not found on this system.])
else
  AC_DEFINE_UNQUOTED(HAVE_PETRIFY, "${PETRIFY}", [Petrify can be used on this system.])
fi


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
# set flags for CUDD
#############################################################################
case `uname -s` in
  Linux)
    CUDD_XFLAGS="-malign-double -DHAVE_IEEE_754 -DBSD"
    ;;
  SunOS)
    CUDD_XFLAGS="-DHAVE_IEEE_754 -DUNIX100 -DEPD_BIG_ENDIAN"
    ;;
  CYGWIN*)
    CUDD_XFLAGS="-malign-double -DHAVE_IEEE_754 -DHAVE_GETRLIMIT=0 -DRLIMIT_DATA_DEFAULT=67108864"
    ;;
  Darwin)
    CUDD_XFLAGS="-DBSD -DHAVE_IEEE_754 -DEPD_BIG_ENDIAN -O3"
    ;;
  *) ;;
esac

AC_SUBST(CUDD_XFLAGS)


#############################################################################
# anything else
#############################################################################

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lm':
AC_CHECK_LIB([m], [main])

# no optimization flag here as it takes too much time
# [NL] I think we should not force any flags, so I commented this
#CXXFLAGS=-g

# Force @builddir@ substitution. Without this src/Makefile.in lacked @builddir@
# substitution on gruenau.informatik.hu-berlin.de with automake 1.9.6.
AC_SUBST(builddir)


#############################################################################
# create configured files (Makefiles, scripts, etc.)
#############################################################################

AC_CONFIG_FILES([Makefile
                 Doxyfile
                 src/Makefile
                 tests/Makefile
                 doc/Makefile
                 cudd/Makefile
                 cudd/setup.sh
                 cudd/shutdown.sh
                 cudd/cudd/Makefile
                 cudd/dddmp/Makefile
                 cudd/epd/Makefile
                 cudd/mnemosyne/Makefile
                 cudd/mtr/Makefile
                 cudd/nanotrav/Makefile
                 cudd/obj/Makefile
                 cudd/st/Makefile
                 cudd/util/Makefile])
AC_OUTPUT


#############################################################################
# Print summary of compile settings
#############################################################################
echo
echo "--------- compile settings ---------"
echo "compile for 64bit...: $enable_64bit"
echo "enable assertions...: $assertions"
echo "------------------------------------"
echo
echo
echo "Run \"make\" to build fiona (cudd package is built automatically)."
echo "Run \"make win32\" to build a fiona executable that is independent of cygwin1.dll."
echo
