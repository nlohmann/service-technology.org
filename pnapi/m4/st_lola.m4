# serial 1
# AC_ST_LOLA([version], [suffix=statespace])
#
# Example: AC_ST_LOLA([1.16])
# Example: AC_ST_LOLA([1.16], [statepredicate1])
AC_DEFUN([AC_ST_LOLA],[

AC_PATH_PROG(WGET, [wget], [])

if test "$2" = ""; then
  LOLASUFFIX="statespace"
else
  LOLASUFFIX="$2"
fi
AC_SUBST(LOLASUFFIX)

export PATH=`pwd`:$PATH
AC_PATH_PROG(LOLA, [lola-${LOLASUFFIX}], [])
if test "${LOLA}" = ""; then
  AC_MSG_WARN([LoLA was not found! It is now downloaded and built.])

  AC_MSG_NOTICE([downloading LoLA from service-technology.org...])
  ${WGET} http://service-technology.org/files/lola/lola.tar.gz > /dev/null 2> /dev/null
  tar xfz lola.tar.gz

  LOLADIR=`ls -d1 */ | grep lola`

  AC_MSG_NOTICE([compiling LoLA...])
  cd $LOLADIR
  ./configure --prefix=`pwd`/local > /dev/null 2> /dev/null
  make lola-${LOLASUFFIX} > /dev/null 2> /dev/null
  make install > /dev/null 2> /dev/null
  cd ..

  cp "$LOLADIR"local/bin/lola-${LOLASUFFIX}* .
  rm -fr $LOLADIR lola.tar.gz

  AC_PATH_PROG(LOLA, [lola-${LOLASUFFIX}], [])
else
AC_PATH_PROGS_FEATURE_CHECK([LOLAVERSION], [lola-${LOLASUFFIX}], [[ac_cv_path_LOLAVERSION=`$ac_path_LOLAVERSION --version | ${SED} -e 's/LoLA //'`]])
AC_MSG_CHECKING([LoLA version])
AC_MSG_RESULT([${ac_cv_path_LOLAVERSION}])
echo ${ac_cv_path_LOLAVERSION} | ${AWK} '{if (@S|@1 >= $1) exit 0; else exit 1}' || AC_MSG_WARN([${PACKAGE_NAME} needs at least LoLA $1! See file REQUIREMENTS.])
fi

AC_DEFINE_UNQUOTED(CONFIG_LOLA, "\"${ac_cv_path_LOLAVERSION}\"", [LoLA can be used.])

])
