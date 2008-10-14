#! /bin/bash

echo ""
echo "UML2oWFN command-line help tests"
echo "test: version output"
echo "      uml2owfn --version"
uml2owfn --version &> /dev/null
result1=$?

echo "test: help output"
echo "      uml2owfn --help"
uml2owfn --help &> /dev/null
result2=$?


if [ $result1 -eq 0 -a $result2 -eq 0 ]
then
  result=0
else
  result=1
fi

exit $result
