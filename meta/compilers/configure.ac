AC_INIT(compilers, 1.00, compilers@service-technology.org, compilers, http://service-technology.org/compilers)

AC_CONFIG_SRCDIR(src/main.cc)
AC_CONFIG_HEADERS(src/config.h)

AC_PREREQ(2.59)
AM_INIT_AUTOMAKE(gnits)
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
AC_PATH_PROG(WENDY, [wendy], not found)
AC_PATH_PROG(SAYO,  [sayo],  not found)
AC_PATH_PROG(SA2SM, [sa2sm], not found)
AC_PATH_PROG(PETRI, [petri], not found)
AC_PATH_PROG(DIFF,  [diff],  not found)

# check for required functions and die if they are not found
AC_CHECK_FUNCS([popen pclose],,AC_MSG_ERROR([cannot use required functions]))

AC_CONFIG_FILES([Makefile
                 src/Makefile
                 maintainer/Makefile maintainer/debian/changelog.template maintainer/debian/info maintainer/debian/README.Debian.template maintainer/debian/rules maintainer/macports/Portfile
                 tests/Makefile tests/package.m4
                 man/Makefile])
AC_CONFIG_FILES([tests/cover.sh], [chmod +x tests/cover.sh])
AC_OUTPUT
