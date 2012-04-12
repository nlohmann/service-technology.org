AC_INIT([LoLA], 2.0-unreleased, lola@service-technology.org, lola, http://service-technology.org/lola)

AC_MSG_NOTICE([ _          _        _    ])
AC_MSG_NOTICE([| |    ___ | |      / \   ])
AC_MSG_NOTICE([| |   / _ \| |     / _ \  ])
AC_MSG_NOTICE([| |__| (_) | |___ / ___ \ ])
AC_MSG_NOTICE([|_____\\___/|_____/_/   \_\\])
AC_MSG_NOTICE([a low-level analyzer])
AC_MSG_NOTICE([http://service-technology.org/lola])
AC_MSG_NOTICE([version ${PACKAGE_VERSION}])
AC_MSG_NOTICE()

AC_CONFIG_SRCDIR(src/Core/main.cc)
AC_CONFIG_HEADER(src/config.h)
AC_CONFIG_AUX_DIR([build-aux])

AM_INIT_AUTOMAKE([gnits subdir-objects])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

${CFLAGS="-g"}
${CXXFLAGS="-g"}

# check for standard programs needed to compile
AC_PROG_CXX
AM_PROG_CC_C_O
AC_PROG_LEX
AC_PROG_YACC
AC_PROG_RANLIB

# use the service-technology.org macros
AC_ST_BASIC
AC_ST_AUTOTEST
AC_ST_VERBOSE


# check for POSIX threads
AX_PTHREAD()
LIBS="$PTHREAD_LIBS $LIBS"
CFLAGS="$CFLAGS $PTHREAD_CFLAGS"
CXXFLAGS="$CXXFLAGS $PTHREAD_CFLAGS"
# CC="$PTHREAD_CC"

# search for optional tools needed by "make check"
AC_PATH_PROG(VALGRIND,    [valgrind], not found)

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_INT8_T
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT64_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_TYPE_UINT8_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T

AC_CHECK_SIZEOF(void*)
AC_CHECK_SIZEOF(char)

AC_LANG_PUSH(C++)
AC_CHECK_SIZEOF(index_t,, [#define _CONFIGURING
#include "$srcdir/src/Dimensions.h"])
AC_CHECK_SIZEOF(mult_t,, [#define _CONFIGURING
#include "$srcdir/src/Dimensions.h"])
AC_CHECK_SIZEOF(capacity_t,, [#define _CONFIGURING
#include "$srcdir/src/Dimensions.h"])
AC_CHECK_SIZEOF(cardbit_t,, [#define _CONFIGURING
#include "$srcdir/src/Dimensions.h"])
AC_CHECK_SIZEOF(hash_t,, [#define _CONFIGURING
#include "$srcdir/src/Dimensions.h"])
AC_CHECK_SIZEOF(fairnessAssumption_t,, [#define _CONFIGURING
#include "$srcdir/src/FairnessAssumptions.h"])
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

AC_CHECK_SIZEOF(bool)
AC_LANG_POP(C++)

# Checks for header files.
AC_FUNC_ALLOCA
AC_CHECK_HEADERS([arpa/inet.h inttypes.h libintl.h malloc.h netinet/in.h stddef.h stdlib.h string.h sys/socket.h unistd.h limits.h netdb.h])

# Checks for library functions.
AC_CHECK_FUNCS([asprintf vasprintf snprintf vsnprintf atexit memset socket strchr strerror strdup strtol strsignal signal gethostbyname inet_ntoa kill])

# Collect required libraries if libraries are not present.
AC_CHECK_FUNC(gethostent, , AC_CHECK_LIB(nsl, gethostent))
AC_CHECK_FUNC(setsockopt, , AC_CHECK_LIB(socket, setsockopt))


####################################################################
# Checks for libraries.
# FIXME: Replace `main' with a function in `-lm':
AC_CHECK_LIB([m], [main])

# needed to compile CUDD on x86_64 machines
AC_CHECK_SIZEOF([void*])
CUDD_XFLAGS="-DSIZEOF_VOID_P=$ac_cv_sizeof_voidp"
AC_SUBST(CUDD_XFLAGS)
####################################################################

AC_CHECK_LIB([dl], [main])

# write files
AC_CONFIG_FILES([
	Makefile src/Makefile doc/Doxyfile doc/Doxydetails
	tests/Makefile tests/package.m4 libs/cudd/Makefile libs/sqlite/Makefile])
AC_CONFIG_FILES([tests/cover.sh], [chmod +x tests/cover.sh])
AC_OUTPUT
