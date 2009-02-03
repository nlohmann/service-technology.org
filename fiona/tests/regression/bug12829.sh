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

cd $testdir/bug12829

result=0

TEMPFILE1=`mktemp`
TEMPFILE2=`mktemp`
TEMPFILE3=`mktemp`
TEMPFILE4=`mktemp`

echo "TEST: bug12829.sh"

fiona -t ig -Q b1.s00000035__s00000934.owfn &> $TEMPFILE1
fiona -t ig -Q b1-1.s00000035__s00000934.owfn &> $TEMPFILE2

fiona -t ig -Q -p cre b1.s00000035__s00000934.owfn &> $TEMPFILE3
fiona -t ig -Q -p rbs b1-1.s00000035__s00000934.owfn &> $TEMPFILE4

grep "net is controllable: NO" $TEMPFILE1 > /dev/null
let result=$result+$?
grep "net is controllable: NO" $TEMPFILE2 > /dev/null
let result=$result+$?
grep "net is controllable: NO" $TEMPFILE3 > /dev/null
let result=$result+$?
grep "net is controllable: NO" $TEMPFILE4 > /dev/null
let result=$result+$?

exit $result
