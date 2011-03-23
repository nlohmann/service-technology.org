# serial 1
# thanks to Niels for this code snipplet
AC_DEFUN([AC_ST_CANDY],[

AC_PATH_PROG(WGET, [wget], [])

export PATH=`pwd`:$PATH
AC_PATH_PROG(CANDY, [candy], [])
if test "${CANDY}" = ""; then
  AC_MSG_WARN([Candy was not found! It is now downloaded and built.])

  AC_MSG_NOTICE([downloading Candy from service-technology.org...])
  ${WGET} http://service-technology.org/files/candy/candy.tar.gz > /dev/null 2> /dev/null
  tar xfz candy.tar.gz

  CANDYDIR=`ls -d1 */ | grep candy`

  AC_MSG_NOTICE([compiling Candy...])
  cd $CANDYDIR
  ./configure --prefix=`pwd`/local > /dev/null 2> /dev/null
  make all > /dev/null 2> /dev/null
  make install > /dev/null 2> /dev/null
  cd ..

  cp "$CANDYDIR"local/bin/candy* .
  rm -fr $CANDYDIR candy.tar.gz

  AC_PATH_PROG(CANDY, [candy], [])
else
AC_PATH_PROGS_FEATURE_CHECK([CANDYVERSION], [candy], [[ac_cv_path_CANDYVERSION=`$ac_path_CANDYVERSION --version | ${SED} -e 's/Candy //'`]])
AC_MSG_CHECKING([Candy version])
AC_MSG_RESULT([${ac_cv_path_CANDYVERSION}])
echo ${ac_cv_path_CANDYVERSION} | ${AWK} '{if (@S|@1 >= $1) exit 0; else exit 1}' || AC_MSG_WARN([${PACKAGE_NAME} needs at least Candy $1! See file REQUIREMENTS.])
fi

])
