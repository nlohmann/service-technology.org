AC_INIT([Marlene],[1.2-unreleased],[christian.gierds@service-technology.org])

AC_MSG_NOTICE([  ______              _                    ])
AC_MSG_NOTICE([ |  ___ \            | |                   ])
AC_MSG_NOTICE([ | | _ | | ____  ____| | ____ ____   ____  ])
AC_MSG_NOTICE([ | || || |/ _  |/ ___) |/ _  )  _ \ / _  ) ])
AC_MSG_NOTICE([ | || || ( ( | | |   | ( (/ /| | | ( (/ /  ])
AC_MSG_NOTICE([ |_||_||_|\_||_|_|   |_|\____)_| |_|\____) ])
AC_MSG_NOTICE()
AC_MSG_NOTICE([ Generating behavioral adapters ])
AC_MSG_NOTICE([ http://service-technology.org/marlene ])
AC_MSG_NOTICE([ version ${PACKAGE_VERSION} ])
AC_MSG_NOTICE()

AC_CONFIG_SRCDIR([src/marlene.cc])
AC_CONFIG_HEADERS([src/config.h])
AC_CONFIG_AUX_DIR([build-aux])

AM_INIT_AUTOMAKE([gnits])
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

# search for optional tools needed by "make check"
AC_PATH_PROG(WENDY, [wendy], [])
AC_PATH_PROG(CANDY, [candy], [])
AC_PATH_PROG(VALGRIND, [valgrind], not found)
AC_PATH_PROG(DIFF, [diff], not found)

AC_PATH_PROGS(PETRIFY, [petrify4.2 petrify4.1 petrify4.0 petrify], not found)
if test "$PETRIFY" = "not found"; then
  AC_MSG_WARN([Petrify was not found. See file README.])
  AC_DEFINE(CONFIG_PETRIFY, "not found", [Petrify was not found.])
else
  AC_DEFINE_UNQUOTED(CONFIG_PETRIFY, "${PETRIFY}", [Petrify can be used.])
fi
AC_SUBST(PETRIFY)


AC_PATH_PROGS(GENET, [genet], not found)
if test "$GENET" = "not found"; then
  AC_MSG_WARN([GENET not found. See file README.])
  AC_DEFINE(CONFIG_GENET, "not found", [Genet was not found.])
else
  AC_DEFINE_UNQUOTED(CONFIG_GENET, "${GENET}", [Genet can be used.])
fi
AC_SUBST(GENET)

# enable Windows temp files
AC_ARG_ENABLE(windowstemp,
              AS_HELP_STRING([--enable-windowstemp],[sets standard temp file for Marlene to %TEMP%/marlene-XXXXXX (should only set when compiling for pure Win32 binary)]),
              enable_windowstemp=${enableval}, enable_windowstemp="no")
AC_MSG_CHECKING([whether to use Windows temp folder])
AC_MSG_RESULT($enable_windowstemp)
if test "$enable_windowstemp" = "yes" ; then
    TEMPFOLDER=`cygpath --windows $TEMP`
else
    TEMPFOLDER=/tmp/marlene-XXXXXX
fi

AC_SUBST(TEMPFOLDER)

AC_MSG_CHECKING([weather to be sunny])
AC_MSG_RESULT([better should be])

# Checks for header files.
AC_HEADER_SYS_WAIT

# check for required functions and die if they are not found
AC_FUNC_ERROR_AT_LINE
AC_CHECK_FUNCS([popen pclose],,AC_MSG_ERROR([cannot use required functions]))


#############################################################################
# anything else
#############################################################################

# Force @builddir@ substitution. Without this src/Makefile.in lacked @builddir@
# substitution on gruenau.informatik.hu-berlin.de with automake 1.9.6.
AC_SUBST(builddir)

#############################################################################
# In the following, command line paraemters for the configure script are
# defined. Each parameter is also written into a Makefile conditional to
# later set compliler flags accordingly. Finally, the result is also written
# into the configuration header "config.h".
#############################################################################

# substitute configure Flags in Makefiles
AC_SUBST(configured_CXXFLAGS)
AC_SUBST(configured_CFLAGS)
AC_SUBST(configured_LDFLAGS)

AC_CONFIG_FILES([Makefile
                 libs/pnapi/Makefile
                 src/marlene.conf
                 src/Makefile
                 tests/Makefile
                 tests/package.m4
                 doc/Doxyfile
                 doc/Makefile
                 man/Makefile])
AC_CONFIG_FILES([tests/cover.sh], [chmod +x tests/cover.sh])
AC_OUTPUT


# display a warning in case a required tool has not been found
if test "${WENDY}" = ""; then
  echo
  echo "WARNING: Not all required tools have been found. Please read file"
  echo "         REQUIREMENTS and edit the file src/marlene.conf before"
  echo "         using or installing Marlene."
  echo
fi

