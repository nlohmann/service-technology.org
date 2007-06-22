#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

# Process this file with autoconf to produce a configure script.
AC_PREREQ(2.59)
AC_INIT(Fiona, 2.0, weinberg@informatik.hu-berlin.de, fiona)
AC_REVISION($Revision: 1.24 $)

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
AC_PROG_RANLIB

if test "$RANLIB" = ":"; then
  AC_MSG_ERROR([ranlib was not found!])
fi
AC_CHECK_PROG(AR, ar, ar, not found)
if test "$AR" = "not found"; then
 AC_MSG_ERROR([ar was not found!])
fi

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lm':
AC_CHECK_LIB([m], [main])

AC_PROG_LEX
if test "$LEX" != "flex"; then
 AC_MSG_ERROR([flex was not found.])
fi

AC_CHECK_PROG(YACC, bison, bison, not found)
if test "$YACC" = "not found"; then
 AC_MSG_ERROR([bison was not found.])
fi

#if test "`uname -o`" != "Cygwin" ; then
#ac_default_prefix=$HOME
#fi

# Check for --enable-64bit parameter to enable 64bit compilation
AC_ARG_ENABLE(64bit,
  [  --enable-64bit  build for 64bit architecture [default=no]],
  enable_64bit="yes", enable_64bit="no")
AM_CONDITIONAL(ENABLE_64BIT, test "$enable_64bit" = "yes")

# no optimization flag here as it takes too much time
CXXFLAGS=-g


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

# Force @builddir@ substitution. Without this src/Makefile.in lacked @builddir@
# substitution on gruenau.informatik.hu-berlin.de with automake 1.9.6.
AC_SUBST(builddir)

AC_CONFIG_FILES([Makefile
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

# Print summary of compile settings

echo "--------- compile settings ---------"
echo "compile for 64bit...: $enable_64bit"
echo "------------------------------------"
echo

echo
echo "Run \"make\" to build fiona (cudd package is built automatically)."
echo "Run \"make win32\" to build a fiona executable that is independent of cygwin1.dll."
echo
