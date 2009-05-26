AC_INIT(Wendy, 1.4-unreleased, wendy@service-technology.org)
AC_CONFIG_SRCDIR([src/main.cc])
AC_CONFIG_HEADERS([src/config.h])

AM_INIT_AUTOMAKE(gnits)

# check for programs needed to compile
AC_PROG_CXX
AM_PROG_CC_C_O
AC_PROG_LEX
AC_PROG_YACC
AC_PROG_RANLIB
AC_PROG_SED
AC_PROG_GREP
AM_MISSING_PROG(GENGETOPT, gengetopt)
AM_MISSING_PROG(HELP2MAN, help2man)

# set up autotest
AM_MISSING_PROG(AUTOM4TE, autom4te)
AC_CONFIG_TESTDIR(tests)

# look up canonical build name and write it to config.h
AC_CANONICAL_BUILD
AC_DEFINE_UNQUOTED([CONFIG_BUILDSYSTEM], ["$build"], [The platform.])

# search for LoLA
AC_PATH_TOOL(LOLA, [lola-full], not found)
if test "$LOLA" = "not found"; then
  AC_MSG_WARN([LoLA was not found in the path.])
  AC_MSG_WARN([You need to execute Wendy with the '--lola' parameter.])
else
  AC_DEFINE_UNQUOTED(BINARY_LOLA, "${LOLA}", [LoLA binary.])
fi

# search for Fiona (nice to have for "make check")
AC_CHECK_PROG(FIONA, fiona, fiona, not found)

# needed for the Petri Net API
AC_DEFINE_UNQUOTED(CONFIG_PETRIFY, "not found", [Petrify is not used.])

AC_HEADER_ASSERT

# experimental (might be used for binary file formats)
AC_C_BIGENDIAN

# check for functions
AC_CHECK_FUNCS([popen pclose],,AC_MSG_WARN([Wendy cannot use UNIX pipes.]))

# check if the Petri Net API is already installed
AM_MISSING_PROG(PKG_CONFIG, pkg-config)
AC_CHECK_LIB(pnapi, main)
AM_CONDITIONAL(COMPILE_PNAPI, [test "${ac_cv_lib_pnapi_main}" = "no"])

# check for exact unsigned types
AC_TYPE_UINT8_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T

AC_CONFIG_FILES([Makefile libs/Makefile src/Makefile doc/Makefile doc/Doxyfile man/Makefile maintainer/Makefile tests/Makefile tests/package.m4])
AC_OUTPUT
