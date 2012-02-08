AC_INIT([Patricia], 1.0, patricia@service-technology.org, patricia, http://service-technology.org/patricia)

AC_MSG_NOTICE([ ____       _        _      _       ])
AC_MSG_NOTICE([|  _ \ __ _| |_ _ __(_) ___(_) __ _ ])
AC_MSG_NOTICE([| |_) / _` | __| '__| |/ __| |/ _` |])
AC_MSG_NOTICE([|  __/ (_| | |_| |  | | (__| | (_| |])
AC_MSG_NOTICE([|_|   \__,_|\__|_|  |_|\___|_|\__,_|])
AC_MSG_NOTICE([scheduling calls to verification tools])
AC_MSG_NOTICE([${PACKAGE_URL}])
AC_MSG_NOTICE([version ${PACKAGE_VERSION}])
AC_MSG_NOTICE()

AC_CONFIG_SRCDIR([src/main.c])
AC_CONFIG_HEADERS([src/config.h])
AC_CONFIG_AUX_DIR([build-aux])

# use the service-technology.org macros
AC_ST_BASIC
# AC_ST_LOLA([1.14])
AC_ST_AUTOTEST
# AC_ST_PNAPI([4.02])
AC_ST_VERBOSE

AX_PTHREAD()
LIBS="$PTHREAD_LIBS $LIBS"
CFLAGS="$CFLAGS $PTHREAD_CFLAGS"
CC="$PTHREAD_CC"

# check for verification tools
AC_PATH_PROG(LOLA_STATEPREDICATE, [lola-statepredicate], [])
AC_PATH_PROG(LOLA_FINDPATH, [lola-findpath], [])
AC_PATH_PROG(SARA, [sara], [])

# check for required functions and die if they are not found
AC_CHECK_FUNCS([popen pclose],,AC_MSG_ERROR([cannot use required functions]))

# write files
AC_CONFIG_FILES([
	Makefile src/Makefile doc/Doxyfile doc/Doxydetails tests/Makefile tests/package.m4])
AC_CONFIG_FILES([tests/cover.sh], [chmod +x tests/cover.sh])
AC_OUTPUT
