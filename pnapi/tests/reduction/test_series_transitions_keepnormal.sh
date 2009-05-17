#!/usr/bin/env bash

NET=test_series_transitions_keepnormal.net.owfn
RED=series_transitions

petri -v ${NET} &> net.stat
petri -r${RED} ${NET} -oowfn &> /dev/null
petri -v ${NET}.reduced.owfn &> reduced1.stat

RED+=",keep_normal"
petri -r${RED} ${NET} -oowfn &> /dev/null
petri -v ${NET}.reduced.owfn &> reduced2.stat

$GREP -q "|P|= 5  |P_in|= 0  |P_out|= 0  |T|= 2  |F|= 6" net.stat
VAL1=$?

$GREP -q "|P|= 4  |P_in|= 0  |P_out|= 0  |T|= 1  |F|= 4" reduced1.stat
VAL2=$?

$GREP -q "|P|= 5  |P_in|= 0  |P_out|= 0  |T|= 2  |F|= 6" reduced2.stat
VAL3=$?

if [ `expr ${VAL1} + ${VAL2} + ${VAL3}` != 0 ] 
  then exit 1
fi

exit 0

