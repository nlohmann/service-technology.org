AC_PREREQ(2.60)
AC_INIT([oWFN2BPEL],[4.7],[niels.lohmann@uni-rostock.de],[owfn2bpel])
AM_INIT_AUTOMAKE

# Set directories.
AC_CONFIG_SRCDIR(src/main.cc)
AC_CONFIG_HEADERS(src/config.h)

# Checks for C++ details.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_YACC
AC_PROG_LEX

AC_CANONICAL_BUILD
AC_DEFINE_UNQUOTED([BUILDSYSTEM], ["$build"], [The platform.])

# The name of the LoLA source distribution. The Makefiles will look for a file
# called "src/LOLANAME.tar.gz" to compile LoLA.
AC_SUBST(LOLANAME, lola-1.01)

# Checks for GNU gengetopt and help2man
AC_CHECK_PROG(GENGETOPT, gengetopt, gengetopt)
AM_MISSING_PROG(HELP2MAN, help2man)

# Set files to generate.
AC_CONFIG_FILES([Makefile src/Makefile src/Doxyfile man/Makefile doc/Makefile tests/Makefile])

AC_OUTPUT
