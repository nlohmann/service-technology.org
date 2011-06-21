#!/bin/sh

echo "downloading LoLA..."
wget http://service-technology.org/files/lola/lola.tar.gz > /dev/null 2> /dev/null
tar xfz lola.tar.gz

LOLADIR=`ls -d1 */ | grep lola`
LOLANAME="lola-$RANDOM"

echo "compiling LoLA using configuration $1..."
cd $LOLADIR
./configure --prefix=`pwd`/local > /dev/null 2> /dev/null

cp ../$1 src/userconfig.H

make > /dev/null 2> /dev/null
make install > /dev/null 2> /dev/null
cd ..

cp "$LOLADIR"local/bin/lola $LOLANAME
rm -fr $LOLADIR lola.tar.gz

echo "created binary $LOLANAME"
