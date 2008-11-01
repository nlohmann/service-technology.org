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
FILE=TestSuite

must_filecount=24
must_filecount_filter=20

echo ""
echo "UML2oWFN compiler tests"

	cmd="uml2owfn -i $DIR/$FILE.xml -f lola --output=$FILE"
	echo $cmd

	$cmd &> /dev/null
	result=$(($result | $?))

	is_filecount=$(ls -l $builddir/*.lola | wc -l)
	if [ ! $is_filecount -eq $must_filecount ] ; then
	  result=1
	  echo "[fail] not all processes have been translated, $is_filecount / $must_filecount"
	fi
	rm $builddir/*.lola


	cmd="uml2owfn -i $DIR/$FILE.xml -p filter -f lola --output=$FILE"
	echo $cmd

	$cmd &> /dev/null
	result=$(($result | $?))

	is_filecount=$(ls -l $builddir/*.lola | wc -l)
	if [ ! $is_filecount -eq $must_filecount_filter ] ; then
	  result=1
	  echo "[fail] incorrect number of processes has been translated, $is_filecount / $must_filecount_filter"
	fi
	rm $builddir/*.lola


	cmd="uml2owfn -i $DIR/$FILE.xml -p filter -f dot --output=$FILE"
	echo $cmd

	$cmd &> /dev/null
	result=$(($result | $?))

	is_filecount=$(ls -l $builddir/*.png | wc -l)
	if [ ! $is_filecount -eq $must_filecount_filter ] ; then
	  result=1
	  echo "[fail] incorrect number of processes has been translated, $is_filecount / $must_filecount_filter"
	fi
	rm $builddir/*.dot
	rm $builddir/*.png


	cmd="uml2owfn -i $DIR/$FILE.xml -p filter -f tpn --output=$FILE"
	echo $cmd

	$cmd &> /dev/null
	result=$(($result | $?))

	is_filecount=$(ls -l $builddir/*.tpn | wc -l)
	if [ ! $is_filecount -eq $must_filecount_filter ] ; then
	  result=1
	  echo "[fail] incorrect number of processes has been translated, $is_filecount / $must_filecount_filter"
	fi
	rm $builddir/*.tpn

	
	cmd="uml2owfn -i $DIR/$FILE.xml -p filter -f owfn --output=$FILE"
	echo $cmd

	$cmd &> /dev/null
	result=$(($result | $?))

	is_filecount=$(ls -l $builddir/*.owfn | wc -l)
	if [ ! $is_filecount -eq $must_filecount_filter ] ; then
	  result=1
	  echo "[fail] incorrect number of processes has been translated, $is_filecount / $must_filecount_filter"
	fi
	rm $builddir/*.owfn


	cmd="uml2owfn -i $DIR/$FILE.xml -p filter -f pep --output=$FILE"
	echo $cmd

	$cmd &> /dev/null
	result=$(($result | $?))

	is_filecount=$(ls -l $builddir/*.ll_net | wc -l)
	if [ ! $is_filecount -eq $must_filecount_filter ] ; then
	  result=1
	  echo "[fail] incorrect number of processes has been translated, $is_filecount / $must_filecount_filter"
	fi
	rm $builddir/*.ll_net


	cmd="uml2owfn -i $DIR/$FILE.xml -p filter -f apnn --output=$FILE"
	echo $cmd

	$cmd &> /dev/null
	result=$(($result | $?))

	is_filecount=$(ls -l $builddir/*.apnn | wc -l)
	if [ ! $is_filecount -eq $must_filecount_filter ] ; then
	  result=1
	  echo "[fail] incorrect number of processes has been translated, $is_filecount / $must_filecount_filter"
	fi
	rm $builddir/*.apnn

	
	cmd="uml2owfn -i $DIR/$FILE.xml -p filter -f ina --output=$FILE"
	echo $cmd

	$cmd &> /dev/null
	result=$(($result | $?))

	is_filecount=$(ls -l $builddir/*.pnt | wc -l)
	if [ ! $is_filecount -eq $must_filecount_filter ] ; then
	  result=1
	  echo "[fail] incorrect number of processes has been translated, $is_filecount / $must_filecount_filter"
	fi
	rm $builddir/*.pnt


	cmd="uml2owfn -i $DIR/$FILE.xml -p filter -f spin --output=$FILE"
	echo $cmd

	$cmd &> /dev/null
	result=$(($result | $?))

	is_filecount=$(ls -l $builddir/*.spin | wc -l)
	if [ ! $is_filecount -eq $must_filecount_filter ] ; then
	  result=1
	  echo "[fail] incorrect number of processes has been translated, $is_filecount / $must_filecount_filter"
	fi
	rm $builddir/*.spin


	cmd="uml2owfn -i $DIR/$FILE.xml -p filter -f info --output=$FILE"
	echo $cmd

	$cmd &> /dev/null
	result=$(($result | $?))

	is_filecount=$(ls -l $builddir/*.info | wc -l)
	if [ ! $is_filecount -eq $must_filecount_filter ] ; then
	  result=1
	  echo "[fail] incorrect number of processes has been translated, $is_filecount / $must_filecount_filter"
	fi
	rm $builddir/*.info

	
	cmd="uml2owfn -i $DIR/$FILE.xml -p filter -f pnml --output=$FILE"
	echo $cmd

	$cmd &> /dev/null
	result=$(($result | $?))

	is_filecount=$(ls -l $builddir/*.pnml | wc -l)
	if [ ! $is_filecount -eq $must_filecount_filter ] ; then
	  result=1
	  echo "[fail] incorrect number of processes has been translated, $is_filecount / $must_filecount_filter"
	fi
	rm $builddir/*.pnml


	cmd="uml2owfn -i $DIR/$FILE.xml -p filter -p anon -f lola --output=$FILE"
	echo $cmd

	$cmd &> /dev/null
	result=$(($result | $?))

	concrete_word_count=$(ls $builddir/*.lola | xargs grep "process" | wc -l)
	if [ ! $concrete_word_count -eq 0 ] ; then
	  result=1
	  echo "[fail] processes are not anonymized"
	fi
	rm $builddir/*.lola


	cmd="uml2owfn -i $DIR/$FILE.xml -p filter -p log -f lola --output=$FILE"
	echo $cmd

	$cmd &> /dev/null
	result=$(($result | $?))

	rm $builddir/*.lola
	rm $builddir/uml2owfn_$FILE.log


# remove files from builddir
#if [ $builddir_cleanup -eq 1 ] ; then
#  rm -f $builddir/compiler/TestSuite.xml
#  rmdir $builddir/compiler
#fi

exit $result
