AC_INIT([Anica], 1.3-unreleased, anica@service-technology.org, anica, http://service-technology.org/anica)

AC_MSG_NOTICE([    _          _           ])
AC_MSG_NOTICE([   / \   _ __ (_) ___ __ _ ])
AC_MSG_NOTICE([  / _ \ | '_ \| |/ __/ _` |])
AC_MSG_NOTICE([ / ___ \| | | | | (_| (_| |])
AC_MSG_NOTICE([/_/   \_\_| |_|_|\___\__,_|])
AC_MSG_NOTICE([Automated Non-Interference Check Assistent])
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
AC_ST_LOLA([1.16])
AC_ST_AUTOTEST
AC_ST_PNAPI([4.03])
AC_ST_VERBOSE

# search for optional tools needed by "make check"
# AC_PATH_PROG(FIONA,       [fiona], not found)
# AC_PATH_PROG(MARLENE,     [marlene], not found)
# AC_PATH_PROG(MIA,         [mia], not found)
# AC_PATH_PROG(PETRI,       [petri], not found)
AC_PATH_PROG(VALGRIND,    [valgrind], not found)


# check for required functions and die if they are not found
# AC_CHECK_FUNCS([popen pclose],,AC_MSG_ERROR([cannot use required functions]))

# FreeBSD does not know log2()
# AC_CHECK_FUNCS([log2])

# check and set exact unsigned types
# AC_TYPE_UINT8_T
# AC_TYPE_UINT16_T
# AC_TYPE_UINT32_T
# AC_TYPE_SIZE_T
# AC_CHECK_SIZEOF([void*])

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lm':
AC_CHECK_LIB([m], [main])

# needed to compile CUDD on x86_64 machines
AC_CHECK_SIZEOF([void*])
CUDD_XFLAGS="-DSIZEOF_VOID_P=$ac_cv_sizeof_voidp"
AC_SUBST(CUDD_XFLAGS)


# Checks for header files.
AC_FUNC_ALLOCA
AC_CHECK_HEADERS([arpa/inet.h i netinet/in.h sys/socket.h netdb.h])

# Checks for library functions.
AC_CHECK_FUNCS([socket gethostbyname inet_ntoa])

# Collect required libraries if libraries are not present.
AC_CHECK_FUNC(gethostent, , AC_CHECK_LIB(nsl, gethostent))
AC_CHECK_FUNC(setsockopt, , AC_CHECK_LIB(socket, setsockopt))


AC_CHECK_SIZEOF(char)
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



# write files
AC_CONFIG_FILES([
	Makefile libs/cudd/Makefile libs/pnapi/Makefile src/Makefile src/anica.conf doc/Makefile
	doc/Doxyfile tests/Makefile tests/package.m4])
AC_CONFIG_FILES([tests/cover.sh], [chmod +x tests/cover.sh])
AC_OUTPUT

