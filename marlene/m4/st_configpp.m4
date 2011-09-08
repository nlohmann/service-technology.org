# serial 1
AC_DEFUN([AC_ST_CONFIGPP],[

# remember LIBS variable set so far (in case we want to reset it)
ST_CONFIGPP_OLDLIBS=${LIBS}

# check whether libconfig++ is present and we can link against it
AC_LANG([C++])
AC_CHECK_LIB(config++, config_init)

# check whether we can use an installed libconfig++
AC_MSG_CHECKING([whether to use installed libconfig++])
  if test "${ac_cv_lib_configpp_config_init}" = "no"; then
    # NO: we cannot link agains the installed libconfig++
    AC_MSG_RESULT([no (not present)])
    LIBCONFIG="0"
    LIBS=${ST_CONFIGPP_OLDLIBS}
  else
    AC_PATH_PROGS_FEATURE_CHECK(PKGCONFIG, [pkg-config], [configpp_version=`pkg-config --modversion libconfig++ 2> /dev/null`])
    if test "`echo $configpp_version | ${AWK} '{if (@S|@1 >= $1) print 0; else print 1}'`" = 1; then
      # NO: installed libconfig++ is too old
      LIBCONFIG="0"
      AC_MSG_RESULT([no (version $configpp_version too old)])
      AC_DEFINE([HAVE_LIBCONFIG__], [0])
      LIBS=${ST_CONFIGPP_OLDLIBS}
    else
      # YES: everything is fine
      LIBCONFIG="1"
      AC_MSG_RESULT([yes (version $configpp_version)])
    fi
  fi
AC_SUBST(LIBCONFIG)
])
