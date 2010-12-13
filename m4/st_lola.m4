# serial 1
AC_DEFUN([AC_ST_LOLA],[

AC_PATH_PROG(LOLA, [lola-statespace], [])
if test "${LOLA}" = ""; then
  AC_MSG_WARN([LoLA was not found! See file REQUIREMENTS.])
else
AC_PATH_PROGS_FEATURE_CHECK([LOLAVERSION], [lola-statespace], [[ac_cv_path_LOLAVERSION=`$ac_path_LOLAVERSION --version | ${SED} -e 's/LoLA //'`]])
AC_MSG_CHECKING([LoLA version])
AC_MSG_RESULT([${ac_cv_path_LOLAVERSION}])
echo ${ac_cv_path_LOLAVERSION} | ${AWK} '{if (@S|@1 >= $1) exit 0; else exit 1}' || AC_MSG_WARN([${PACKAGE_NAME} needs at least LoLA $1! See file REQUIREMENTS.])
fi

])
