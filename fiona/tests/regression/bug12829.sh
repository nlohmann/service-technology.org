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

result=0

fiona -t ig -Q b1.s00000035__s00000934.owfn &> result-1.tmp
fiona -t ig -Q b1-1.s00000035__s00000934.owfn &> result-2.tmp

fiona -t ig -Q -p cre b1.s00000035__s00000934.owfn &> result-3.tmp
fiona -t ig -Q -p rbs b1-1.s00000035__s00000934.owfn &> result-4.tmp

grep "net is controllable: NO" result-1.tmp > /dev/null
let result=$result+$?
grep "net is controllable: NO" result-2.tmp > /dev/null
let result=$result+$?
grep "net is controllable: NO" result-3.tmp > /dev/null
let result=$result+$?
grep "net is controllable: NO" result-4.tmp > /dev/null
let result=$result+$?

exit $result
