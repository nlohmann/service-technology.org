#! /bin/bash

must_filecount=24
must_filecount_filter=20

echo ""

echo "UML2oWFN compiler tests"
echo "uml2owfn -i $testdir/compiler/TestSuite.xml -f lola -o"
uml2owfn -i $testdir/compiler/TestSuite.xml -f lola -o &> /dev/null
result1=$?

is_filecount=$(ls -l $testdir/compiler/*.lola | wc -l)
if [ ! $is_filecount -eq $must_filecount ] ; then
  result1=1
  echo "[fail] not all processes have been translated, $is_filecount / $must_filecount"
fi
rm -f $testdir/compiler/*.lola

echo "uml2owfn -i $testdir/compiler/TestSuite.xml -p filter -f lola -o"
uml2owfn -i $testdir/compiler/TestSuite.xml -p filter -f lola -o &> /dev/null
result2=$?

is_filecount=$(ls -l $testdir/compiler/*.lola | wc -l)
if [ ! $is_filecount -eq $must_filecount_filter ] ; then
  result2=1
  echo "[fail] incorrect number of processes has been translated, $is_filecount / $must_filecount_filter"
fi
rm -f $testdir/compiler/*.lola

echo "uml2owfn -i $testdir/compiler/TestSuite.xml -p filter -f dot -o"
uml2owfn -i $testdir/compiler/TestSuite.xml -p filter -f dot -o &> /dev/null
result3=$?


is_filecount=$(ls -l $testdir/compiler/*.png | wc -l)
if [ ! $is_filecount -eq $must_filecount_filter ] ; then
  result3=1
  echo "[fail] incorrect number of processes has been translated, $is_filecount / $must_filecount_filter"
fi
rm -f $testdir/compiler/*.dot
rm -f $testdir/compiler/*.png

echo "uml2owfn -i $testdir/compiler/TestSuite.xml -p filter -f tpn -o"
uml2owfn -i $testdir/compiler/TestSuite.xml -p filter -f tpn -o &> /dev/null
result4=$?


is_filecount=$(ls -l $testdir/compiler/*.tpn | wc -l)
if [ ! $is_filecount -eq $must_filecount_filter ] ; then
  result4=1
  echo "[fail] incorrect number of processes has been translated, $is_filecount / $must_filecount_filter"
fi
rm -f $testdir/compiler/*.tpn

echo "uml2owfn -i $testdir/compiler/TestSuite.xml -p filter -f owfn -o"
uml2owfn -i $testdir/compiler/TestSuite.xml -p filter -f owfn -o &> /dev/null
result5=$?


is_filecount=$(ls -l $testdir/compiler/*.owfn | wc -l)
if [ ! $is_filecount -eq $must_filecount_filter ] ; then
  result5=1
  echo "[fail] incorrect number of processes has been translated, $is_filecount / $must_filecount_filter"
fi
rm -f $testdir/compiler/*.owfn

echo "uml2owfn -i $testdir/compiler/TestSuite.xml -p filter -f pep -o"
uml2owfn -i $testdir/compiler/TestSuite.xml -p filter -f pep -o &> /dev/null
result6=$?


is_filecount=$(ls -l $testdir/compiler/*.ll_net | wc -l)
if [ ! $is_filecount -eq $must_filecount_filter ] ; then
  result6=1
  echo "[fail] incorrect number of processes has been translated, $is_filecount / $must_filecount_filter"
fi
rm -f $testdir/compiler/*.ll_net

echo "uml2owfn -i $testdir/compiler/TestSuite.xml -p filter -f apnn -o"
uml2owfn -i $testdir/compiler/TestSuite.xml -p filter -f apnn -o &> /dev/null
result7=$?


is_filecount=$(ls -l $testdir/compiler/*.apnn | wc -l)
if [ ! $is_filecount -eq $must_filecount_filter ] ; then
  result7=1
  echo "[fail] incorrect number of processes has been translated, $is_filecount / $must_filecount_filter"
fi
rm -f $testdir/compiler/*.apnn

echo "uml2owfn -i $testdir/compiler/TestSuite.xml -p filter -f ina -o"
uml2owfn -i $testdir/compiler/TestSuite.xml -p filter -f ina -o &> /dev/null
result8=$?


is_filecount=$(ls -l $testdir/compiler/*.pnt | wc -l)
if [ ! $is_filecount -eq $must_filecount_filter ] ; then
  result8=1
  echo "[fail] incorrect number of processes has been translated, $is_filecount / $must_filecount_filter"
fi
rm -f $testdir/compiler/*.pnt

echo "uml2owfn -i $testdir/compiler/TestSuite.xml -p filter -f spin -o"
uml2owfn -i $testdir/compiler/TestSuite.xml -p filter -f spin -o &> /dev/null
result9=$?


is_filecount=$(ls -l $testdir/compiler/*.spin | wc -l)
if [ ! $is_filecount -eq $must_filecount_filter ] ; then
  result9=1
  echo "[fail] incorrect number of processes has been translated, $is_filecount / $must_filecount_filter"
fi
rm -f $testdir/compiler/*.spin

echo "uml2owfn -i $testdir/compiler/TestSuite.xml -p filter -f info -o"
uml2owfn -i $testdir/compiler/TestSuite.xml -p filter -f info -o &> /dev/null
result10=$?


is_filecount=$(ls -l $testdir/compiler/*.info | wc -l)
if [ ! $is_filecount -eq $must_filecount_filter ] ; then
  result10=1
  echo "[fail] incorrect number of processes has been translated, $is_filecount / $must_filecount_filter"
fi
rm -f $testdir/compiler/*.info

echo "uml2owfn -i $testdir/compiler/TestSuite.xml -p filter -f pnml -o"
uml2owfn -i $testdir/compiler/TestSuite.xml -p filter -f pnml -o &> /dev/null
result11=$?


is_filecount=$(ls -l $testdir/compiler/*.pnml | wc -l)
if [ ! $is_filecount -eq $must_filecount_filter ] ; then
  result11=1
  echo "[fail] incorrect number of processes has been translated, $is_filecount / $must_filecount_filter"
fi
rm -f $testdir/compiler/*.pnml

echo "uml2owfn -i $testdir/compiler/TestSuite.xml -p filter -p anon -f lola -o"
uml2owfn -i $testdir/compiler/TestSuite.xml -p filter -p anon -f lola -o &> /dev/null
result12=$?

concrete_word_count=$(ls $testdir/compiler/*.lola | xargs grep "process" | wc -l)
if [ ! $concrete_word_count -eq 0 ] ; then
  result12=1
  echo "[fail] processes are not anonymized"
fi
rm -f $testdir/compiler/*.lola

echo "uml2owfn -i $testdir/compiler/TestSuite.xml -p filter -p log -f lola -o"
uml2owfn -i $testdir/compiler/TestSuite.xml -p filter -p log -f lola -o &> /dev/null
result13=$?
if [ ! -f $testdir/soundness/uml2owfn_TestSuite.log ] ; then
  result13=1
  echo "[fail] no log file generated"
fi
rm -f $testdir/compiler/*.lola
rm -f $testdir/compiler/uml2owfn_TestSuite.log

if test \( $result1 -eq 0 -a $result2 -eq 0 -a $result3 -eq 0 -a $result4 -eq 0 -a $result5 -eq 0 -a $result6 -eq 0 -a $result7 -eq 0 -a $result8 -eq 0 -a $result9 -eq 0 -a $result10 -eq 0 -a $result11 -eq 0 -a $result12 -eq 0 -a $result13 -eq 0 \)
then
  result=0
else
  result=1
fi

exit $result
