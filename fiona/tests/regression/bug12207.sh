#!/usr/bin/env bash

#############################################################################
# BUG #12207 (https://gna.org/bugs/?12207)
#
# DESCRIPTION
#   Reduced IGs could not be parsed by Fiona.
#
# TEST
#   Parse OG with old syntax and expect an error. Then parse an OG with
#   new syntax and expect success.
#############################################################################

cd $testdir/bug12207

result=0

echo "TEST: bug12207.sh"

fiona coffee_extended.owfn.ig.og -tpng -pno-png &> /dev/null
let result=$result+$?-1

fiona coffee_extended.fixed.owfn.ig.og -tpng -pno-png &> /dev/null
let result=$result+$?

exit $result
