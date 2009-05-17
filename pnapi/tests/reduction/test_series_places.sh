#!/usr/bin/env bash

NET=test_series_places.net.owfn
RED=series_places

petri -v ${NET} &> net.stat
petri -r${RED} ${NET} -oowfn &> /dev/null
petri -v ${NET}.reduced.owfn &> reduced.stat

$GREP -q "|P|= 5  |P_in|= 0  |P_out|= 0  |T|= 7  |F|= 11" net.stat
VAL1=$?

$GREP -q "|P|= 4  |P_in|= 0  |P_out|= 0  |T|= 6  |F|= 9" reduced.stat
VAL2=$?

if [ `expr ${VAL1} + ${VAL2}` != 0 ] 
  then exit 1
fi

exit 0

