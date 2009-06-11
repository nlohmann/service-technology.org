#!/usr/bin/env bash

echo "This script downloads and installs precompiled binaries of Petrify."
read -p "Press enter to display the terms for Petrify. "
echo


echo "----------------------------------------------------------------------------"
echo "Copyright (C) 1999 The Universitat Politecnica de Catalunya. All rights"
echo "reserved. Permission is hereby granted, without written agreement and"
echo "without license or royalty fees, to use, copy, modify, and distribute this"
echo "software and its documentation for any purpose, provided that the above"
echo "copyright notice and the following two paragraphs appear in all copies of"
echo "this software."
echo ""
echo "IN NO EVENT SHALL THE UNIVERSITAT POLITECNICA DE CATALUNYA BE LIABLE TO ANY"
echo "PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES"
echo "ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE"
echo "UNIVERSITAT POLITECNICA DE CATALUNYA HAS BEEN ADVISED OF THE POSSIBILITY OF"
echo "SUCH DAMAGE."
echo ""
echo "THE UNIVERSITAT POLITECNICA DE CATALUNYA SPECIFICALLY DISCLAIMS ANY"
echo "WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF"
echo "MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE PROVIDED"
echo "HEREUNDER IS ON AN \"AS IS\" BASIS, AND THE UNIVERSITAT POLITECNICA DE"
echo "CATALUNYA HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,"
echo "ENHANCEMENTS, OR MODIFICATIONS."
echo "----------------------------------------------------------------------------"
read -p "Do you accept the terms above? [yes/no]? "
if [ "$REPLY" != "yes" ]; then
  exit
fi

files=""

echo

case `uname -o` in
  "Darwin")
    echo -n "Downloading Petrify 4.2 for Mac OS X (Universal)..."
    wget http://www.lsi.upc.edu/~jordicf/petrify/distrib/petrify-4.2-mac.tgz &> /dev/null
    tar xfz petrify-4.2-mac.tgz
    mv petrify-universal petrify
    rm petrify-4.2-mac.tgz petrify-intel petrify-ppc README
    files="petrify"
    binary="petrify"
  ;;

  "Solaris")
    echo -n "Downloading Petrify 4.2 for Solaris (SPARC)..."
    wget http://www.lsi.upc.edu/~jordicf/petrify/distrib/petrify-4.2-sol.tgz &> /dev/null
    tar xfz petrify-4.2-sol.tgz
    cp petrify/bin/petrify ./petrify-solaris
    rm -fr petrify petrify-4.2-sol.tgz
    mv petrify-solaris petrify
    files="petrify"
    binary="petrify"
  ;;

  "GNU/Linux")
    echo -n "Downloading Petrify 4.2 for GNU/Linux (i386)..."
    wget http://www.lsi.upc.edu/~jordicf/petrify/distrib/petrify-4.2-linux.tgz &> /dev/null
    tar xfz petrify-4.2-linux.tgz
    cp petrify/bin/petrify ./petrify-linux
    rm -fr petrify petrify-4.2-linux.tgz
    mv petrify-linux petrify    
    files="petrify"
    binary="petrify"
  ;;
  
  "Cygwin")
    echo -n "Downloading Petrify 4.1 for Windows (Cygwin)..."
    wget http://www.lsi.upc.edu/~jordicf/petrify/distrib/petrify-4.1.zip &> /dev/null
    unzip petrify-4.1.zip &> /dev/null
    rm petrify-4.1.zip cygwin1.dll
    rm -fr petrify4.1
    files="petrify.def petrify.dll petrify4.1.exe"
    binary="petrify4.1.exe"
  ;;

  *)
    echo "System `uname -o` unknown. Check http://www.lsi.upc.edu/~jordicf/petrify/distrib/home.html for Petrify binaries."
    exit
esac

echo " done."
echo
echo "Please enter a directory to which Petrify will be copied. Best would"
read -p "be a directory of your path [/usr/local/bin] "

if [ "$REPLY" == "" ]; then
  target="/usr/local/bin"
else
  target=$REPLY
fi

echo

echo -n "Copying Petrify ($files) to $target..."

cp $files $target &> /dev/null

if [ $? == 0 ]; then
  echo " done."
  echo
  echo "Petrify will now be found in your path and can be called with '$binary'."
  echo
  rm $files
else
  echo " error."
  echo
  echo "There was a problem copying. Maybe you don't have permission to write to"
  echo "$target -- try calling the script as superuser."
  echo
fi
