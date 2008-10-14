#! /bin/bash

echo ""

echo "UML2oWFN parser tests"
echo "uml2owfn -i $testdir/compiler/TestSuite.xml"
uml2owfn -i $testdir/compiler/TestSuite.xml &> /dev/null
result1=$?

echo "uml2owfn -i $testdir/soundness/TestSuite_soundness.xml"
uml2owfn -i $testdir/soundness/TestSuite_soundness.xml &> /dev/null
result2=$?

if test \( $result1 -eq 0 -a $result2 -eq 0 \)
then
  result=0
else
  result=1
fi

exit $result
