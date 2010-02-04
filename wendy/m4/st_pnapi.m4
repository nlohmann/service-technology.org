# serial 1
AC_DEFUN([AC_ST_PNAPI],[

  AC_PROG_RANLIB
  AC_PROG_CXX
  AC_PROG_LEX
  AC_PROG_YACC

  # only use installed Petri Net API if this was configured with --enable-pnapi
    AC_ARG_WITH(pnapi,
    AS_HELP_STRING([--without-pnapi],[do not use installed version of the Petri Net API]),
    [], [with_pnapi=yes]
  )

  # Check whether the PNAPI is present.
  AC_CHECK_LIB(pnapi, main)
  
  # Check whether we link against it.
  AC_LANG([C++])
  AC_CHECK_LIB(pnapi, libpnapi_is_present)
  AC_MSG_CHECKING([whether to use installed Petri Net API])
  if test "x$with_pnapi" != xno -a "${ac_cv_lib_pnapi_main}" = "yes" -a "${ac_cv_lib_pnapi_libpnapi_is_present}" = "yes"; then
    AC_MSG_RESULT([yes])
    AM_CONDITIONAL(COMPILE_PNAPI, [false])
  else
    AC_MSG_RESULT([no])
    AM_CONDITIONAL(COMPILE_PNAPI, [true])
  fi

])
