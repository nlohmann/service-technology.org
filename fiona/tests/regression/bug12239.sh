#!/usr/bin/env bash

#############################################################################
# BUG #12239 (https://gna.org/bugs/?12239)
#
# DESCRIPTION
#   The net is already normalized. If it is normalized again, unnecessary
#   nodes are added.
#
# TEST
#   Normalize net and compare size with expected size.
#############################################################################

fiona coffee_extended-partner.normalized.owfn -t normalize -Q &> result.tmp

grep "|P|=8, |P_in|= 2, |P_out|= 1, |T|=5, |F|=12" result.tmp > /dev/null

exit $?
