#!/usr/bin/env bash

NET=test_starke_4.net.owfn
RED=starke4

petri -v ${NET} &> net.stat
petri -r${RED} ${NET} -oowfn &> /dev/null
petri -v ${NET}.reduced.owfn &> reduced.stat

$GREP -q "|P|= 4  |P_in|= 0  |P_out|= 0  |T|= 4  |F|= 8" net.stat
VAL1=$?

$GREP -q "|P|= 3  |P_in|= 0  |P_out|= 0  |T|= 3  |F|= 6" reduced.stat
VAL2=$?

if [ `expr ${VAL1} + ${VAL2}` != 0 ] 
  then exit 1
fi

exit 0

