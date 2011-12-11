AC_INIT(Maxis, 0.2-unreleased, parnjai@informatik.hu-berlin.de, maxis, http://service-technology.org/maxis )

AC_MSG_NOTICE([ __  __              _       ])
AC_MSG_NOTICE([|  \/  | ___  __  __(_)  ___ ])
AC_MSG_NOTICE([| |\/| |/ _ \ \ \/ /| | (  _|])
AC_MSG_NOTICE([| |  | | (_| | >  < | | _\ \ ])
AC_MSG_NOTICE([|_|  |_|\__,_|/_/\_\|_||___/ ])
AC_MSG_NOTICE([Synthesizing Mamximal Partner for services])
AC_MSG_NOTICE([${PACKAGE_URL}])
AC_MSG_NOTICE([version ${PACKAGE_VERSION} ])
AC_MSG_NOTICE()

AC_CONFIG_SRCDIR(src/main.cc)
AC_CONFIG_HEADERS(src/config.h)
AC_CONFIG_AUX_DIR([build-aux])

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
