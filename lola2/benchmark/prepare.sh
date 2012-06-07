#!/bin/sh

BDIR=$(pwd)

rm -fr $BDIR/local
mkdir -p $BDIR/local

cd ..
./configure --disable-assert --prefix=$BDIR/local
make clean
make CXXFLAGS="-O3" CFLAGS="-O3" LDFLAGS="-O3"
make install
