#!/usr/bin/env bash
# change the name of the Newprog World tool into something completely different...
#
NEWNAME=$1
NEWBIGNAME=$2
NEWBIGBIGNAME=$3

#assuming the script is started in "/hello/utils/"
cd ../..

function changeit () {
sed s/hello/$NEWNAME/ $1 > $1.tmp
sed s/Hello/$NEWBIGNAME/ $1.tmp > $1
sed s/HELLO/$NEWBIGBIGNAME/ $1 > $1.tmp
cp $1.tmp $1
rm $1.tmp
}

changeit AUTHORS
changeit NEWS
changeit README
changeit THEORY
changeit REQUIREMENTS
changeit THANKS
changeit ChangeLog
changeit configure.ac

changeit ./src/cmdline.ggo
changeit ./src/hello.conf.in
changeit ./src/main.cc
changeit ./src/Makefile.am
changeit ./src/Output.cc
changeit ./src/Output.h
changeit ./src/verbose.cc
changeit ./src/verbose.h
changeit ./src/wendy_lexic.ll
changeit ./src/wendy_syntax.yy

changeit ./man/extra

changeit ./doc/ChangeLog.texi
changeit ./doc/hello.texi
cp ./doc/hello.texi ./doc/$1.texi
rm ./doc/hello.texi
changeit ./doc/Makefile.am

changeit ./tests/testsuite.at