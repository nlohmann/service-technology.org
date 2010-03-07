AC_INIT([OGAPI], 1.00-unreleased, ogapi@service-technology.org, ogapi, http://service-technology.org/ogapi)

AC_CONFIG_SRCDIR(src/productOG/main.cc)
AC_CONFIG_HEADERS(src/config.h)

AC_PREREQ(2.59)
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
AC_ST_VERBOSE

# search for optional tools needed by "make check"
AC_PATH_PROG(FIONA, [fiona], not found)
AC_PATH_PROG(WENDY2FIONA, [wendy2fiona], not found)

# check for required functions and die if they are not found
AC_CHECK_FUNCS([popen pclose],,AC_MSG_ERROR([cannot use required functions]))

AC_CONFIG_FILES([Makefile
                 src/productOG/Makefile src/minimize/Makefile
                 tests/Makefile tests/package.m4])
AC_CONFIG_FILES([tests/cover.sh], [chmod +x tests/cover.sh])
AC_OUTPUT
