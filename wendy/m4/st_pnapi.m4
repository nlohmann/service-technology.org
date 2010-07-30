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

# check whether PNAPI is present and we can link against it
AC_LANG([C++])
AC_CHECK_LIB(pnapi, libpnapi_is_present)

# check whether we can use an installed Petri Net API
AC_MSG_CHECKING([whether to use installed Petri Net API])
if test "x$with_pnapi" = xno; then
  # NO: user gave '--without-pnapi' parameter
  AC_MSG_RESULT([no (--without-pnapi)])
  AM_CONDITIONAL(COMPILE_PNAPI, [true])
  LIBS=${ST_PNAPI_OLDLIBS}
else
  if test "${ac_cv_lib_pnapi_libpnapi_is_present}" = "no"; then
    # NO: we cannot link agains the installed Petri Net API
    AC_MSG_RESULT([no (not present)])
    AM_CONDITIONAL(COMPILE_PNAPI, [true])
    LIBS=${ST_PNAPI_OLDLIBS}
  else
    AC_PATH_PROGS_FEATURE_CHECK(PKGCONFIG, [pkg-config], [pnapi_version=`pkg-config --modversion libpnapi 2> /dev/null`])
    if test "`echo $pnapi_version | ${AWK} '{if (@S|@1 >= $1) print 0; else print 1}'`" = 1; then
      # NO: installed Petri Net API is too old
      AC_MSG_RESULT([no (version $pnapi_version too old)])
      AM_CONDITIONAL(COMPILE_PNAPI, [true])
      LIBS=${ST_PNAPI_OLDLIBS}
    else
      # YES: everything is fine
      AC_MSG_RESULT([yes (version $pnapi_version)])
      AM_CONDITIONAL(COMPILE_PNAPI, [false])
    fi
  fi
fi

])
