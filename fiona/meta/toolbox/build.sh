#!/bin/bash

mkdir build
touch NEWS README AUTHORS ChangeLog
autoreconf -iv
CFLAGS=-O3 CXXFLAGS=-O3 LDFLAGS=-O3 ./configure --disable-assert --prefix=`pwd`/build --docdir=`pwd`/build/doc sysconfdir=`pwd`/build/bin

make all
make -k install
make -k install-html install-pdf AM_MAKEINFOHTMLFLAGS="--no-split"

make svn-clean
