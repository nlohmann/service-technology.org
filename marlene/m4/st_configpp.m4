# serial 1
AC_DEFUN([AC_ST_CONFIGPP],[

# remember LIBS variable set so far (in case we want to reset it)
ST_CONFIGPP_OLDLIBS=${LIBS}
ST_CONFIGPP_OLDCXXFLAGS=${CXXFLAGS}
ST_CONFIGPP_OLDLDFLAGS=${LDFLAGS}


AC_MSG_CHECKING([whether libconfig++ is accessible using pkg-config])

ST_CONFIGPP_PRESENT=`pkg-config --exists libconfig++`
ST_CONFIGPP_PRESENT=$?

if test "${ST_CONFIGPP_PRESENT}" = 0; then
    AC_MSG_RESULT([yes (adding paths to includes and libraries)])
    CXXFLAGS+=" -I`pkg-config --variable=includedir libconfig++`"
    LDFLAGS+=" -L`pkg-config --variable=libdir libconfig++`"
else
    # NO: we cannot link agains the installed libconfig++
    AC_MSG_RESULT([no (not found)])
    LIBCONFIG="0"
fi

# check whether libconfig++ is present and we can link against it
AC_LANG([C++])
AC_CHECK_LIB(config++, config_init)
AC_MSG_CHECKING([whether to use installed libconfig++])
if test "${ac_cv_lib_configpp_config_init}" = "no"; then
    # NO: we cannot link agains the installed libconfig++
    AC_MSG_RESULT([no (not usable)])
    LIBCONFIG="0"
    LIBS=${ST_CONFIGPP_OLDLIBS}
    CXXFLAGS=${ST_CONFIGPP_OLDCXXFLAGS}
    LDFLAGS=${ST_CONFIGPP_OLDLDFLAGS}
else
    AC_PATH_PROGS_FEATURE_CHECK(PKGCONFIG, [pkg-config], [configpp_version=`pkg-config --modversion libconfig++`])
    if test "`echo $configpp_version | ${AWK} '{if (@S|@1 >= $1) print 0; else print 1}'`" = 1; then
        # NO: installed libconfig++ is too old
        LIBCONFIG="0"
        AC_MSG_RESULT([no (version $configpp_version too old)])
        AC_DEFINE([HAVE_LIBCONFIG__], [0])
        LIBS=${ST_CONFIGPP_OLDLIBS}
        CXXFLAGS=${ST_CONFIGPP_OLDCXXFLAGS}
        LDFLAGS=${ST_CONFIGPP_OLDLDFLAGS}
    else
        # YES: everything is fine
        LIBCONFIG="1"
        LDFLAGS+=" -Wl,-R`pkg-config --variable=libdir libconfig++` "
        AC_MSG_RESULT([yes (version $configpp_version)])
    fi
fi

AC_SUBST(LIBCONFIG)
])
