#! /bin/bash

source defaults.sh

# copy files to builddir (in case it is a different directory)
#if test \( "$testdir" = "$builddir" \)
#then
#  builddir_cleanup=0
#else 
#  $MKDIR_P $builddir/compiler
#  cp $testdir/compiler/* $builddir/compiler
#  builddir_cleanup=1
#fi

result=0
DIR=$testdir/compiler

must_filecount=24
must_filecount_filter=20

echo ""
echo "UML2oWFN compiler tests"

	cmd="uml2owfn -i $DIR/TestSuite.xml -f lola -o"
	echo $cmd

	$cmd &> /dev/null
	result=$(($result | $?))

	is_filecount=$(ls -l $DIR/*.lola | wc -l)
	if [ ! $is_filecount -eq $must_filecount ] ; then
	  result=1
	  echo "[fail] not all processes have been translated, $is_filecount / $must_filecount"
	fi
	rm -f $DIR/*.lola


	cmd="uml2owfn -i $DIR/TestSuite.xml -p filter -f lola -o"
	echo $cmd

	$cmd &> /dev/null
	result=$(($result | $?))

	is_filecount=$(ls -l $DIR/*.lola | wc -l)
	if [ ! $is_filecount -eq $must_filecount_filter ] ; then
	  result=1
	  echo "[fail] incorrect number of processes has been translated, $is_filecount / $must_filecount_filter"
	fi
	rm -f $DIR/*.lola


	cmd="uml2owfn -i $DIR/TestSuite.xml -p filter -f dot -o"
	echo $cmd

	$cmd &> /dev/null
	result=$(($result | $?))

	is_filecount=$(ls -l $DIR/*.png | wc -l)
	if [ ! $is_filecount -eq $must_filecount_filter ] ; then
	  result=1
	  echo "[fail] incorrect number of processes has been translated, $is_filecount / $must_filecount_filter"
	fi
	rm -f $DIR/*.dot
	rm -f $DIR/*.png


	cmd="uml2owfn -i $DIR/TestSuite.xml -p filter -f tpn -o"
	echo $cmd

	$cmd &> /dev/null
	result=$(($result | $?))

	is_filecount=$(ls -l $DIR/*.tpn | wc -l)
	if [ ! $is_filecount -eq $must_filecount_filter ] ; then
	  result=1
	  echo "[fail] incorrect number of processes has been translated, $is_filecount / $must_filecount_filter"
	fi
	rm -f $DIR/*.tpn

	
	cmd="uml2owfn -i $DIR/TestSuite.xml -p filter -f owfn -o"
	echo $cmd

	$cmd &> /dev/null
	result=$(($result | $?))

	is_filecount=$(ls -l $DIR/*.owfn | wc -l)
	if [ ! $is_filecount -eq $must_filecount_filter ] ; then
	  result=1
	  echo "[fail] incorrect number of processes has been translated, $is_filecount / $must_filecount_filter"
	fi
	rm -f $DIR/*.owfn


	cmd="uml2owfn -i $DIR/TestSuite.xml -p filter -f pep -o"
	echo $cmd

	$cmd &> /dev/null
	result=$(($result | $?))

	is_filecount=$(ls -l $DIR/*.ll_net | wc -l)
	if [ ! $is_filecount -eq $must_filecount_filter ] ; then
	  result=1
	  echo "[fail] incorrect number of processes has been translated, $is_filecount / $must_filecount_filter"
	fi
	rm -f $DIR/*.ll_net


	cmd="uml2owfn -i $DIR/TestSuite.xml -p filter -f apnn -o"
	echo $cmd

	$cmd &> /dev/null
	result=$(($result | $?))

	is_filecount=$(ls -l $DIR/*.apnn | wc -l)
	if [ ! $is_filecount -eq $must_filecount_filter ] ; then
	  result=1
	  echo "[fail] incorrect number of processes has been translated, $is_filecount / $must_filecount_filter"
	fi
	rm -f $DIR/*.apnn

	
	cmd="uml2owfn -i $DIR/TestSuite.xml -p filter -f ina -o"
	echo $cmd

	$cmd &> /dev/null
	result=$(($result | $?))

	is_filecount=$(ls -l $DIR/*.pnt | wc -l)
	if [ ! $is_filecount -eq $must_filecount_filter ] ; then
	  result=1
	  echo "[fail] incorrect number of processes has been translated, $is_filecount / $must_filecount_filter"
	fi
	rm -f $DIR/*.pnt


	cmd="uml2owfn -i $DIR/TestSuite.xml -p filter -f spin -o"
	echo $cmd

	$cmd &> /dev/null
	result=$(($result | $?))

	is_filecount=$(ls -l $DIR/*.spin | wc -l)
	if [ ! $is_filecount -eq $must_filecount_filter ] ; then
	  result=1
	  echo "[fail] incorrect number of processes has been translated, $is_filecount / $must_filecount_filter"
	fi
	rm -f $DIR/*.spin


	cmd="uml2owfn -i $DIR/TestSuite.xml -p filter -f info -o"
	echo $cmd

	$cmd &> /dev/null
	result=$(($result | $?))

	is_filecount=$(ls -l $DIR/*.info | wc -l)
	if [ ! $is_filecount -eq $must_filecount_filter ] ; then
	  result=1
	  echo "[fail] incorrect number of processes has been translated, $is_filecount / $must_filecount_filter"
	fi
	rm -f $DIR/*.info

	
	cmd="uml2owfn -i $DIR/TestSuite.xml -p filter -f pnml -o"
	echo $cmd

	$cmd &> /dev/null
	result=$(($result | $?))

	is_filecount=$(ls -l $DIR/*.pnml | wc -l)
	if [ ! $is_filecount -eq $must_filecount_filter ] ; then
	  result=1
	  echo "[fail] incorrect number of processes has been translated, $is_filecount / $must_filecount_filter"
	fi
	rm -f $DIR/*.pnml


	cmd="uml2owfn -i $DIR/TestSuite.xml -p filter -p anon -f lola -o"
	echo $cmd

	$cmd &> /dev/null
	result=$(($result | $?))

	concrete_word_count=$(ls $DIR/*.lola | xargs grep "process" | wc -l)
	if [ ! $concrete_word_count -eq 0 ] ; then
	  result=1
	  echo "[fail] processes are not anonymized"
	fi
	rm -f $DIR/*.lola


	cmd="uml2owfn -i $DIR/TestSuite.xml -p filter -p log -f lola -o"
	echo $cmd

	$cmd &> /dev/null
	result=$(($result | $?))

	if [ ! -f $DIR/uml2owfn_TestSuite.log ] ; then
	  result=1
	  echo "[fail] no log file generated"
	fi
	rm -f $DIR/*.lola
	rm -f $DIR/uml2owfn_TestSuite.log


# remove files from builddir
#if [ $builddir_cleanup -eq 1 ] ; then
#  rm -f $builddir/compiler/TestSuite.xml
#  rmdir $builddir/compiler
#fi

exit $result
