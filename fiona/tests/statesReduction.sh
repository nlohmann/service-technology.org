#!/bin/bash

############################################################################
# Copyright 2005, 2006 Peter Massuthe, Daniela Weinberg, Dennis Reinert,   #
#                      Jan Bretschneider and Christian Gierds              #
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

DIR=$testdir/statesReduction
FIONA=fiona

#loeschen aller erzeugten Dateien im letzten Durchlauf
rm -f $DIR/*.out
rm -f $DIR/*.OG.png
rm -f $DIR/*.IG.png
rm -f $DIR/*.og
rm -f $DIR/*.log

result=0

############################################################################
# reduced IG with node reduction
############################################################################

# Don't run this test, because -R is buggy.

shop3bluenodes_soll=11
shop3blueedges_soll=13
shop3storedstates_soll=72 

owfn="$DIR/06-03-23_BPM06_shop_sect_3.owfn"
cmd="$FIONA $owfn -R -t IG"

if [ "$quiet" != "no" ]; then
    cmd="$cmd -Q"
fi

if [ "$memcheck" = "yes" ]; then
    memchecklog="$owfn.R.IG.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`

    echo $OUTPUT | grep "net is controllable: YES" > /dev/null
    shop3control=$?

    echo $OUTPUT | grep "number of blue nodes: $shop3bluenodes_soll" > /dev/null
    shop3bluenodes=$?

    echo $OUTPUT | grep "number of blue edges: $shop3blueedges_soll" > /dev/null
    shop3blueedges=$?

    echo $OUTPUT | grep "number of states stored in nodes: $shop3storedstates_soll" > /dev/null
    shop3storedstates=$?

    if [ $shop3control -ne 0 -o $shop3bluenodes -ne 0 -o $shop3blueedges -ne 0 -o $shop3storedstates -ne 0 ]
    then
    echo   ... failed to build the IG with node reduction correctly
    fi

    result=`expr $result + $shop3control + $shop3bluenodes + $shop3blueedges + $shop3storedstates`
fi


############################################################################
# reduced IG with node reduction
############################################################################

# Don't run this test, because -R is buggy.

shop3bluenodes_soll=8
shop3blueedges_soll=8
shop3storedstates_soll=49

owfn="$DIR/06-03-23_BPM06_shop_sect_3.owfn"
cmd="$FIONA $owfn -t IG -r -R"

if [ "$quiet" != "no" ]; then
    cmd="$cmd -Q"
fi

if [ "$memcheck" = "yes" ]; then
    memchecklog="$owfn.rR.IG.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`

    echo $OUTPUT | grep "net is controllable: YES" > /dev/null
    shop3control=$?

    echo $OUTPUT | grep "number of blue nodes: $shop3bluenodes_soll" > /dev/null
    shop3bluenodes=$?

    echo $OUTPUT | grep "number of blue edges: $shop3blueedges_soll" > /dev/null
    shop3blueedges=$?

    echo $OUTPUT | grep "number of states stored in nodes: $shop3storedstates_soll" > /dev/null
    shop3storedstates=$?

    if [ $shop3control -ne 0 -o $shop3bluenodes -ne 0 -o $shop3blueedges -ne 0 -o $shop3storedstates -ne 0 ]
    then
    echo   ... failed to build the reduced IG with node reduction correctly
    fi

    result=`expr $result + $shop3control + $shop3bluenodes + $shop3blueedges + $shop3storedstates`
fi


############################################################################
# IG with node reduction
############################################################################

# Don't run this test, because -R is buggy.

shop6bluenodes_soll=6
shop6blueedges_soll=5
shop6storedstates_soll=247 #314

owfn="$DIR/06-03-23_BPM06_shop_sect_6.owfn"
cmd="$FIONA $owfn -R -t IG"

if [ "$quiet" != "no" ]; then
    cmd="$cmd -Q"
fi

if [ "$memcheck" = "yes" ]; then
    memchecklog="$owfn.R.IG.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`

    echo $OUTPUT | grep "net is controllable: YES" > /dev/null
    shop6control=$?

    echo $OUTPUT | grep "number of blue nodes: $shop6bluenodes_soll" > /dev/null
    shop6bluenodes=$?

    echo $OUTPUT | grep "number of blue edges: $shop6blueedges_soll" > /dev/null
    shop6blueedges=$?

    echo $OUTPUT | grep "number of states stored in nodes: $shop6storedstates_soll" > /dev/null
    shop6storedstates=$?

    if [ $shop6control -ne 0 -o $shop6bluenodes -ne 0 -o $shop6blueedges -ne 0 -o $shop6storedstates -ne 0 ]
    then
    echo   ... failed to build IG with node reduction correctly
    fi

    result=`expr $result + $shop6control + $shop6bluenodes + $shop6blueedges + $shop6storedstates`
fi


############################################################################
# reduced IG with node reduction and IG reduction
############################################################################

# Don't run this test, because -R is buggy.

shop6bluenodes_soll=6
shop6blueedges_soll=5
shop6storedstates_soll=158

owfn="$DIR/06-03-23_BPM06_shop_sect_6.owfn"
cmd="$FIONA $owfn -t IG -r -R"

if [ "$quiet" != "no" ]; then
    cmd="$cmd -Q"
fi

if [ "$memcheck" = "yes" ]; then
    memchecklog="$owfn.rR.IG.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`

    echo $OUTPUT | grep "net is controllable: YES" > /dev/null
    shop6control=$?

    echo $OUTPUT | grep "number of blue nodes: $shop6bluenodes_soll" > /dev/null
    shop6bluenodes=$?

    echo $OUTPUT | grep "number of blue edges: $shop6blueedges_soll" > /dev/null
    shop6blueedges=$?

    echo $OUTPUT | grep "number of states stored in nodes: $shop6storedstates_soll" > /dev/null
    shop6storedstates=$?

    if [ $shop6control -ne 0 -o $shop6bluenodes -ne 0 -o $shop6blueedges -ne 0 -o $shop6storedstates -ne 0 ]
    then
    echo   ... failed to build reduced IG with node reduction correctly
    fi

    result=`expr $result + $shop6control + $shop6bluenodes + $shop6blueedges + $shop6storedstates`
fi

############################################################################

echo

exit $result
