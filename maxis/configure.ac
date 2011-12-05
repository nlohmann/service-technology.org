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

AM_INIT_AUTOMAKE #(gnits)
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

# check for standard programs needed to compile
AC_PROG_CXX
AM_PROG_CC_C_O
AC_PROG_LEX
AC_PROG_YACC
# AC_PROG_SED
AC_PROG_GREP

# allow the configure script to control assertions (just include config.h)
AC_HEADER_ASSERT
AH_BOTTOM([#ifdef __cplusplus
#include <cassert>
#else
#include <assert.h>
#endif])

# search for Fiona, sa2sm and Wendy (nice to have for "make check")
AC_PATH_PROG(FIONA, [fiona], not found)
AC_PATH_PROG(SA2SM, [sa2sm], not found)
AC_PATH_PROG(WENDY, [wendy], not found)
AC_PATH_PROG(WENDY2FIONA, [wendy2fiona], not found)

# check for additional programs
AM_MISSING_PROG(GENGETOPT, gengetopt)
AM_MISSING_PROG(HELP2MAN, help2man)

# export tools needed to determine memory consumption
AC_DEFINE_UNQUOTED(TOOL_AWK, "${AWK}", [awk])
AC_DEFINE_UNQUOTED(TOOL_GREP, "${GREP}", [grep])

# check for required functions and die if they are not found
AC_CHECK_FUNCS([popen pclose],,AC_MSG_ERROR([cannot use required functions]))

# create configured files (Makefiles, scripts, etc.)
AC_CONFIG_FILES([
     Makefile
     src/Makefile
])

AC_OUTPUT