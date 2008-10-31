#! /bin/bash

result=0

echo ""
echo "UML2oWFN command-line help tests"

	cmd="uml2owfn --version"
	echo $cmd

	$cmd &> /dev/null
	result=$(($result | $?))


	cmd="uml2owfn --help"
	echo $cmd

	$cmd &> /dev/null
	result=$(($result | $?))


exit $result
