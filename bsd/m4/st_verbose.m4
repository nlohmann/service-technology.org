# serial 1
AC_DEFUN([AC_ST_VERBOSE],[

  # syslogging can be configured with `--enable-syslog'
  AC_ARG_ENABLE(syslog,
    AS_HELP_STRING([--enable-syslog],[Status messages are printed to standard output by default. Using this parameter, they are additionally added to the syslog.]),
    AC_DEFINE([USE_SYSLOG], 1, [whether to use syslog]))

  # check for required functions and die if they are not found
  AC_LANG([C++])
  AC_CHECK_FUNCS([mktemp vfprintf],,AC_MSG_ERROR([cannot use required functions]))

])