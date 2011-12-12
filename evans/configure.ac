AC_INIT(Evans, 0.1-unreleased, parnjai@informatik.hu-berlin.de, evans, http://service-technology.org/evans )

AC_MSG_NOTICE([ _____                           ])
AC_MSG_NOTICE([| ___|__   _   __ _  _ __    ___ ])
AC_MSG_NOTICE([| \_  \ \ | | / _` || '_ \  (  _|])
AC_MSG_NOTICE([| /__  \ \| |( (_| || | | | _\ \ ])
AC_MSG_NOTICE([|____|  \___| \__,_||_| |_||___/ ])
AC_MSG_NOTICE([                                 ])
AC_MSG_NOTICE([Synthesizing Public View and Substitutable Services])
AC_MSG_NOTICE([${PACKAGE_URL}])
AC_MSG_NOTICE([version ${PACKAGE_VERSION} ])
AC_MSG_NOTICE()

AC_CONFIG_SRCDIR(src/main.cc)
AC_CONFIG_HEADERS(src/config.h)
AC_CONFIG_AUX_DIR([build-aux])

# search for Wendy, and Maxis (nice to have for "make check")
AC_PATH_PROG(WENDY, [wendy], not found)
AC_PATH_PROG(MAXIS, [maxis], not found)
AC_PATH_PROG(PETRI, [petri], not found)

# use the service-technology.org macros
AC_ST_BASIC
AC_ST_LOLA([1.14])
#AC_ST_AUTOTEST
AC_ST_PNAPI([4.02])
AC_ST_VERBOSE

# check for required functions and die if they are not found
AC_CHECK_FUNCS([popen pclose],,AC_MSG_ERROR([cannot use required functions]))

# create configured files (Makefiles, scripts, etc.)
AC_CONFIG_FILES([ Makefile  src/Makefile ])
AC_OUTPUT