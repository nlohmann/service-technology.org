#!/bin/sh

mkdir -p bin/common/bin
chmod a+x bin/common

cd bin/common

PATH=`pwd`/bin:/usr/local/bin:/usr/bin:/bin:$PATH
DIR=`pwd`

# Genet
#cp `which genet` bin
#chmod a+x bin/genet


# LoLA
TOOL=lola
wget http://esla.informatik.uni-rostock.de:8080/job/lola/185/artifact/lola/lola.tar.gz
# wget http://esla.informatik.uni-rostock.de:8080/job/$TOOL/lastSuccessfulBuild/artifact/$TOOL/$TOOL.tar.gz
# wget http://service-technology.org/files/lola/lola.tar.gz
tar xfz $TOOL.tar.gz
cd $TOOL*
./configure --prefix=$DIR --sysconfdir=$DIR/bin
make all lola-statespace lola-statespace1 lola-bpm-liveprop1 lola-deadtransition lola-findpath lola-bpm-statepredicate1 lola-liveprop lola-boundedplace lola-boundednet lola-statepredicate lola-full lola-full1
make install
cd ..
rm -fr $TOOL* $TOOL.tar.gz


for TOOL in genet pnapi wendy candy marlene mia bpel2owfn rachel x2dot compilers sayo uml2owfn linda yasmina2
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

