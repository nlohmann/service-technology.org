#!/bin/bash

DIRS=`find ../../ | grep "configure.ac" | grep -v ".svn" | sed -e 's/configure.ac//g;s/\/\//\//'`

mkdir build
touch NEWS README AUTHORS ChangeLog

echo "AC_INIT([service-technology.org toolbox], `svn info | grep "Revision" | awk '{print $2}'`, tools@service-technology.org, service-tech, http://service-technology.org)" > configure.ac
echo "AM_INIT_AUTOMAKE" >> configure.ac
echo "AC_CONFIG_FILES(Makefile)" >> configure.ac
echo -n "AC_CONFIG_SUBDIRS([" >> configure.ac
echo -n $DIRS >> configure.ac
echo "])" >> configure.ac
echo "AC_OUTPUT" >> configure.ac

echo -n "SUBDIRS = " > Makefile.am
echo $DIRS >> Makefile.am
echo >> Makefile.am
echo "svn-clean:" >> Makefile.am
echo "	-@for sub in \$(SUBDIRS); do \\" >> Makefile.am
echo "		\$(MAKE) svn-clean -C \$\$sub; \\" >> Makefile.am
echo "	done;" >> Makefile.am
echo "	rm -fr AUTHORS COPYING ChangeLog INSTALL Makefile Makefile.in NEWS \\" >> Makefile.am
echo "		README aclocal.m4 autom4te.cache config.log config.status \\" >> Makefile.am
echo "		configure install-sh missing configure.ac Makefile.am" >> Makefile.am

autoreconf -is -Wnone
./configure --prefix=`pwd`/build --docdir=`pwd`/build/doc sysconfdir=`pwd`/build/bin

make all
make -k install
make -k install-html install-pdf AM_MAKEINFOHTMLFLAGS="--no-split"

make svn-clean
