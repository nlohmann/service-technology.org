# Process this file with autoconf to produce a configure script.
AC_PREREQ(2.59)
AC_INIT([GNU BPEL2oWFN],[2.04-unreleased],[bpel2owfn-bug@gnu.org],[bpel2owfn])
AM_INIT_AUTOMAKE


# Set directories.
AC_CONFIG_SRCDIR(src/bpel2owfn.cc)
AC_CONFIG_HEADERS(src/config.h)


# Compute the canonical build-system type variable, build, and its three
# individual parts build_cpu, build_vendor, and build_os.
AC_CANONICAL_BUILD
AC_DEFINE_UNQUOTED([BUILDSYSTEM], ["$build"], [The platform.])


# Checks for C/C++ details.
AC_PROG_CXX


# Checks for header files and functions.
AC_CHECK_HEADERS([windows.h])

# Check for libraries
# lpsolve55 solves linear problems
AC_CHECK_LIB([lpsolve55], [solve])


# Check for optional programs.
AM_MISSING_PROG(HELP2MAN, help2man)
AC_PROG_YACC
AC_PROG_LEX
AC_PROG_SED

AC_CHECK_PROG(GENGETOPT, gengetopt, gengetopt)
AC_CHECK_PROG(KC, kc++, kc++)
AC_CHECK_PROG(DOT, dot, dot, not found)
if test "$DOT" = "not found"; then
 AC_DEFINE(HAVE_DOT, 0, [Graphviz dot can not be used on this system.])
else
 AC_DEFINE(HAVE_DOT, 1, [Graphviz dot can be used on this system.])
fi


# Set files to generate.
AC_CONFIG_FILES([Makefile \
	  doc/Makefile \
	  src/Makefile src/Doxyfile \
	  man/Makefile \
	  tests/Makefile \
	  gui/static_release.sh \
	  maintainer/gnu/Makefile
	  ])

AC_OUTPUT


# fix permissions of generated shell scripts
chmod a+x gui/static_release.sh
