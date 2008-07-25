# Process this file with autoconf to produce a configure script.
AC_PREREQ(2.60)
AC_INIT([LoLA],[1.02],[niels.lohmann@uni-rostock.de],[lola])
AM_INIT_AUTOMAKE

# Set directories.
AC_CONFIG_SRCDIR(src/net.cc)

# Set the configuration header.
AC_CONFIG_HEADER(src/config.H)
AC_CANONICAL_BUILD
AC_DEFINE_UNQUOTED([BUILDSYSTEM], ["$build"], [The platform.])

# Checks for needed programs.
AC_PROG_CXX
AC_PROG_LEX
AC_PROG_YACC

# Checks for header files.
AC_FUNC_ALLOCA
AC_HEADER_STDC
AC_CHECK_HEADERS([inttypes.h libintl.h limits.h malloc.h stddef.h stdlib.h string.h strings.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT8_T
AC_TYPE_SIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([memset])

# Set files to generate.
AC_CONFIG_FILES([Makefile src/Makefile tests/Makefile])

AC_OUTPUT

echo ""
echo "WARNING! This is not an official release version of LoLA."
echo "         For an official release, visit <http://service-technology.org/lola>."
echo ""
