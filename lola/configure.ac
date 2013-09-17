AC_INIT([LoLA], 1.17, lola@service-technology.org, lola, http://service-technology.org/lola)

AC_MSG_NOTICE([ _          _        _    ])
AC_MSG_NOTICE([| |    ___ | |      / \   ])
AC_MSG_NOTICE([| |   / _ \| |     / _ \  ])
AC_MSG_NOTICE([| |__| (_) | |___ / ___ \ ])
AC_MSG_NOTICE([|_____\\___/|_____/_/   \_\\])
AC_MSG_NOTICE([a low-level analyzer])
AC_MSG_NOTICE([http://service-technology.org/lola])
AC_MSG_NOTICE([version ${PACKAGE_VERSION}])
AC_MSG_NOTICE()

AC_CONFIG_SRCDIR(src/net.cc)
AC_CONFIG_HEADER(src/config.h)
AC_CONFIG_AUX_DIR([build-aux])

AM_INIT_AUTOMAKE(gnits)
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

# check for standard programs needed to compile
AC_PROG_CXX
AM_PROG_CC_C_O
AC_PROG_LEX
AC_PROG_YACC

# use the service-technology.org macros
AC_ST_BASIC
AC_ST_AUTOTEST
AC_ST_VERBOSE

# use drand48 if possible and fall back to rand otherwise
AC_CHECK_FUNCS([drand48])

# check for libconfig++
#AC_CHECK_LIB(config++, main)
#AM_CONDITIONAL(USE_LIBCONFIG, [test ${ac_cv_lib_configpp_main} = "yes"])


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


# write files
AC_CONFIG_FILES([
	Makefile src/Makefile doc/Doxyfile doc/Doxydetails doc/Makefile
	man/Makefile tests/Makefile tests/package.m4 utils/Makefile])
AC_CONFIG_FILES([tests/cover.sh], [chmod +x tests/cover.sh])
AC_OUTPUT

echo
echo "------------------------------------------------------------------------------"
echo " Please have a look at the documentation and edit the file 'src/userconfig.h'"
echo " according to your needs. Then, run 'make' to compile LoLA."
echo
echo " Alternatively, you can compile a preconfigured version of LoLA using a"
echo " configuration from directory 'src/configs'. To use, for example, the"
echo " configuration 'src/configs/deadlock/userconfig.h', run 'make lola-deadlock'."
echo " To compile all preconfigured versions, run 'make all-configs'."
echo "------------------------------------------------------------------------------"
echo
