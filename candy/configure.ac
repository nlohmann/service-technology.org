# initial data for autoconf
AC_INIT([Candy], [0.1-unreleased], [richard.mueller@service-technology.org])

##########################################################################################
# checks for autoconf version 2.59
AC_PREREQ(2.59)

# initialize automake
AM_INIT_AUTOMAKE #(gnits)

# checks for specific files (random sample)
AC_CONFIG_SRCDIR([src/main.cc])

# write test results as #defines into header file
AC_CONFIG_HEADERS([src/config.h])

# checks whether to enable assertions in the style of assert.h
AC_HEADER_ASSERT

# checks for programs
AC_PROG_CXX
AM_PROG_CC_C_O
AC_PROG_LEX
AC_PROG_YACC
AC_PROG_SED
AC_PROG_GREP

# checks for programs
AM_MISSING_PROG(GENGETOPT, gengetopt)
AM_MISSING_PROG(HELP2MAN, help2man)

# look up canonical build name and write it to config.h
AC_CANONICAL_BUILD
AC_DEFINE_UNQUOTED([CONFIG_BUILDSYSTEM], ["$build"], [The platform.])

# check for functions
AC_CHECK_FUNCS([popen pclose],,AC_MSG_WARN([cannot use popen or pclose]))

##########################################################################################
# write files
AC_CONFIG_FILES([Makefile
                 src/Makefile
                 doc/Makefile doc/Doxyfile
                 man/Makefile])
AC_OUTPUT
