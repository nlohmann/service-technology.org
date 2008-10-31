#! /bin/bash

source defaults.sh

# copy files to builddir (in case it is a different directory)
#if test \( "$testdir" = "$builddir" \)
#then
#  builddir_cleanup=0
#else 
#  $MKDIR_P $builddir/compiler
#  cp $testdir/compiler/* $builddir/compiler
#  $MKDIR_P $builddir/soundness
#  cp $testdir/soundness/* $builddir/soundness
#  builddir_cleanup=1
#fi

result=0

echo ""
echo "UML2oWFN parser tests"

DIR=$testdir/compiler

	cmd="uml2owfn -i $DIR/TestSuite.xml"
	echo $cmd

	$cmd &> /dev/null
	result=$(($result | $?))


DIR=$testdir/soundness

	cmd="uml2owfn -i $DIR/TestSuite_soundness.xml"
	echo $cmd

	$cmd &> /dev/null
	result=$(($result | $?))


# remove files from builddir
#if [ $builddir_cleanup -eq 1 ] ; then
#  rm -f $builddir/compiler/TestSuite.xml
#  rmdir $builddir/compiler
#  rm -f $builddir/soundness/TestSuite_soundness.txt
#  rm -f $builddir/soundness/TestSuite_soundness.xml
#  rmdir $builddir/soundness
#fi

exit $result
