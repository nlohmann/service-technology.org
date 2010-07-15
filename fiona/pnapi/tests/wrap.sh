#!/usr/bin/env bash

basename="${1##*/}"       # strip trailing path
dirname="${basename%.*}"  # strip extension

oldumask=`umask`

umask 022

#echo -n "TEST: $basename"

# copy the files for the test to the temp dir
mkdir -p $testdir/$dirname
cp $testroot/$dirname.*.* $testdir/$dirname 2>/dev/null

# change to the temp dir and execute script
cd $testdir/$dirname
$testroot/$1
result=$?

umask $oldumask

#echo ""

exit $result
