#!/bin/bash

# Script for testing the implementation of SList.


source defaults.sh
source memcheck_helper.sh

echo
echo ---------------------------------------------------------------------
echo running $0
echo

result=0

############################################################################

cmd=$builddir/slistcheck

if [ "$memcheck" = "yes" ]; then
    memchecklog="$cmd.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    $cmd
    result=$?
fi

echo
exit $result
