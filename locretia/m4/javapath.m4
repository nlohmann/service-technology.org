# serial 1
AC_DEFUN([AC_JAVAPATH],[

#AC_PATH_PROG(WGET, [wget], [])

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

#AC_MSG_NOTICE([downloading Java JDK from service-technology.org...])
#${WGET} http://download.oracle.com/otn-pub/java/jdk/7u7-b10/jdk-7u7-linux-i586.tar.gz
#JDKDIR=`ls -d1 */ | grep jdk`
#AC_MSG_NOTICE([directory: $JDKDIR])
])