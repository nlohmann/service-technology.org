# serial 1
AC_DEFUN([AC_JAVAPATH],[

AC_MSG_NOTICE([setting path to java classes...])

TEMPVAR=`pwd`/java
AC_SUBST(JAVA, $TEMPVAR)

AC_MSG_NOTICE([extracting jar files...])

cd java
for file in `ls | grep .jar`
do
AC_MSG_NOTICE([extracting $file...])
unzip -ouq $file
done

cd ..

])