#!/bin/sh

TARNAME=$1
TOOLNAME=`echo $1 | sed 's/.tar.gz//'`

tar xfz $TARNAME
rm -fr tmp
mv $TOOLNAME tmp

PREFIX=`pwd`/$TOOLNAME

cd tmp
CC=$CC CXX=$CXX CPPFLAGS=-O3 ./configure --prefix=$PREFIX --disable-assert
make all install
cd ..

rm -fr tmp
