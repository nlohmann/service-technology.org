#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

# Process this file with autoconf to produce a configure script.
AC_PREREQ(2.59)
AC_INIT(Fiona, 1.5, weinberg@informatik.hu-berlin.de, fiona)
AC_REVISION($Revision: 1.15 $)

AC_CONFIG_SRCDIR(src/main.cc)
AC_CONFIG_HEADERS(src/fiona.h)

AM_INIT_AUTOMAKE

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
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

# Checks for header files.
AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS([float.h libintl.h limits.h stddef.h stdlib.h string.h strings.h sys/file.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_TYPE_OFF_T
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_FUNC_MEMCMP
AC_FUNC_REALLOC
AC_FUNC_STRCOLL
AC_FUNC_WAIT3
AC_CHECK_FUNCS([bzero dup2 floor gethostname getpagesize memchr memmove memset pow sqrt strchr strcspn strerror strpbrk strrchr strspn strstr])

AC_PROG_LEX
if test "$LEX" != "flex"; then
 AC_MSG_ERROR([flex was not found.])
fi

AC_CHECK_PROG(YACC, bison, bison, not found)
if test "$YACC" = "not found"; then
 AC_MSG_ERROR([bison was not found.])
fi

# Check for --enable-64bit parameter to enable 64bit compilation
AC_ARG_ENABLE(64bit,
  [  --enable-64bit  build for 64bit architecture [default=no]],
  enable_64bit="yes", enable_64bit="no")
AM_CONDITIONAL(ENABLE_64BIT, test "$enable_64bit" = "yes")

# no optimization flag here as it takes too much time
CXXFLAGS=-g

AC_CONFIG_FILES([Makefile
                 src/Makefile
                 tests/Makefile
		 doc/Makefile])
AC_OUTPUT

# Print summary of compile settings

echo "--------- compile settings ---------"
echo "compile for 64bit...: $enable_64bit"
echo "------------------------------------"
echo

AC_MSG_NOTICE([Creating Makefile include in order to build cudd package])
# create Makefile include for cudd package
echo "# created by configure script"  > $srcdir/cudd/Makefile.inc
echo                      >> $srcdir/cudd/Makefile.inc
echo "CPP = $CXX"             >> $srcdir/cudd/Makefile.inc
echo              >> $srcdir/cudd/Makefile.inc
echo "# IMPORTANT NOTICE: C sources also need to be compiled as C++, because the code" >> $srcdir/cudd/Makefile.inc
echo "# for logging malloc, realloc, free, new and delete (newlogger.cc) is" >> $srcdir/cudd/Makefile.inc
echo "# written in C++, and C cannot call C++. Only C++ can do that." >> $srcdir/cudd/Makefile.inc
echo "CC  = $CXX"             >> $srcdir/cudd/Makefile.inc
echo                                  >> $srcdir/cudd/Makefile.inc
echo "RANLIB = $RANLIB"           >> $srcdir/cudd/Makefile.inc
echo                                  >> $srcdir/cudd/Makefile.inc
case `uname -s` in
  Linux)    echo "XCFLAGS = -march=`uname -m` -malign-double -DHAVE_IEEE_754 -DBSD" >> $srcdir/cudd/Makefile.inc ;;
  SunOS)    echo "XCFLAGS = -mcpu=ultrasparc  -DHAVE_IEEE_754 -DUNIX100 -DEPD_BIG_ENDIAN" >> $srcdir/cudd/Makefile.inc;;
  CYGWIN*)  echo "XCFLAGS = -march=`uname -m` -malign-double -DHAVE_IEEE_754 -DHAVE_GETRLIMIT=0 -DRLIMIT_DATA_DEFAULT=67108864" >> $srcdir/cudd/Makefile.inc;;
#  Darwin)   echo "XCFLAGS = -m`uname -p` -DBSD -DHAVE_IEEE_754 -DEPD_BIG_ENDIAN -O3 -qstrict" >> $srcdir/cudd/Makefile.inc ;;
  *) ;;
esac
echo                                  >> $srcdir/cudd/Makefile.inc

echo
echo "Run \"make\" to build fiona (cudd package is built automatically)."
echo "Run \"make win32\" to build a fiona executable that is independent of cygwin1.dll."
echo
