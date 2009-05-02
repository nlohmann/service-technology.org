#!/usr/bin/env bash

basename="${1##*/}"       # strip trailing path
dirname="${basename%.*}"  # strip extension

oldumask=`umask`

umask 022

echo -n "TEST: $basename"

# copy the files for the test to the temp dir
mkdir -p $testdir/$dirname
cp $testroot/$dirname/* $testdir/$dirname

# change to the temp dir and execute script
cd $testdir/$dirname
$testroot/$1
result=$?

umask $oldumask

echo ""

if test $result -ne 0 -a $result -ne 77 ; then
    echo "  unexpected exit code $result"
    echo "  see 'tmp/$dirname/result.log' for more information"
fi

exit $result
