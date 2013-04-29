#!/bin/sh

TARNAME=$1
TOOLNAME=`echo $1 | sed 's/.tar.gz//'`

tar xfz $TARNAME
rm -fr tmp
mv $TOOLNAME tmp

PREFIX=`pwd`/$TOOLNAME

cd tmp
CC=$CC CXX=$CXX ./configure --prefix=$PREFIX
make all install
cd ..

rm -fr tmp
