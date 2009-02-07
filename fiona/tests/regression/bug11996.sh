#!/usr/bin/env bash

#############################################################################
# BUG #11996 (https://gna.org/bugs/?11996)
#
# DESCRIPTION
#   Product OG has no final states.
#
# TEST
#   Create product OG and search for "final" in .og file.
#############################################################################

fiona a.owfn.og b.owfn.og -t productog -pno-png &> /dev/null
grep "final" a.owfn_X_b.owfn.og &> /dev/null

exit $?
