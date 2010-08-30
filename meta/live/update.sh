#!/bin/sh

mkdir -p bin/common/bin
chmod a+x bin/common

cd bin/common

PATH=`pwd`/bin:$PATH
DIR=`pwd`

# Genet
cp `which genet` bin
chmod a+x bin/genet


# LoLA
TOOL=lola
#wget http://esla.informatik.uni-rostock.de:8080/job/$TOOL/lastSuccessfulBuild/artifact/$TOOL/$TOOL.tar.gz
wget http://service-technology.org/files/lola/lola.tar.gz
tar xfz $TOOL.tar.gz
cd $TOOL*
./configure --prefix=$DIR --sysconfdir=$DIR/bin
make lola-statespace lola-statespace1
make install
cd ..
rm -fr $TOOL $TOOL.tar.gz


for TOOL in pnapi wendy marlene mia
do
  wget http://esla.informatik.uni-rostock.de:8080/job/$TOOL/lastSuccessfulBuild/artifact/$TOOL/$TOOL.tar.gz
  tar xfz $TOOL.tar.gz
  cd $TOOL
  ./configure --prefix=$DIR --sysconfdir=$DIR/bin
  make
  make install
  cd ..
  rm -fr $TOOL $TOOL.tar.gz
done

