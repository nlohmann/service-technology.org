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

cd $testdir/bug12765

result=0

TEMPFILE1=`mktemp`
TEMPFILE2=`mktemp`

echo "TEST: bug12765.sh"

fiona -t ig -Q -m4 cabbababb.owfn &> $TEMPFILE1
fiona -t og -Q -m4 cabbababb.owfn &> $TEMPFILE2

grep "net is controllable: NO" $TEMPFILE1 > /dev/null
let result=$result+$?
grep "net is controllable: NO" $TEMPFILE2 > /dev/null
let result=$result+$?

exit $result
