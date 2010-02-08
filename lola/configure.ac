AC_INIT([LoLA], 1.15-unreleased, lola@service-technology.org, lola, http://service-technology.org/lola)

AC_MSG_NOTICE([ _          _        _    ])
AC_MSG_NOTICE([| |    ___ | |      / \   ])
AC_MSG_NOTICE([| |   / _ \| |     / _ \  ])
AC_MSG_NOTICE([| |__| (_) | |___ / ___ \ ])
AC_MSG_NOTICE([|_____\\___/|_____/_/   \_\\])
AC_MSG_NOTICE([a low-level analyzer])
AC_MSG_NOTICE([http://service-technology.org/lola])
AC_MSG_NOTICE([version ${PACKAGE_VERSION}])
AC_MSG_NOTICE()

#############################################################################
# check for prerequisites, programms, headers, and libraries
#############################################################################

# prerequisites for Autoconf and Automake
AC_PREREQ(2.60)
AM_INIT_AUTOMAKE(gnits)
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

# where to look for source code and how the config header should be called
AC_CONFIG_SRCDIR(src/net.cc)
AC_CONFIG_HEADER(src/config.h)

# check for generic programs
AC_PROG_CXX
AM_PROG_CC_C_O
AC_PROG_LEX
AC_PROG_YACC
AC_PROG_SED
AC_PROG_AWK
AC_PROG_GREP
AC_PROG_INSTALL

# check for libconfig++
AC_CHECK_LIB(config++, main)
AM_CONDITIONAL(USE_LIBCONFIG, [test ${ac_cv_lib_configpp_main} = "yes"])

# check for additional programs
AM_MISSING_PROG(GENGETOPT, gengetopt)
AM_MISSING_PROG(HELP2MAN, help2man)

# set up autotest
AM_MISSING_PROG(AUTOM4TE, autom4te)
AC_CONFIG_TESTDIR(tests)

# store the SVN revision number
AC_DEFINE_UNQUOTED(VERSION_SVN, "`svnversion -n 2> /dev/null`", [The SVN revision.])

# allow the configure script to control assertions (just include config.h)
AC_HEADER_ASSERT
AH_BOTTOM([#ifdef __cplusplus
#include <cassert>
#else
#include <assert.h>
#endif])

# look up canonical build name and write it to config.h
AC_CANONICAL_BUILD
AC_DEFINE_UNQUOTED([BUILDSYSTEM], ["$build"], [The platform.])


#############################################################################
# check for MPI: if compiler wrappers are found, use them instead of GCC
#############################################################################

AC_ARG_ENABLE(mpi,
  AS_HELP_STRING([--enable-mpi],[compile LoLA with MPI functions (disabled by default)]),
  enable_mpi=${enableval}, enable_mpi="no")
AC_MSG_CHECKING([whether to compile with MPI])
AC_MSG_RESULT($enable_mpi)
AC_DEFINE_UNQUOTED(CONFIG_ENABLEMPI, "$enable_mpi", [whether to compile with MPI])

if test "$enable_mpi" = "yes"; then
  AC_PATH_PROG(MPICC, mpicc, not found)
  AC_PATH_PROG(MPICXX, mpicxx, not found)

  if test "$MPICC" = "not found" -o "$MPICXX" = "not found"; then
    AC_MSG_ERROR([MPI compiler wrappers were not found in the path])
  else
    CC=$MPICC
    CXX=MPICXX
    AC_CHECK_HEADER([mpi.h])
  fi
fi


#############################################################################
# create output files
#############################################################################

# Set files to generate.
AC_CONFIG_FILES([Makefile src/Makefile doc/Doxyfile doc/Makefile man/Makefile  tests/Makefile tests/package.m4 utils/Makefile])

AC_OUTPUT

echo
echo "------------------------------------------------------------------------------"
echo " Please have a look at the documentation and edit the file 'src/userconfig.H'"
echo " according to your needs. Then, run 'make' to compile LoLA."
echo
echo " Alternatively, you can compile a preconfigured version of LoLA using a"
echo " configuration from directory 'src/configs'. To use, for example, the"
echo " configuration 'src/configs/userconfig.H.deadlock', run 'make lola-deadlock'."
echo " To compile all preconfigured versions, run 'make all-configs'."
echo "------------------------------------------------------------------------------"
echo
