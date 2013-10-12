#!/bin/sh

PREFIX=`pwd`
PATH=`pwd`/bin:$PATH

cd src

for TOOL in `cat ../tools` genet
do
  tar xfz $TOOL.tar.gz
  cd $TOOL
  ./configure --prefix=$PREFIX --sysconfdir=$PREFIX/bin --without-pnapi

  if [ "$TOOL" = "lola" ]
    then make lola-statespace lola-statespace1
    else make all
  fi

  make install
  cd ..
  rm -fr $TOOL
done

cd ..
