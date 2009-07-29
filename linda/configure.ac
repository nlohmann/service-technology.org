AC_INIT(Linda, 0.5, linda@service-technology.org)
AC_CONFIG_SRCDIR([src/main.cc])
AC_CONFIG_HEADERS([src/config.h])

AM_INIT_AUTOMAKE(gnits)

# Checks for programs.
AM_PROG_CC_C_O
AC_PROG_CXX
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

# needed for the Petri Net API
AC_DEFINE_UNQUOTED(CONFIG_PETRIFY, "not found", [Petrify is not used.])

#AC_HEADER_ASSERT

# check for dlopen/ftime (required by lp_solve)
AC_SEARCH_LIBS([dlopen], [dl zrldsrl])
AC_SEARCH_LIBS([ftime], [compat])

# check if the Petri Net API is already installed
AC_CHECK_LIB([pnapi], [main])
AM_CONDITIONAL(COMPILE_PNAPI, [test "${ac_cv_lib_pnapi_main}" = "no"])

# check for functions
AC_CHECK_FUNCS([popen pclose],,AC_MSG_WARN([cannot use popen or pclose]))

# check for exact unsigned types
#AC_TYPE_UINT8_T
#AC_TYPE_UINT16_T
#AC_TYPE_UINT32_T

AC_CONFIG_FILES([Makefile 
				libs/pnapi/Makefile 
				libs/lp_solve/Makefile 
				src/Makefile 
				doc/Makefile 
				man/Makefile
				maintainer/Makefile 
                maintainer/macports/Portfile
				tests/Makefile tests/package.m4])
AC_OUTPUT
