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

result=0

fiona -t ig -Q -m4 cabbababb.owfn &> result-1.tmp
fiona -t og -Q -m4 cabbababb.owfn &> result-2.tmp

grep "net is controllable: NO" result-1.tmp > /dev/null
let result=$result+$?
grep "net is controllable: NO" result-2.tmp > /dev/null
let result=$result+$?

exit $result
