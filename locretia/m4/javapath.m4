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

SYSTEM=`uname -a`
MACHINE=`uname -m`
IS32=`echo $MACHINE | grep 86 | wc -w`
IS64=`echo $MACHINE | grep 64 | wc -w`

AC_MSG_NOTICE([Running System: $SYSTEM])
AC_MSG_NOTICE([Machine: $MACHINE])

TESTVAR1=`ls -1 jre*/ 2> /dev/null | grep lib | wc -w`
if test "${TESTVAR1}" = "0" 
then
 AC_MSG_NOTICE([Java JRE not found.])
 TESTVAR2=`ls -1 *.tar.gz 2> /dev/null | grep jre | wc -w`
 if test "${TESTVAR2}" = "0" 
 then
  AC_MSG_NOTICE([Downloading JRE...])
  if test "${IS32}" = "1"
  then
   ${WGET} http://www2.informatik.hu-berlin.de/~heiden/jre-7u7-linux-i586.tar.gz > /dev/null 2> /dev/null
  fi
  if test "${IS64}" = "1"
  then
   ${WGET} http://www2.informatik.hu-berlin.de/~heiden/jre-7u7-linux-x64.tar.gz > /dev/null 2> /dev/null
  fi
 else
  AC_MSG_NOTICE([tar archive was found...])
 fi
if test "${IS32}" = "1"
then
 tar xfz jre-7u7-linux-i586.tar.gz
fi
if test "${IS64}" = "1"
then
 tar xfz jre-7u7-linux-x64.tar.gz
fi
else
 AC_MSG_NOTICE([JAVA JRE was found...])
fi

if test "${IS32}" = "1"
then
 JRELIBDIR=`pwd`/`ls -d1 */ | grep jre`lib/i386/client
 JDKLIBDIR=/libs/libjvm/linux32
fi
if test "${IS64}" = "1"
then
 JRELIBDIR=`pwd`/`ls -d1 */ | grep jre`lib/amd64/server
 JDKLIBDIR=/libs/libjvm/amd64
fi

AC_MSG_NOTICE([Java JRE library dir: $JRELIBDIR])
AC_MSG_NOTICE([Java JDK library dir: $JDKLIBDIR])
AC_SUBST(JRELIB, $JRELIBDIR)
AC_SUBST(JDKLIB, $JDKLIBDIR)

])