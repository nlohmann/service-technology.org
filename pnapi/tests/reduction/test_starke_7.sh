#!/usr/bin/env bash

NET=test_starke_7.net.owfn
RED=starke7

petri -v ${NET} &> net.stat
petri -r${RED} ${NET} -oowfn &> /dev/null
petri -v ${NET}.reduced.owfn &> reduced.stat

$GREP -q "|P|= 3  |P_in|= 0  |P_out|= 0  |T|= 2  |F|= 6" net.stat
VAL1=$?

$GREP -q "|P|= 2  |P_in|= 0  |P_out|= 0  |T|= 1  |F|= 2" reduced.stat
VAL2=$?

if [ `expr ${VAL1} + ${VAL2}` != 0 ] 
  then exit 1
fi

exit 0

