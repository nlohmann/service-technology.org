AC_INIT(Sara, 0.9, sara@service-technology.org, sara, http://service-technology.org/sara)

AC_CONFIG_SRCDIR([src/main.cc])
AC_CONFIG_HEADERS([src/config.h])
#AC_CONFIG_AUX_DIR([build-aux])

AM_INIT_AUTOMAKE#(gnits)
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

# check for standard programs needed to compile
AC_PROG_CXX
AM_PROG_CC_C_O
AC_PROG_LEX
AC_PROG_YACC

# use the service-technology.org macros
AC_ST_BASIC
AC_ST_AUTOTEST
AC_ST_PNAPI
AC_ST_VERBOSE

# search for optional tools
AC_PROG_LIBTOOL

# check for dlopen/ftime (required by lp_solve)
AC_SEARCH_LIBS([dlopen], [dl zrldsrl])
AC_SEARCH_LIBS([ftime], [compat])

# check for functions
AC_CHECK_FUNCS([popen pclose],,AC_MSG_WARN([cannot use pipes -- expect bad performance]))

# check for exact unsigned types
AC_TYPE_UINT8_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T

# write files
AC_CONFIG_FILES([
	Makefile libs/pnapi/Makefile libs/lp_solve/Makefile src/Makefile tests/Makefile tests/package.m4
	])
AC_CONFIG_FILES([tests/cover.sh], [chmod +x tests/cover.sh])
AC_OUTPUT
