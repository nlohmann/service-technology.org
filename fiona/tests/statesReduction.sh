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

#exit $result

############################################################################
# reduced IG with node reduction
############################################################################

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
# check if graph using -R is the same as the one generated without -R
############################################################################
owfn="$DIR/06-03-23_BPM06_shop_sect_3.owfn"
cmd1="$FIONA $owfn -R -t IG -s empty"

echo checking equivalence of the graphs with and without -R ...

#echo running $cmd1
OUTPUT=`$cmd1 2>&1`

cmd2="$FIONA $owfn -t IG -s empty"

#echo running $cmd2
OUTPUT=`$cmd2 2>&1`



cmd="$FIONA -t equivalence $owfn.ig.og $owfn.R.ig.og"

echo   running $cmd
OUTPUT=`$cmd 2>&1`

echo $OUTPUT | grep "The two OGs characterize the same strategies." > /dev/null
    equivalent=$?

if [ $equivalent -ne 0 ]
then
    echo   ... the two graphs do not characterize the same strategies
fi


result=`expr $result + $equivalent`

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
# check if graph using -R is the same as the one generated without -R
############################################################################
#owfn="$DIR/06-03-23_BPM06_shop_sect_3.owfn"
#cmd1="$FIONA $owfn -R -r -t IG -s empty"

#echo checking equivalence of the graphs with and without -R ...

#echo running $cmd1
#OUTPUT=`$cmd1 2>&1`

#cmd2="$FIONA $owfn -r -t IG -s empty"

#echo running $cmd2
#OUTPUT=`$cmd2 2>&1`



#cmd="$FIONA -t equivalence $owfn.ig.og $owfn.R.ig.og"

#echo   running $cmd
#OUTPUT=`$cmd 2>&1`

#echo $OUTPUT | grep "The two OGs characterize the same strategies." > /dev/null
#    equivalent=$?

#if [ $equivalent -ne 0 ]
#then
#    echo   ... the two graphs do not characterize the same strategies
#fi


#result=`expr $result + $equivalent`

############################################################################
# check if graph using -R is the same as the one generated without -R
# ==> OG
############################################################################
owfn="$DIR/06-03-23_BPM06_shop_sect_3.owfn"
cmd1="$FIONA $owfn -R -t OG -s empty"

echo checking equivalence of the graphs with and without -R ...

echo running $cmd1
OUTPUT=`$cmd1 2>&1`

cmd2="$FIONA $owfn -t OG -s empty"

echo running $cmd2
OUTPUT=`$cmd2 2>&1`



cmd="$FIONA -t equivalence $owfn.og $owfn.R.og"

echo   running $cmd
OUTPUT=`$cmd 2>&1`

echo $OUTPUT | grep "The two OGs characterize the same strategies." > /dev/null
    equivalent=$?

if [ $equivalent -ne 0 ]
then
    echo   ... the two graphs do not characterize the same strategies
fi


result=`expr $result + $equivalent`

############################################################################
# IG with node reduction
############################################################################

# Don't run this test, because -R is buggy.

shop6bluenodes_soll=6
shop6blueedges_soll=5
shop6storedstates_soll=263 #314

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
# check if graph using -R is the same as the one generated without -R
############################################################################
owfn="$DIR/06-03-23_BPM06_shop_sect_6.owfn"
cmd1="$FIONA $owfn -R -t IG -s empty"

echo checking equivalence of the graphs with and without -R ...

#echo running $cmd1
OUTPUT=`$cmd1 2>&1`

cmd2="$FIONA $owfn -t IG -s empty"

#echo running $cmd2
OUTPUT=`$cmd2 2>&1`



cmd="$FIONA -t equivalence $owfn.ig.og $owfn.R.ig.og"

echo   running $cmd
OUTPUT=`$cmd 2>&1`

echo $OUTPUT | grep "The two OGs characterize the same strategies." > /dev/null
    equivalent=$?

if [ $equivalent -ne 0 ]
then
    echo   ... the two graphs do not characterize the same strategies
fi


result=`expr $result + $equivalent`

############################################################################
# reduced IG with node reduction and IG reduction
############################################################################

# Don't run this test, because -R is buggy.

shop6bluenodes_soll=6
shop6blueedges_soll=5
shop6storedstates_soll=170

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
# check if graph using -R is the same as the one generated without -R
############################################################################
#owfn="$DIR/06-03-23_BPM06_shop_sect_6.owfn"
#cmd1="$FIONA $owfn -R -r -t IG -s empty"

#echo checking equivalence of the graphs with and without -R ...

