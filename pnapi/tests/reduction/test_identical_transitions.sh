#!/usr/bin/env bash

NET=test_identical_transitions.net.owfn
RED=identical_transitions

petri -v ${NET} &> net.stat
petri -r${RED} ${NET} -oowfn &> /dev/null
petri -v ${NET}.reduced.owfn &> reduced.stat

$GREP -q "|P|= 6  |P_in|= 0  |P_out|= 0  |T|= 8  |F|= 18" net.stat
VAL1=$?

$GREP -q "|P|= 6  |P_in|= 0  |P_out|= 0  |T|= 7  |F|= 12" reduced.stat
VAL2=$?

if [ `expr ${VAL1} + ${VAL2}` != 0 ] 
  then exit 1
fi

exit 0

