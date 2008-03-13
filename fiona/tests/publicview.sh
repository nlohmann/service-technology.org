#!/bin/bash

############################################################################
# Copyright 2007 Peter Laufer                                              #
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

DIR=$testdir/publicview
FIONA=fiona

#loeschen aller erzeugten Dateien im letzten Durchlauf
rm -f $DIR/*.out
rm -f $DIR/*.png
rm -f $DIR/*.log

result=0

############################################################################
# 1st Fix
############################################################################

nodes_soll=4
edges_soll=4

og="$DIR/1stFix.og"
cmd="$FIONA $og -t PV"

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

    echo $OUTPUT | grep "nodes: $nodes_soll" > /dev/null
    nodes=$?

    echo $OUTPUT | grep "edges: $edges_soll" > /dev/null
    edges=$?

    if [ $nodes -ne 0 -o $edges -ne 0 ]
    then
    echo   ... failed to build PVSA correctly
    fi

    result=`expr $result + $nodes + $edges`
fi


############################################################################
# 2nd FixA
############################################################################

nodes_soll=5
edges_soll=6

og="$DIR/2ndFixA.og"
cmd="$FIONA $og -t PV"

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

    echo $OUTPUT | grep "nodes: $nodes_soll" > /dev/null
    nodes=$?

    echo $OUTPUT | grep "edges: $edges_soll" > /dev/null
    edges=$?

    if [ $nodes -ne 0 -o $edges -ne 0 ]
    then
    echo   ... failed to build PVSA correctly
    fi

    result=`expr $result + $nodes + $edges`
fi


############################################################################
# 2nd FixB
############################################################################

nodes_soll=4
edges_soll=4

og="$DIR/2ndFixB.og"
cmd="$FIONA $og -t PV"

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

    echo $OUTPUT | grep "nodes: $nodes_soll" > /dev/null
    nodes=$?

    echo $OUTPUT | grep "edges: $edges_soll" > /dev/null
    edges=$?

    if [ $nodes -ne 0 -o $edges -ne 0 ]
    then
    echo   ... failed to build PVSA correctly
    fi

    result=`expr $result + $nodes + $edges`
fi

############################################################################
# 3rd Fix
############################################################################

nodes_soll=6
edges_soll=5

og="$DIR/3rdFix.og"
cmd="$FIONA $og -t PV"

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

    echo $OUTPUT | grep "nodes: $nodes_soll" > /dev/null
    nodes=$?

    echo $OUTPUT | grep "edges: $edges_soll" > /dev/null
    edges=$?

    if [ $nodes -ne 0 -o $edges -ne 0 ]
    then
    echo   ... failed to build PVSA correctly
    fi

    result=`expr $result + $nodes + $edges`
fi


############################################################################
# 2nd FixB as owfn
############################################################################

nodes_soll=4
edges_soll=4

og="$DIR/2ndFixB.owfn"
cmd="$FIONA $og -t PV"

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

    echo $OUTPUT | grep "nodes: $nodes_soll" > /dev/null
    nodes=$?

    echo $OUTPUT | grep "edges: $edges_soll" > /dev/null
    edges=$?

    if [ $nodes -ne 0 -o $edges -ne 0 ]
    then
    echo   ... failed to build PVSA correctly
    fi

    result=`expr $result + $nodes + $edges`
fi

############################################################################
# 3rd Fix as owfn
############################################################################

nodes_soll=6
edges_soll=5

og="$DIR/3rdFix.owfn"
cmd="$FIONA $og -t PV"

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

    echo $OUTPUT | grep "nodes: $nodes_soll" > /dev/null
    nodes=$?

    echo $OUTPUT | grep "edges: $edges_soll" > /dev/null
    edges=$?

    if [ $nodes -ne 0 -o $edges -ne 0 ]
    then
    echo   ... failed to build PVSA correctly
    fi

    result=`expr $result + $nodes + $edges`
fi

############################################################################
# 1st Fix (--multipledeadlocks)
############################################################################

nodes_soll=4
edges_soll=4

og="$DIR/1stFix.og"
cmd="$FIONA $og -t PV --multipledeadlocks"

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

    echo $OUTPUT | grep "nodes: $nodes_soll" > /dev/null
    nodes=$?

    echo $OUTPUT | grep "edges: $edges_soll" > /dev/null
    edges=$?

    if [ $nodes -ne 0 -o $edges -ne 0 ]
    then
    echo   ... failed to build PVSA correctly
    fi

    result=`expr $result + $nodes + $edges`
fi


############################################################################
# 2nd FixA (--multipledeadlocks)
############################################################################

nodes_soll=5
edges_soll=6

og="$DIR/2ndFixA.og"
cmd="$FIONA $og -t PV --multipledeadlocks"

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

    echo $OUTPUT | grep "nodes: $nodes_soll" > /dev/null
    nodes=$?

    echo $OUTPUT | grep "edges: $edges_soll" > /dev/null
    edges=$?

    if [ $nodes -ne 0 -o $edges -ne 0 ]
    then
    echo   ... failed to build PVSA correctly
    fi

    result=`expr $result + $nodes + $edges`
fi


############################################################################

############################################################################
# complexPV equivalence
############################################################################

nodes_soll=46
edges_soll=151

service="$DIR/complexPV"
cmd="$FIONA $service.owfn -t PV"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$service.owfn.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`

    echo $OUTPUT | grep "nodes: $nodes_soll" > /dev/null
    nodes=$?

    echo $OUTPUT | grep "edges: $edges_soll" > /dev/null
    edges=$?

    if [ $nodes -ne 0 -o $edges -ne 0 ]
    then
    echo   ... failed to build PVSA correctly
    fi

    result=`expr $result + $nodes + $edges`
fi

cmd="$FIONA $service.PV.owfn $service.owfn.og -t equivalence"

if [ "$quiet" != "no" ]; then
    cmd="$cmd -Q"
fi

if [ "$memcheck" = "yes" ]; then
    memchecklog="$service.sa.owfn.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
    
    echo $OUTPUT | grep "are equivalent: YES" > /dev/null
    resultSIM=$?
    if [ $resultSIM -ne 0 ]; then
        let "result += 1"
        echo ... equivalence check of the service's and its public view's OGs failed.
    fi
fi



############################################################################

echo

exit $result
