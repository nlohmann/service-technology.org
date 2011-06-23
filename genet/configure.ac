AC_INIT(Genet, 1.1, jcarmona@lsi.upc.edu, genet, http://www.lsi.upc.edu/~jcarmona/genet.html)

AC_MSG_NOTICE([   ____                 _    ])
AC_MSG_NOTICE([  / ___| ___ _ __   ___| |_  ])
AC_MSG_NOTICE([ | |  _ / _ \ '_ \ / _ \ __| ])
AC_MSG_NOTICE([ | |_| |  __/ | | |  __/ |_  ])
AC_MSG_NOTICE([  \____|\___|_| |_|\___|\__| ])
AC_MSG_NOTICE()
AC_MSG_NOTICE([ http://www.lsi.upc.edu/~jcarmona/genet.html ])
AC_MSG_NOTICE([ version ${PACKAGE_VERSION} ])
AC_MSG_NOTICE()

AC_CONFIG_SRCDIR([src/main.cpp])
AC_CONFIG_HEADERS([src/config.h])
AC_CONFIG_AUX_DIR([build-aux])

AM_INIT_AUTOMAKE(gnu)
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

# parameters for Libtool
AC_PROG_LIBTOOL
AC_LIBTOOL_WIN32_DLL
AC_CONFIG_MACRO_DIR([m4])

# check for programs needed to compile
AC_PROG_CXX
AM_PROG_CC_C_O
AC_PROG_SED
AC_PROG_GREP
AC_PROG_AWK
AC_PROG_RANLIB

# look up canonical build name and write it to config.h
AC_CANONICAL_BUILD
AC_DEFINE_UNQUOTED([CONFIG_BUILDSYSTEM], ["$build"], [The platform.])

# check for functions
AC_CHECK_FUNCS([popen pclose],,AC_MSG_WARN([cannot use pipes -- expect bad performance]))

# needed to compile CUDD on x86_64 machines
AC_CHECK_SIZEOF([void*])

# In case we compile for 64 bit (either explicitly with "--using-64bit" or
# implicitly on a x86_64 system), we have to set a preprocessor definition
# "SIZEOF_VOID_P" for CUDD.
# (the variable "$ac_cv_sizeof_voidp" is set by "AC_CHECK_SIZEOF([void*])")
if test "$ac_cv_sizeof_voidp" = "8" -o "$enable_64bit" = "yes"; then
  CUDD_XFLAGS=$CUDD_XFLAGS" -DSIZEOF_VOID_P=8"
fi


# allow the configure script to control assertions
AC_HEADER_ASSERT

# Checks for header files.
#AC_INCLUDES_DEFAULT()
AC_HEADER_RESOLV
AC_CHECK_HEADERS([sys/malloc.h malloc.h], [], [],
[[#ifdef HAVE_SYS_MALLOC_H
#include <sys/malloc.h>
#endif
#ifdef HAVE_MALLOC_H
#include <malloc.h>
#endif
]])
AC_CHECK_HEADERS([stdarg.h], [], [],
[[#ifdef HAVE_STDARG_H
#include <stdarg.h>
#endif
]])

AC_CHECK_HEADERS([sys/stat.h], [], [],
[[#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif
]])

AC_CHECK_HEADERS([ctype.h], [], [],
[[#ifdef HAVE_CTYPE_H
#include <ctype.h>
#endif
]])

AC_CHECK_HEADERS([netinet/tcp.h], [], [],
[[#ifdef HAVE_NETINET_TCP_H
#include <netinet/tcp.h>
#endif
]])

AC_CHECK_HEADERS([sys/socket.h socket.h], [], [],
[[#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#ifdef HAVE_SOCKET_H
#include <socket.h>
#endif
]])

# Checks for GNULIB stuff
AC_CHECK_HEADERS([include/cwctype])
AC_CHECK_FUNC([iswblank])

# Checks for JNI integration
AC_CHECK_HEADERS([jni.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T
AC_TYPE_INT8_T
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT64_T
AC_TYPE_INTPTR_T
AC_TYPE_UINT8_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINTPTR_T
AC_C_INLINE


# Checks for library functions.
AC_CHECK_FUNCS([memmove memset strdup accept])

AC_SUBST(CUDD_XFLAGS)

# write files
AC_CONFIG_FILES([Makefile
                 src/libgenet.pc 
                 src/Makefile
                 libs/Makefile
                 libs/cudd/Makefile
                 libs/alglib/Makefile
                 utils/pccts/Makefile
                 utils/pccts/antlr/Makefile
                 utils/pccts/dlg/Makefile
                 utils/pccts/h/Makefile
                 utils/pccts/sorcerer/Makefile
                 utils/pccts/support/genmk/Makefile
                 maintainer/Makefile
                 ])
AC_OUTPUT


