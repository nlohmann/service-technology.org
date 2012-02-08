#!/bin/sh

PREFIX=$(pwd)

tar xfz lola.tar.gz
cd lola-1.16
./configure --prefix=$PREFIX
make lola-findpath lola-statepredicate
make install
cd ..
rm -fr lola-1.16

tar xfz sara.tar.gz
cd sara-1.0
./configure --prefix=$PREFIX --without-pnapi
make all
make install
cd ..
rm -fr sara-1.0

echo ""
echo "------------------------------------"
echo "Please add $(pwd)/bin to your PATH and reconfigure Patricia."
echo "------------------------------------"
echo ""