#echo running $cmd1
#OUTPUT=`$cmd1 2>&1`

#cmd2="$FIONA $owfn -r -t IG -s empty"

#echo running $cmd2
#OUTPUT=`$cmd2 2>&1`



#cmd="$FIONA -t equivalence $owfn.ig.og $owfn.R.ig.og"

#echo   running $cmd
#OUTPUT=`$cmd 2>&1`

#echo $OUTPUT | grep "The two OGs characterize the same strategies." > /dev/null
#    equivalent=$?

#if [ $equivalent -ne 0 ]
#then
#    echo   ... the two graphs do not characterize the same strategies
#fi


#result=`expr $result + $equivalent`

############################################################################

############################################################################
# reduced IG with node reduction
############################################################################

myCoffeebluenodes_soll=5
myCoffeeblueedges_soll=5
myCoffeestoredstates_soll=5

owfn="$DIR/myCoffee.owfn"
cmd="$FIONA $owfn -t IG -R"

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
    myCoffeecontrol=$?

    echo $OUTPUT | grep "number of blue nodes: $myCoffeebluenodes_soll" > /dev/null
    myCoffeebluenodes=$?

    echo $OUTPUT | grep "number of blue edges: $myCoffeeblueedges_soll" > /dev/null
    myCoffeeblueedges=$?

    echo $OUTPUT | grep "number of states stored in nodes: $myCoffeestoredstates_soll" > /dev/null
    myCoffeestoredstates=$?

    if [ $myCoffeecontrol -ne 0 -o $myCoffeebluenodes -ne 0 -o $myCoffeeblueedges -ne 0 -o $myCoffeestoredstates -ne 0 ]
    then
    echo   ... failed to build IG with node reduction correctly
    fi

    result=`expr $result + $myCoffeecontrol + $myCoffeebluenodes + $myCoffeeblueedges + $myCoffeestoredstates`
fi

############################################################################
# check if graph using -R is the same as the one generated without -R
############################################################################
owfn="$DIR/myCoffee.owfn"
cmd1="$FIONA $owfn -R -t IG -s empty"

echo checking equivalence of the graphs with and without -R ...

#echo running $cmd1
OUTPUT=`$cmd1 2>&1`

cmd2="$FIONA $owfn -t IG -s empty"

#echo running $cmd2
OUTPUT=`$cmd2 2>&1`



cmd="$FIONA -t equivalence $owfn.ig.og $owfn.R.ig.og"

echo   running $cmd
OUTPUT=`$cmd 2>&1`

echo $OUTPUT | grep "The two OGs characterize the same strategies." > /dev/null
    equivalent=$?

if [ $equivalent -ne 0 ]
then
    echo   ... the two graphs do not characterize the same strategies
fi


result=`expr $result + $equivalent`


############################################################################
# reduced IG with node reduction
############################################################################

DKE3bluenodes_soll=172
DKE3blueedges_soll=357
DKE3storedstates_soll=2586

owfn="$DIR/dke07_shop_sect_3.owfn"
cmd="$FIONA $owfn -t IG -R"

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
    DKE3control=$?

    echo $OUTPUT | grep "number of blue nodes: $DKE3bluenodes_soll" > /dev/null
    DKE3bluenodes=$?

    echo $OUTPUT | grep "number of blue edges: $DKE3blueedges_soll" > /dev/null
    DKE3blueedges=$?

    echo $OUTPUT | grep "number of states stored in nodes: $DKE3storedstates_soll" > /dev/null
    DKE3storedstates=$?

    if [ $DKE3control -ne 0 -o $DKE3bluenodes -ne 0 -o $DKE3blueedges -ne 0 -o $DKE3storedstates -ne 0 ]
    then
    echo   ... failed to build IG with node reduction correctly
    fi

    result=`expr $result + $DKE3control + $DKE3bluenodes + $DKE3blueedges + $DKE3storedstates`
fi

############################################################################
# check if graph using -R is the same as the one generated without -R
############################################################################
owfn="$DIR/dke07_shop_sect_3.owfn"
cmd1="$FIONA $owfn -R -t IG -s empty"

echo checking equivalence of the graphs with and without -R ...

#echo running $cmd1
OUTPUT=`$cmd1 2>&1`

cmd2="$FIONA $owfn -t IG -s empty"

#echo running $cmd2
OUTPUT=`$cmd2 2>&1`



cmd="$FIONA -t equivalence $owfn.ig.og $owfn.R.ig.og"

echo   running $cmd
OUTPUT=`$cmd 2>&1`

echo $OUTPUT | grep "The two OGs characterize the same strategies." > /dev/null
    equivalent=$?

