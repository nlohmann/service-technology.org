#!/usr/bin/env bash

#############################################################################
# BUG #12895 (https://gna.org/bugs/?12895)
#
# DESCRIPTION
#   The OG of the public view of the net is not equivalent to the net's OG.
#
# TEST
#   Calculate public view of the net and then check equivalence of the net
#   and the public view.
#############################################################################

cd bug12895

echo "TEST: bug12895.sh"

fiona non_empty_true_node3.owfn -t pv -p no-dot &> /dev/null
fiona -t equvialence non_empty_true_node3.owfn non_empty_true_node3.pv.owfn -Q  &> result.txt

grep "are equivalent: YES" result.txt > /dev/null

exit $?
