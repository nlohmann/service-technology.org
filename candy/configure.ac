AC_INIT([Candy], [1.0], [candy@service-technology.org], candy,
http://service-technology.org/candy)

AC_MSG_NOTICE([  ____                 _       ])
AC_MSG_NOTICE([ / ___| __ _ _ __   __| |_   _ ])
AC_MSG_NOTICE([| |    / _` | '_ \ / _` | | | |])
AC_MSG_NOTICE([| |___| (_| | | | | (_| | |_| |])
AC_MSG_NOTICE([ \____|\__,_|_| |_|\__,_|\__, |])
AC_MSG_NOTICE([                         |___/ ])
AC_MSG_NOTICE([synthesizing cost efficient partners for services])
AC_MSG_NOTICE([${PACKAGE_URL}])
AC_MSG_NOTICE([version ${PACKAGE_VERSION}])
AC_MSG_NOTICE()

AC_CONFIG_SRCDIR([src/main.cc])
AC_CONFIG_HEADERS([src/config.h])
AC_CONFIG_AUX_DIR([build-aux])

# use the service-technology.org macros
AC_ST_BASIC
AC_ST_AUTOTEST
AC_ST_VERBOSE

# search for optional tools needed by "make check"
AC_PATH_PROG(MARLENE,     [marlene], not found)
AC_PATH_PROG(VALGRIND,    [valgrind], not found)
AC_PATH_PROG(WENDY,       [wendy], not found)

# check for required functions and die if they are not found
AC_CHECK_FUNCS([popen pclose],,AC_MSG_ERROR([cannot use required functions]))

# create configured files (Makefiles, scripts, etc.)
AC_CONFIG_FILES([Makefile
                 src/Makefile src/candy.conf
                 doc/Makefile doc/Doxyfile doc/Doxydetails
                 tests/Makefile tests/package.m4])
AC_CONFIG_FILES([tests/cover.sh], [chmod +x tests/cover.sh])
AC_OUTPUT
