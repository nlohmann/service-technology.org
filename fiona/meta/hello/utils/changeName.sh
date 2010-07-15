#!/usr/bin/env bash
# change the name of the Hello World tool into something completely different...
#

# is the number of arguments correct (= 3)?
if [ $# -ne 3 ]
then
    echo "Error!" 
    echo "Syntax: $0 newname NewName NEWNAME"
    exit
fi

# rename the variables
NEWNAME=$1
NEWBIGNAME=$2
NEWBIGBIGNAME=$3

# test if the script was started in "/hello/utils/"
if [ ! -f changeName.sh ]
then
    echo "script wasn't started in [.../hello/utils]!"
    exit
fi

# change the directory
cd ../..

# does a folder with the given name already exist?
if [ -d $NEWNAME ]
then
    echo "Directory [$NEWNAME] already exists"
    echo "[$NEWNAME] will be completely removed and then overwritten..."
    echo "Okay to remove? ( y/n ) : "
    read answer
    if [ "$answer" != "y" ] && [ "$answer" != "Y" ]
    then
	echo "Aborting"
	exit
    fi
    echo "deleting [$NEWNAME]..."
    rm -r -f $NEWNAME
    echo "...done"
fi

# export the whole folder of the 'Hello World' program to a new one with the given name
echo "exporting directory [hello] into directory [$NEWNAME]..."
svn export hello $NEWNAME
echo "...done"

# go into that just created folder
cd $NEWNAME

# delete the folder 'utils', which isn't needed any more
rm -r -f utils

# declaration of a function to change all appearances of "hello" into the name of the new program
function changeit () {
    sed -e s/hello/$NEWNAME/g -e s/Hello/$NEWBIGNAME/g -e s/HELLO/$NEWBIGBIGNAME/g $1 > $1.tmp
    # caution: $1 is the given argument of the function, NOT the argument of the main script
    cp $1.tmp $1
    rm $1.tmp
}

# use that function to change some files
echo "changing [hello] into [$NEWNAME]..."
changeit AUTHORS
changeit NEWS
changeit README
changeit THEORY
changeit REQUIREMENTS
changeit THANKS
changeit ChangeLog
changeit configure.ac

changeit ./src/cmdline.ggo
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
changeit ./doc/Makefile.am

changeit ./tests/testsuite.at

# these 2 files have 'hello' in their filenames, so we just rename them
changeit ./src/hello.conf.in
cp ./src/hello.conf.in ./src/$NEWNAME.conf.in
rm ./src/hello.conf.in

changeit ./doc/hello.texi
cp ./doc/hello.texi ./doc/$NEWNAME.texi
rm ./doc/hello.texi
echo "...done"

# ...is this really necessary???
echo "doing a <chmod 775> on all shell-scripts..."
find . -name *.sh -exec chmod 775 {} \;
# this one might be necessary...
chmod 775 ./tests/lcov/lcov
echo "...done"

echo "Job done! Bye bye!"