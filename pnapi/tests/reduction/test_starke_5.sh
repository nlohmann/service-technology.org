#!/usr/bin/env bash

NET=test_starke_5.net.owfn
RED=starke5

petri -v ${NET} &> net.stat
petri -r${RED} ${NET} -oowfn &> /dev/null
petri -v ${NET}.reduced.owfn &> reduced.stat

$GREP -q "|P|= 4  |P_in|= 0  |P_out|= 0  |T|= 4  |F|= 9" net.stat
VAL1=$?

$GREP -q "|P|= 3  |P_in|= 0  |P_out|= 0  |T|= 3  |F|= 7" reduced.stat
VAL2=$?

if [ `expr ${VAL1} + ${VAL2}` != 0 ] 
  then exit 1
fi

exit 0

