AC_INIT([UML2oWFN],[2.00-unreleased],[znamirow@informatik.hu-berlin.de],[uml2owfn])

# prerequisites for Autoconf and Automake
AC_PREREQ(2.59)
AM_INIT_AUTOMAKE

# Location of main() function and name of config header file.
AC_CONFIG_SRCDIR(src/uml2owfn.cc)
AC_CONFIG_HEADERS(src/uml2owfn.h)

# Compute the canonical build-system type variable, build, and its three
# individual parts build_cpu, build_vendor, and build_os and write it to
# the config header file
AC_CANONICAL_BUILD
AC_DEFINE_UNQUOTED([BUILDSYSTEM], ["$build"], [The platform.])

# Check for C/C++.
AC_PROG_CXX
AM_PROG_CC_C_O

# flags set by the configure script
configured_CFLAGS=""
configured_CXXFLAGS=""
configured_LDFLAGS=""

############################################################################
# Check for programs.
############################################################################

# GNU parser generators
AC_PROG_YACC
AC_PROG_LEX

# *nix command-line tools for ./doc
AC_PROG_SED
AC_PROG_AWK
AC_PROG_GREP

# Dot
AC_CHECK_PROG(DOT, dot, dot, not found)
if test "$DOT" = "not found"; then
 AC_MSG_WARN([Graphviz dot was not found.])
 AC_DEFINE(HAVE_DOT, 0, [Graphviz dot can not be used on this system.])
else
 AC_DEFINE(HAVE_DOT, 1, [Graphviz dot can be used on this system.])
fi

############################################################################
# further compiler flangs and definitions.
############################################################################

#############################################################################
# In the following, command line paraemters for the configure script are
# defined. Each parameter is also written into a Makefile conditional to
# later set compliler flags accordingly. Finally, the result is also written
# into the configuration header "config.h".
#############################################################################

# parameter "--enable-debug"
AC_ARG_ENABLE(debug,
  AS_HELP_STRING([--enable-debug],[enable verbose debug messages]),
  [enable_debug=${enableval}], [enable_debug="no"])
AC_MSG_CHECKING([whether to compile debug statements])
AC_MSG_RESULT($enable_debug)
AC_DEFINE_UNQUOTED(CONFIG_ENABLEDEBUG, "$enable_debug", [whether to compile debug statements])

if test "$enable_debug" = "yes"; then
  AC_DEFINE(DEBUG, 1, [Compiling with DEBUG statements.])
fi

# parameter "--disable-assert"
AC_ARG_ENABLE(assert,
  AS_HELP_STRING([--disable-assert],[disable assertions and verbose debug messages (enabled by default)]),
  [enable_assert=${enableval}], [enable_assert=yes])
AC_MSG_CHECKING([whether to use assertions])
AC_MSG_RESULT($enable_assert)
AC_DEFINE_UNQUOTED(CONFIG_ENABLEASSERT, "$enable_assert", [whether to use assertions])

if test "$enable_assert" = "no"; then
  if test "$enable_debug" = "no"; then
    AC_DEFINE(NDEBUG,1,[Turn off assertions and verbose debug messages.])
  else
    AC_MSG_ERROR([Cannot combine `--disable-assert' and `--enable-debug'])
  fi
fi

# export flags
AC_SUBST(configured_CFLAGS)
AC_SUBST(configured_CXXFLAGS)
AC_SUBST(configured_LDFLAGS)

############################################################################
# Set files to generate.
############################################################################
AC_CONFIG_FILES([src/Makefile \
                 tests/Makefile \
                 Makefile])

AC_OUTPUT
