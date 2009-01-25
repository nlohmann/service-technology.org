#!/usr/bin/env bash

#############################################################################
# BUG #12765 (https://gna.org/bugs/?12765)
#
# DESCRIPTION
#   The net is uncontrollable. The OG detects this, but the IG claims that
#   the net is controllable. Note: the message bound for this net is 4.
#
# TEST
#   Check controllability in both modes and check if both detect the
#   uncontrollability.
#############################################################################

cd bug12765

result=0

echo "TEST: bug12765.sh"

fiona -t ig -Q -m4 cabbababb.owfn &> result1.txt
fiona -t og -Q -m4 cabbababb.owfn &> result2.txt

grep "net is controllable: NO" result1.txt > /dev/null
let result=$result+$?
grep "net is controllable: NO" result2.txt > /dev/null
let result=$result+$?

exit $result
