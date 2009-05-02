AC_INIT(Wendy, 1.0-unreleased, niels@service-technology.org)
AC_CONFIG_SRCDIR([src/main.cc])
AC_CONFIG_HEADERS([src/config.h])

AM_INIT_AUTOMAKE(gnits)

# check for programs needed to compile
AM_PROG_CC_C_O
AC_PROG_CXX
AC_PROG_LEX
AC_PROG_YACC
AC_PROG_RANLIB
AM_MISSING_PROG(GENGETOPT, gengetopt)
AM_MISSING_PROG(HELP2MAN, help2man)
AC_PROG_SED

# search for LoLA
AC_PATH_TOOL(LOLA, [lola-full], not found)
if test "$LOLA" = "not found"; then
  AC_MSG_WARN([LoLA was not found. You need to execute Wendy with the '--lola' parameter.])
else
  AC_DEFINE_UNQUOTED(BINARY_LOLA, "${LOLA}", [LoLA binary.])
fi

# search for Fiona (needed for "make check")
AC_CHECK_PROG(FIONA, fiona, fiona, not found)

# needed for the Petri Net API
AC_DEFINE_UNQUOTED(CONFIG_PETRIFY, "not found", [Petrify is not used.])

AC_HEADER_ASSERT

# check for functions
AC_CHECK_FUNCS([popen pclose],,AC_MSG_WARN([cannot use popen or pclose]))

# check for exact unsigned types
AC_TYPE_UINT8_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T

AC_CONFIG_FILES([Makefile libs/Makefile src/Makefile doc/Makefile doc/Doxyfile man/Makefile tests/Makefile])
AC_OUTPUT
