# serial 1
AC_DEFUN([AC_ST_PNAPI],[

# required if any libraries are built
AC_PROG_RANLIB

# remember LIBS variable set so far (in case we want to reset it)
ST_PNAPI_OLDLIBS=${LIBS}

# only use installed Petri Net API if this was configured with --with-pnapi
  AC_ARG_WITH(pnapi,
  AS_HELP_STRING([--without-pnapi],[The configure script will try to link against a preinstalled version of the Petri Net API (libpnapi). If this fails, a shipped version (see 'libs/pnapi') will be used instead. This parameter overrides this check and always uses the shipped version.]),
  [], [with_pnapi=yes]
)

# Check whether PNAPI is present and we can link against it.
AC_LANG([C++])
AC_CHECK_LIB(pnapi, libpnapi_is_present)
AC_MSG_CHECKING([whether to use installed Petri Net API])
if test "x$with_pnapi" != xno -a "${ac_cv_lib_pnapi_libpnapi_is_present}" = "yes"; then
  AC_MSG_RESULT([yes])
  AM_CONDITIONAL(COMPILE_PNAPI, [false])
else
  # these tools are needed to compile the Petri Net API
  AC_PROG_LEX
  AC_PROG_YACC

  if test "x$with_pnapi" != xno; then
    AC_MSG_RESULT([no (library not found or not usable)])
    AM_CONDITIONAL(COMPILE_PNAPI, [true])
  else
    AC_MSG_RESULT([no])
    AM_CONDITIONAL(COMPILE_PNAPI, [true])

    # reset LIBS variable to previous value
    LIBS=${ST_PNAPI_OLDLIBS}
  fi
fi

])
