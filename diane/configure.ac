AC_INIT(Diane, 1.1, diane@service-technology.org, diane, http://service-technology.org/diane)

AC_MSG_NOTICE([ ____  _                  ])
AC_MSG_NOTICE([|  _ \(_) __ _ _ __   ___ ])
AC_MSG_NOTICE([| | | | |/ _` | '_ \ / _ \\])
AC_MSG_NOTICE([| |_| | | (_| | | | |  __/])
AC_MSG_NOTICE([|____/|_|\__,_|_| |_|\___|])
AC_MSG_NOTICE([decomposing Petri nets])
AC_MSG_NOTICE([http://service-technology.org/diane])
AC_MSG_NOTICE([version ${PACKAGE_VERSION}])
AC_MSG_NOTICE()

AC_CONFIG_SRCDIR([src/main.cc])
AC_CONFIG_HEADERS([src/config.h])
AC_CONFIG_AUX_DIR([build-aux])

# use the service-technology.org macros
AC_ST_BASIC
AC_ST_AUTOTEST
AC_ST_PNAPI([4.02])
AC_ST_VERBOSE

# check and set exact unsigned types
AC_TYPE_UINT8_T

AC_CONFIG_FILES([Makefile libs/pnapi/Makefile doc/Makefile src/Makefile man/Makefile tests/Makefile tests/package.m4 doc/Doxyfile])
AC_CONFIG_FILES([tests/cover.sh], [chmod +x tests/cover.sh])
AC_OUTPUT
