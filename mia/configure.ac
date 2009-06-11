AC_INIT(Mia, 0.1, niels.lohmann@service-technology.org)
AC_CONFIG_SRCDIR([src/main.cc])
AC_CONFIG_HEADERS([src/config.h])

AM_INIT_AUTOMAKE #(gnits)

# Checks for programs.
AM_PROG_CC_C_O
AC_PROG_CXX
AC_PROG_LEX
AC_PROG_YACC
AC_PROG_RANLIB

AM_MISSING_PROG(GENGETOPT, gengetopt)
AM_MISSING_PROG(HELP2MAN, help2man)

# needed for the Petri Net API
AC_DEFINE_UNQUOTED(CONFIG_PETRIFY, "not found", [Petrify is not used.])

AC_HEADER_ASSERT

# check for Wendy
AC_CHECK_PROG(WENDY, wendy, wendy, not found)
if test "$WENDY" = "not found"; then
  AC_MSG_ERROR([Wendy was not found, but is needed by Mia.])
fi

# check for LoLA
AC_CHECK_PROG(LOLA, lola-full, lola-full, not found)
if test "$LOLA" = "not found"; then
  AC_MSG_ERROR([LoLA was not found, but is needed by Mia.])
fi

# check if the Petri Net API is already installed
AM_MISSING_PROG(PKG_CONFIG, pkg-config)
AC_CHECK_LIB(pnapi, main)
AM_CONDITIONAL(COMPILE_PNAPI, [test "${ac_cv_lib_pnapi_main}" = "no"])

# check for functions
AC_CHECK_FUNCS([popen pclose],,AC_MSG_WARN([cannot use popen or pclose]))

AC_CONFIG_FILES([Makefile libs/pnapi/Makefile src/Makefile doc/Doxyfile man/Makefile])
AC_OUTPUT
