#!/bin/sh

PREFIX=`pwd`

cd src

TOOL=lola
tar xfz $TOOL.tar.gz
cd $TOOL
./configure --prefix=$PREFIX --sysconfdir=$PREFIX/bin
make lola-statespace
make install
cd ..
rm -fr $TOOL

TOOL=wendy
tar xfz $TOOL.tar.gz
cd $TOOL
./configure --prefix=$PREFIX --sysconfdir=$PREFIX/bin
make all
make install
cd ..
rm -fr $TOOL

TOOL=marlene
tar xfz $TOOL.tar.gz
cd $TOOL
./configure --prefix=$PREFIX --sysconfdir=$PREFIX/bin
make all
make install
cd ..
rm -fr $TOOL

cd ..
