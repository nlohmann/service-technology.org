AC_INIT(Sara, 0.9, sara@service-technology.org, sara, http://service-technology.org/sara)
AC_CONFIG_SRCDIR([src/main.cc])
AC_CONFIG_HEADERS([src/config.h])
AC_CONFIG_AUX_DIR([build-aux])

AM_INIT_AUTOMAKE #(gnits)
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

# check for standard programs needed to compile
AC_PROG_CXX
AM_PROG_CC_C_O
AC_PROG_LEX
AC_PROG_YACC
AC_PROG_RANLIB
AC_PROG_SED
AC_PROG_GREP
AC_PROG_AWK

# check for additional programs needed to compile
AM_MISSING_PROG(GENGETOPT, gengetopt)
AM_MISSING_PROG(HELP2MAN, help2man)

# export tools needed to determine memory consumption
AC_DEFINE_UNQUOTED(TOOL_AWK, "${AWK}", [awk])
AC_DEFINE_UNQUOTED(TOOL_GREP, "${GREP}", [grep])

# set up autotest
#AM_MISSING_PROG(AUTOM4TE, autom4te)
#AC_CONFIG_TESTDIR(tests)

# store the SVN revision number
#AC_SUBST(VERSION_SVN, "`svnversion -n 2> /dev/null`")
#AC_DEFINE_UNQUOTED(VERSION_SVN, "${VERSION_SVN}", [The SVN revision.])

# allow the configure script to control assertions
AC_HEADER_ASSERT

# look up canonical build name and write it to config.h
AC_CANONICAL_BUILD
AC_DEFINE_UNQUOTED([CONFIG_BUILDSYSTEM], ["$build"], [The platform.])

# search for required tools
#AC_PATH_PROG(LOLA, [lola-statespace], [])

AC_PROG_LIBTOOL

# check for dlopen/ftime (required by lp_solve)
AC_SEARCH_LIBS([dlopen], [dl zrldsrl])
AC_SEARCH_LIBS([ftime], [compat])

# search for optional tools needed by "make check"
#AC_PATH_PROG(FIONA, [fiona], not found)
#AC_PATH_PROG(SA2SM, [sa2sm], not found)
#AC_PATH_PROG(MIA, [mia], not found)
#AC_PATH_PROG(MARLENE, [marlene], not found)

# only use installed Petri Net API if this was configured with --enable-pnapi
AC_ARG_ENABLE(pnapi,
  AS_HELP_STRING([--enable-pnapi],[use installed Petri Net API (disabled by default)]),
  enable_pnapi=${enableval}, enable_pnapi="no")
AC_MSG_CHECKING([whether to use installed Petri Net API])
AC_MSG_RESULT($enable_pnapi)
if test "$enable_pnapi" = "yes"; then
  # check if the Petri Net API is already installed
  AM_MISSING_PROG(PKG_CONFIG, pkg-config)
  AC_CHECK_LIB(pnapi, main)
  if test "${ac_cv_lib_pnapi_main}" = "no"; then
    AC_MSG_WARN([Petri Net API was not found!])
    AC_MSG_WARN([Using shipped version from libs/pnapi.])
  fi
  AM_CONDITIONAL(COMPILE_PNAPI, [test "${ac_cv_lib_pnapi_main}" = "no"])
else
  AM_CONDITIONAL(COMPILE_PNAPI, [true])
fi

# check for functions
#AC_CHECK_FUNCS([popen pclose],,AC_MSG_WARN([cannot use pipes -- expect bad performance]))
#AC_CHECK_FUNCS([mktemp vfprintf],,AC_MSG_ERROR([cannot use required functions]))

# check for exact unsigned types
AC_TYPE_UINT8_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T

# write files
AC_CONFIG_FILES([
	Makefile libs/pnapi/Makefile libs/lp_solve/Makefile src/Makefile 
	])
AC_OUTPUT


