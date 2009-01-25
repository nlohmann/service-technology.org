#!/usr/bin/env bash

#############################################################################
# BUG #12829 (https://gna.org/bugs/?12829)
#
# DESCRIPTION
#   I used Fiona (version 3693, main branch) to compute the IG of two
#   processes. Computing the IG without any reduction (only option -t ig),
#   both processes are controllable. By applying any of the reduction rules
#   "-p cre" or "-p rbs" the processes are uncontrollable.
#
# TEST
#   Check controllability of both nets, then again with reduction rules,
#   and then check whether all 4 results are positive.
#############################################################################

cd bug12829

result=0

echo "TEST: bug12829.sh"

fiona -t ig -Q b1.s00000035__s00000934.owfn &> result1.txt
fiona -t ig -Q b1-1.s00000035__s00000934.owfn &> result2.txt

fiona -t ig -Q -p cre b1.s00000035__s00000934.owfn &> result3.txt
fiona -t ig -Q -p rbs b1-1.s00000035__s00000934.owfn &> result4.txt

grep "net is controllable: YES" result1.txt > /dev/null
let result=$result+$?
grep "net is controllable: YES" result2.txt > /dev/null
let result=$result+$?
grep "net is controllable: YES" result3.txt > /dev/null
let result=$result+$?
grep "net is controllable: YES" result4.txt > /dev/null
let result=$result+$?

exit $result
