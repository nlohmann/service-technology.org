#!/usr/bin/env bash

#############################################################################
# BUG #12101 (https://gna.org/bugs/?12101)
#
# DESCRIPTION
#   If a a file cannot be created by Fiona, no error is displayed.
#
# TEST
#   Create a file "myCoffee.ig.og" without writing permissions and then run
#   Fiona to create "myCoffee.ig.og". Check if Fiona returns error code 4.
#############################################################################

result=0

touch myCoffee.owfn.ig.og
chmod a-w myCoffee.owfn.ig.og

fiona myCoffee.owfn -p no-dot &> /dev/null
let result=$?-4

chmod a+w myCoffee.owfn.ig.og

exit $result
