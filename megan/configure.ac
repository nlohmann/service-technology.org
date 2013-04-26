AC_INIT([Megan], 1.0-unreleased, megan@service-technology.org, megan, http://service-technology.org/megan)

AC_MSG_NOTICE([ __  __                        ])
AC_MSG_NOTICE([|  \/  | ___  __ _  __ _ _ __  ])
AC_MSG_NOTICE([| |\/| |/ _ \/ _` |/ _` | '_ \ ])
AC_MSG_NOTICE([| |  | |  __/ (_| | (_| | | | |])
AC_MSG_NOTICE([|_|  |_|\___|\__, |\__,_|_| |_|])
AC_MSG_NOTICE([             |___/             ])
AC_MSG_NOTICE([dispatching verfication tasks])
AC_MSG_NOTICE([${PACKAGE_URL}])
AC_MSG_NOTICE([version ${PACKAGE_VERSION}])
AC_MSG_NOTICE()

AC_CONFIG_SRCDIR([src/main.cc])
AC_CONFIG_HEADERS([src/config.h])
AC_CONFIG_AUX_DIR([build-aux])

AC_PROG_CXX
AC_PROG_CC
AC_PROG_RANLIB
AM_PROG_CC_C_O

# use the service-technology.org macros
AC_ST_BASIC
AC_ST_AUTOTEST
AC_ST_PNAPI([4.03])
AC_ST_VERBOSE

# look for Kimwitu++
AM_MISSING_PROG(KC, kc++)

AC_DEFINE_UNQUOTED([TOOL_PATH], ["`pwd`/tools"], [The absolute tool path.])

AC_CHECK_SIZEOF(char)
AC_CHECK_SIZEOF(void*)
AC_CHECK_SIZEOF(sockaddr_in,, [#include <sys/types.h>
#ifdef WIN32
#include <winsock.h>
#include <windows.h>
#include <stdint.h>
typedef uint32_t socklen_t;
#else
#include <arpa/inet.h>
#include <sys/socket.h>
#endif])


# Checks for header files.
AC_FUNC_ALLOCA

# write files
AC_CONFIG_FILES([
	Makefile libs/pnapi/Makefile src/Makefile
	doc/Doxyfile tests/Makefile tests/package.m4])
AC_CONFIG_FILES([tests/cover.sh], [chmod +x tests/cover.sh])
AC_OUTPUT