if [ $equivalent -ne 0 ]
then
    echo   ... the two graphs do not characterize the same strategies
fi


result=`expr $result + $equivalent`

############################################################################
# reduced IG with node reduction
############################################################################

DKE6bluenodes_soll=5
DKE6blueedges_soll=4
DKE6storedstates_soll=2318

owfn="$DIR/dke07_shop_sect_6.owfn"
cmd="$FIONA $owfn -t IG -R"

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
    DKE6control=$?

    echo $OUTPUT | grep "number of blue nodes: $DKE6bluenodes_soll" > /dev/null
    DKE6bluenodes=$?

    echo $OUTPUT | grep "number of blue edges: $DKE6blueedges_soll" > /dev/null
    DKE6blueedges=$?

    echo $OUTPUT | grep "number of states stored in nodes: $DKE6storedstates_soll" > /dev/null
    DKE6storedstates=$?

    if [ $DKE6control -ne 0 -o $DKE6bluenodes -ne 0 -o $DKE6blueedges -ne 0 -o $DKE6storedstates -ne 0 ]
    then
    echo   ... failed to build IG with node reduction correctly
    fi

    result=`expr $result + $DKE6control + $DKE6bluenodes + $DKE6blueedges + $DKE6storedstates`
fi

############################################################################
# check if graph using -R is the same as the one generated without -R
############################################################################
owfn="$DIR/dke07_shop_sect_6.owfn"
cmd1="$FIONA $owfn -R -t IG -s empty"

echo checking equivalence of the graphs with and without -R ...

#echo running $cmd1
OUTPUT=`$cmd1 2>&1`

cmd2="$FIONA $owfn -t IG -s empty"

#echo running $cmd2
OUTPUT=`$cmd2 2>&1`



cmd="$FIONA -t equivalence $owfn.ig.og $owfn.R.ig.og"

echo   running $cmd
OUTPUT=`$cmd 2>&1`

echo $OUTPUT | grep "The two OGs characterize the same strategies." > /dev/null
    equivalent=$?

if [ $equivalent -ne 0 ]
then
    echo   ... the two graphs do not characterize the same strategies
fi


result=`expr $result + $equivalent`

############################################################################
# reduced IG with node reduction
############################################################################

Reservationsbluenodes_soll=5
Reservationsblueedges_soll=4
Reservationsstoredstates_soll=2318

owfn="$DIR/Reservations.owfn"
cmd="$FIONA $owfn -t IG -R"

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
    Reservationscontrol=$?

    echo $OUTPUT | grep "number of blue nodes: $Reservationsbluenodes_soll" > /dev/null
    Reservationsbluenodes=$?

    echo $OUTPUT | grep "number of blue edges: $Reservationsblueedges_soll" > /dev/null
    Reservationsblueedges=$?

    echo $OUTPUT | grep "number of states stored in nodes: $Reservationsstoredstates_soll" > /dev/null
    Reservationsstoredstates=$?

    if [ $Reservationscontrol -ne 0 -o $Reservationsbluenodes -ne 0 -o $Reservationsblueedges -ne 0 -o $Reservationsstoredstates -ne 0 ]
    then
    echo   ... failed to build IG with node reduction correctly
    fi

    result=`expr $result + $Reservationscontrol + $Reservationsbluenodes + $Reservationsblueedges + $Reservationsstoredstates`
fi

############################################################################
# check if graph using -R is the same as the one generated without -R
############################################################################
owfn="$DIR/dke07_shop_sect_6.owfn"
cmd1="$FIONA $owfn -R -t IG -s empty"

echo checking equivalence of the graphs with and without -R ...

#echo running $cmd1
OUTPUT=`$cmd1 2>&1`

cmd2="$FIONA $owfn -t IG -s empty"

#echo running $cmd2
OUTPUT=`$cmd2 2>&1`



cmd="$FIONA -t equivalence $owfn.ig.og $owfn.R.ig.og"

echo   running $cmd
OUTPUT=`$cmd 2>&1`

echo $OUTPUT | grep "The two OGs characterize the same strategies." > /dev/null
    equivalent=$?

if [ $equivalent -ne 0 ]
then
    echo   ... the two graphs do not characterize the same strategies
fi


result=`expr $result + $equivalent`

############################################################################

############################################################################

#loeschen aller erzeugten Dateien, die im Durchlauf entstanden sind
rm -f $DIR/*.out
rm -f $DIR/*.OG.png
rm -f $DIR/*.IG.png
rm -f $DIR/*.og
rm -f $DIR/*.log

echo

exit $result
