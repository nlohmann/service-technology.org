AC_INIT([LoLA],[1.11],[lola@service-technology.org],[lola])

#############################################################################
# check for prerequisites, programms, headers, and libraries
#############################################################################

# prerequisites for Autoconf and Automake
AC_PREREQ(2.60)
AM_INIT_AUTOMAKE(gnits)

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

# check for additional programs
AM_MISSING_PROG(GENGETOPT, gengetopt)
AM_MISSING_PROG(HELP2MAN, help2man)

# look up canonical build name and write it to config.h
AC_CANONICAL_BUILD
AC_DEFINE_UNQUOTED([BUILDSYSTEM], ["$build"], [The platform.])

# flags set by the configure script
configured_CFLAGS=""
configured_CXXFLAGS=""
configured_LDFLAGS=""


#############################################################################
# In the following, command line paraemters for the configure script are
# defined. Each parameter is also written into a Makefile conditional to
# later set compliler flags accordingly. Finally, the result is also written
# into the configuration header "config.h".
#############################################################################

# parameter "--enable-assert"
AC_ARG_ENABLE(assert,
  AS_HELP_STRING([--disable-assert],[disable assertions and verbose debug messages (enabled by default)]),
  [enable_assert=${enableval}], [enable_assert=yes])
AC_MSG_CHECKING([whether to use assertions])
AC_MSG_RESULT($enable_assert)
AC_DEFINE_UNQUOTED(CONFIG_ENABLEASSERT, "$enable_assert", [whether to use assertions])

if test "$enable_assert" = "no"; then
  AC_DEFINE(NDEBUG,1,[Turn off assertions and verbose debug messages.])
fi

# parameter "--enable-64bit"
AC_ARG_ENABLE(64bit,
  AS_HELP_STRING([--enable-64bit],[build for a 64 bit architecture such as x86_64 or ppc64 (disabled by default)]),
  enable_64bit=${enableval}, enable_64bit="no")
AC_MSG_CHECKING([whether to build for a 64 bit architecture])
AC_MSG_RESULT($enable_64bit)
AC_DEFINE_UNQUOTED(CONFIG_ENABLE64BIT, "$enable_64bit", [whether to build a 64 Bit binary])

if test "$enable_64bit" = "yes"; then
  configured_CFLAGS=$configured_CFLAGS" -m64"
  configured_CXXFLAGS=$configured_CXXFLAGS" -m64"
  configured_LDFLAGS=$configured_LDFLAGS" -m64"
fi

# parameter "--enable-universal"
AC_ARG_ENABLE(universal,
  AS_HELP_STRING([--enable-universal],[build a Mac Universal binary which is executable on Intel and Power PC platforms (disabled by default)]),
  enable_universal=${enableval}, enable_universal="no")
AC_MSG_CHECKING([whether to build Mac universal binary])
AC_MSG_RESULT($enable_universal)
AC_DEFINE_UNQUOTED(CONFIG_ENABLEUNIVERSAL, "$enable_universal", [whether to build a Mac universal binary])

if test "$enable_universal" = "yes"; then
  if test "$enable_dependency_tracking" != no ; then
    AC_MSG_ERROR([Please re-run configure with these options:
  ‘--disable-dependency-tracking --enable-universal’])
  fi
  configured_CFLAGS=$configured_CFLAGS" -isysroot /Developer/SDKs/MacOSX10.5.sdk -arch ppc -arch i386"
  configured_CXXFLAGS=$configured_CXXFLAGS" -isysroot /Developer/SDKs/MacOSX10.5.sdk -arch ppc -arch i386"
  configured_LDFLAGS=$configured_LDFLAGS" -isysroot /Developer/SDKs/MacOSX10.5.sdk -arch ppc -arch i386 -mmacosx-version-min=10.4"
fi

# parameter "--enable-win32"
AC_ARG_ENABLE(win32,
  AS_HELP_STRING([--enable-win32],[build a Windows binary that is independent of a local Cygwin installation (disabled by default)]),
  enable_win32=${enableval}, enable_win32="no")
AC_MSG_CHECKING([whether to build Windows MinGW binary])
AC_MSG_RESULT($enable_win32)
AC_DEFINE_UNQUOTED(CONFIG_ENABLEWIN32, "$enable_win32", [whether to build a Cygwin-independent Windows binary])

if test "$enable_win32" = "yes"; then
  configured_CFLAGS=$configured_CFLAGS" -mno-cygwin -U _WIN32"
  configured_CXXFLAGS=$configured_CXXFLAGS" -mno-cygwin -U _WIN32"
  configured_LDFLAGS=$configured_LDFLAGS" -mno-cygwin -U _WIN32"
fi


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


# export flags
AC_SUBST(configured_CFLAGS)
AC_SUBST(configured_CXXFLAGS)
AC_SUBST(configured_LDFLAGS)


#############################################################################
# create output files
#############################################################################

# Set files to generate.
AC_CONFIG_FILES([Makefile src/Makefile doc/Makefile man/Makefile maintainer/Makefile tests/Makefile utils/Makefile Doxyfile])

AC_OUTPUT

echo
echo "------------------------------------------------------------------------------"
echo " Please have a look at the documentation and edit the file ‘src/userconfig.H’"
echo " according to your needs. Then, run ‘make’ to compile LoLA."
echo
echo " Alternatively, you can compile a preconfigured version of LoLA using a"
echo " configuration from directory ‘src/configs’. To use, for example, the"
echo " configuration ‘src/configs/userconfig.H.deadlock’, run ‘make lola-deadlock’."
echo " To compile all preconfigured versions, run ‘make all-configs’."
echo "------------------------------------------------------------------------------"
echo
