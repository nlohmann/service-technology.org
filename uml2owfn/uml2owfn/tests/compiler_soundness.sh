#! /bin/bash

source defaults.sh

# copy files to builddir (in case it is a different directory)
#if test \( "$testdir" = "$builddir" \)
#then
#  builddir_cleanup=0
#else 
#  $MKDIR_P $builddir/soundness
#  cp $testdir/soundness/* $builddir/soundness
#  builddir_cleanup=1
#fi

result=0
DIR=$testdir/soundness
FILE=TestSuite_soundness

echo ""
echo "UML2oWFN compiler for soundness analysis tests"
echo ""
echo "[strong termination and safeness, LoLA, taskfiles]"

	cmd="uml2owfn -i $DIR/$FILE.xml -p filter -a soundness -a safe -p taskfile -f lola --output=$FILE"
	echo $cmd

	$cmd &> /dev/null
	result=$(($result | $?))
	
	if [ ! -f $builddir/TestSuite_soundness.BOMcncpt1__ProcessPinDisjoint_sound_ifStop.lola.fin.task -o ! -f $builddir/TestSuite_soundness.BOMcncpt1__ProcessPinDisjoint_sound_ifStop.lola.safe.task ] ; then
	  result=1
	  echo "[fail] task files not generated"
	fi
	rm $builddir/*.task
	rm $builddir/*.lola
	rm $builddir/check_$FILE.sh
	
	
echo ""
echo "[strong termination, LoLA, no taskfiles]"

	cmd="uml2owfn -i $DIR/$FILE.xml -p filter -a soundness -f lola --output=$FILE"
	echo $cmd
	
	$cmd &> /dev/null
	result=$(($result | $?))
	
	if [ -f $builddir/TestSuite_soundness.BOMcncpt1__ProcessPinDisjoint_sound_ifStop.lola.fin.task -o -f $builddir/TestSuite_soundness.BOMcncpt1__ProcessPinDisjoint_sound_ifStop.lola.safe.task ] ; then
	  result=1
	  echo "[fail] task files generated"
	fi
	rm $builddir/*.lola
	rm $builddir/check_$FILE.sh


echo ""
echo "[strong termination and safeness, LoLA, taskfiles, orJoin termination semantics]"

	cmd="uml2owfn -i $DIR/$FILE.xml -p filter -a soundness -a safe -a orJoin -p taskfile -f lola --output=$FILE"
	echo $cmd
	
	$cmd &> /dev/null
	result=$(($result | $?))

	rm $builddir/*.task
	rm $builddir/*.lola
	rm $builddir/check_$FILE.sh


echo ""
echo "[strong termination and safeness, LoLA, taskfiles, weak data-flow termination semantics]"
	
	cmd="uml2owfn -i $DIR/$FILE.xml -p filter -a soundness -a safe -a removePinsets -p taskfile -f lola --output=$FILE"
	echo $cmd
	
	$cmd &> /dev/null
	result=$(($result | $?))

	rm $builddir/*.task
	rm $builddir/*.lola
	rm $builddir/check_$FILE.sh


echo ""
echo "[Woflan (TPN)]"
	
	cmd="uml2owfn -i $DIR/$FILE.xml -p filter -a soundness -a wfNet -f tpn --output=$FILE"
	echo $cmd
	
	$cmd &> /dev/null
	result=$(($result | $?))
	
	rm $builddir/*.tpn


echo ""
echo "[CTL model checking]"
	
	cmd="uml2owfn -i $DIR/$FILE.xml -p filter -a soundness -p ctl -f lola --output=$FILE"
	echo $cmd
	
	$cmd &> /dev/null
	result=$(($result | $?))

	must_filecount_filter=12
	agef_count=$(ls $builddir/$FILE.*.lola | xargs grep "ALLPATH ALWAYS ( EXPATH EVENTUALLY (" | wc -l)
	if [ ! $agef_count -eq $must_filecount_filter ] ; then
	  result=1
	  echo "[fail] CTL properties not generated for all processes"
	fi
	lola_mc_count=$(grep "lola-mc" $builddir/check_$FILE.sh | wc -l)
	if [ ! $lola_mc_count -eq $must_filecount_filter ] ; then
	  result=1
	  echo "[fail] not calling CTL model checker for CTL analysis"
	fi
	
	rm -f $builddir/*.task       # task files are optional
	rm $builddir/*.lola
	rm $builddir/check_$FILE.sh


# remove files from builddir
#if [ $builddir_cleanup -eq 1 ] ; then
#  rm -f $builddir/soundness/TestSuite_soundness.txt
#  rm -f $builddir/soundness/TestSuite_soundness.xml
#  rmdir $builddir/soundness
#fi


exit $result
