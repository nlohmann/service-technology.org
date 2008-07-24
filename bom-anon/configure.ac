AC_PREREQ(2.61)
AC_INIT(BOM-Anonymizer, 0.4, niels.lohmann@service-technology.org, bom-anon)
AC_CONFIG_SRCDIR([src/bom-anon.ll])
AC_CONFIG_HEADERS(src/config.h)

AM_INIT_AUTOMAKE

# Checks for programs.
AC_PROG_CXX
AM_PROG_LEX

# For debug purposes.
AC_CANONICAL_BUILD
AC_DEFINE_UNQUOTED([BUILDSYSTEM], ["$build"], [The platform.])

AC_CONFIG_FILES([Makefile src/Makefile doc/Makefile])
AC_OUTPUT
