#!/bin/bash

############################################################################
# Copyright 2007 Peter Laufer                                              #
# Copyright 2008 Peter Massuthe                                            #
#                                                                          #
#                                                                          #
# This file is part of Fiona.                                              #
#                                                                          #
# Fiona is free software; you can redistribute it and/or modify it         #
# under the terms of the GNU General Public License as published by the    #
# Free Software Foundation; either version 2 of the License, or (at your   #
# option) any later version.                                               #
#                                                                          #
# Fiona is distributed in the hope that it will be useful, but WITHOUT     #
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or    #
# FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for #
# more details.                                                            #
#                                                                          #
# You should have received a copy of the GNU General Public License along  #
# with Fiona; if not, write to the Free Software Foundation, Inc., 51      #
# Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.                     #
############################################################################

source defaults.sh
source memcheck_helper.sh

echo
echo ---------------------------------------------------------------------
echo running $0
echo

DIR=$testdir/oWFNreduction
FIONA=fiona

result=0

############################################################################
# misc Coffee reduction
############################################################################

nodes_soll=53
transitions_soll=62

owfn="$DIR/060116-misc-coffee.owfn"
cmd="$FIONA $owfn -t reduce"

if [ "$quiet" != "no" ]; then
    cmd="$cmd -Q"
fi

if [ "$memcheck" = "yes" ]; then
    memchecklog="$og.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`

    echo $OUTPUT | grep "|P|=$nodes_soll" > /dev/null
    nodes=$?

    echo $OUTPUT | grep "|T|=$transitions_soll" > /dev/null
    edges=$?

    if [ $nodes -ne 0 -o $edges -ne 0 ]
    then
    echo   ... failed to reduce the oWFN correctly
    fi

    result=`expr $result + $nodes + $edges`
fi

############################################################################
# phcontrol10 reduction
############################################################################

nodes_soll=61
transitions_soll=21

owfn="$DIR/phcontrol10.unf.owfn"
cmd="$FIONA $owfn -t reduce"

if [ "$quiet" != "no" ]; then
    cmd="$cmd -Q"
fi

if [ "$memcheck" = "yes" ]; then
    memchecklog="$og.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`

    echo $OUTPUT | grep "|P|=$nodes_soll" > /dev/null
    nodes=$?

    echo $OUTPUT | grep "|T|=$transitions_soll" > /dev/null
    edges=$?

    if [ $nodes -ne 0 -o $edges -ne 0 ]
    then
    echo   ... failed to reduce the oWFN correctly
    fi

    result=`expr $result + $nodes + $edges`
fi

############################################################################

echo

exit $result
