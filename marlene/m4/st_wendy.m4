# serial 1
# thanks to Niels for this code snipplet
AC_DEFUN([AC_ST_WENDY],[

AC_PATH_PROGS(WGET, [wget], [])
AC_PATH_PROGS(GTAR, [gtar tar], [])

export PATH=`pwd`:$PATH
AC_PATH_PROG(WENDY, [wendy], [])
if test "${WENDY}" = ""; then
  AC_MSG_WARN([Wendy was not found! It is now downloaded and built.])

  if test "${WGET}" != "" -a "${GTAR}" != ""; then
    AC_MSG_NOTICE([downloading Wendy from service-technology.org...])
    ${WGET} http://service-technology.org/files/wendy/wendy.tar.gz > /dev/null 2> /dev/null
    ${GTAR} xzf  wendy.tar.gz 

    WENDYDIR=`ls -d1 */ | grep wendy`

    AC_MSG_NOTICE([compiling Wendy...])
    cd $WENDYDIR
    ./configure --prefix=`pwd`/local > /dev/null 2> /dev/null
    make all > /dev/null 2> /dev/null
    make install > /dev/null 2> /dev/null
    cd ..

    cp "$WENDYDIR"local/bin/wendy* .
    rm -fr $WENDYDIR wendy.tar.gz

    AC_PATH_PROG(WENDY, [wendy], [])
  fi
else
AC_PATH_PROGS_FEATURE_CHECK([WENDYVERSION], [wendy], [[ac_cv_path_WENDYVERSION=`$ac_path_WENDYVERSION --version | ${SED} -e 's/Wendy //'`]])
AC_MSG_CHECKING([Wendy version])
AC_MSG_RESULT([${ac_cv_path_WENDYVERSION}])
echo ${ac_cv_path_WENDYVERSION} | ${AWK} '{if (@S|@1 >= $1) exit 0; else exit 1}' || AC_MSG_WARN([${PACKAGE_NAME} needs at least Wendy $1! See file REQUIREMENTS.])
fi

])
