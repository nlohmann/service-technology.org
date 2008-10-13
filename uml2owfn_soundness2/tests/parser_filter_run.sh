#! /bin/bash

echo ""

echo "UML2oWFN parser filter tests"
echo "uml2owfn -i $testdir/compiler/TestSuite.xml -p filter"
uml2owfn -i $testdir/compiler/TestSuite.xml -p filter &> /dev/null
result1=$?

echo "uml2owfn -i $testdir/soundness/TestSuite_soundness.xml -p filter"
uml2owfn -i $testdir/soundness/TestSuite_soundness.xml -p filter &> /dev/null
result2=$?

if test \( $result1 -eq 0 -a $result2 -eq 0 \)
then
  result=0
else
  result=1
fi

exit $result
