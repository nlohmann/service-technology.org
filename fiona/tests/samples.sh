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

DIR=$testdir/samples
FIONA=fiona

#loeschen aller erzeugten Dateien im letzten Durchlauf
rm -f $DIR/*.out
rm -f $DIR/*.OG.png
rm -f $DIR/*.IG.png
rm -f $DIR/*.og
rm -f $DIR/*.log

result=0

############################################################################
# OG
############################################################################

mycoffeebluenodes_soll=7
mycoffeeblueedges_soll=9
mycoffeestoredstates_soll=32

owfn="$DIR/myCoffee.owfn"
cmd="$FIONA $owfn -t OG"

if [ "$quiet" != "no" ]; then
    cmd="$cmd -Q"
fi

if [ "$memcheck" = "yes" ]; then
    memchecklog="$owfn.OG.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$?
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`

    echo $OUTPUT | grep "net is controllable: YES" > /dev/null
    mycoffeecontrol=$?

    echo $OUTPUT | grep "number of blue nodes: $mycoffeebluenodes_soll" > /dev/null
    mycoffeebluenodes=$?

    echo $OUTPUT | grep "number of blue edges: $mycoffeeblueedges_soll" > /dev/null
    mycoffeeblueedges=$?

    echo $OUTPUT | grep "number of states stored in nodes: $mycoffeestoredstates_soll" > /dev/null
    mycoffeestoredstates=$?

    if [ $mycoffeecontrol -ne 0 -o $mycoffeebluenodes -ne 0 -o $mycoffeeblueedges -ne 0 -o $mycoffeestoredstates -ne 0 ]
    then
    echo   ... failed to build OG correctly
    fi

    result=`expr $result + $mycoffeecontrol + $mycoffeebluenodes + $mycoffeeblueedges + $mycoffeestoredstates`
fi


############################################################################
# IG
############################################################################

mycoffeebluenodes_soll=5
mycoffeeblueedges_soll=5
mycoffeestoredstates_soll=12

owfn="$DIR/myCoffee.owfn"
cmd="$FIONA $owfn -t IG"

if [ "$quiet" != "no" ]; then
    cmd="$cmd -Q"
fi

if [ "$memcheck" = "yes" ]; then
    memchecklog="$owfn.IG.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$?
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`

    echo $OUTPUT | grep "net is controllable: YES" > /dev/null
    mycoffeecontrol=$?

    echo $OUTPUT | grep "number of blue nodes: $mycoffeebluenodes_soll" > /dev/null
    mycoffeebluenodes=$?

    echo $OUTPUT | grep "number of blue edges: $mycoffeeblueedges_soll" > /dev/null
    mycoffeeblueedges=$?

    echo $OUTPUT | grep "number of states stored in nodes: $mycoffeestoredstates_soll" > /dev/null
    mycoffeestoredstates=$?

    if [ $mycoffeecontrol -ne 0 -o $mycoffeebluenodes -ne 0 -o $mycoffeeblueedges -ne 0 -o $mycoffeestoredstates -ne 0 ]
    then
    echo   ... failed to build IG correctly
    fi

    result=`expr $result + $mycoffeecontrol + $mycoffeebluenodes + $mycoffeeblueedges + $mycoffeestoredstates`
fi

############################################################################
# reduced IG
############################################################################

mycoffeebluenodes_soll=5
mycoffeeblueedges_soll=5
mycoffeestoredstates_soll=12

owfn="$DIR/myCoffee.owfn"
cmd="$FIONA $owfn -r -t IG"

if [ "$quiet" != "no" ]; then
    cmd="$cmd -Q"
fi

if [ "$memcheck" = "yes" ]; then
    memchecklog="$owfn.r.IG.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$?
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`

    echo $OUTPUT | grep "net is controllable: YES" > /dev/null
    mycoffeecontrol=$?

    echo $OUTPUT | grep "number of blue nodes: $mycoffeebluenodes_soll" > /dev/null
    mycoffeebluenodes=$?

    echo $OUTPUT | grep "number of blue edges: $mycoffeeblueedges_soll" > /dev/null
    mycoffeeblueedges=$?

    echo $OUTPUT | grep "number of states stored in nodes: $mycoffeestoredstates_soll" > /dev/null
    mycoffeestoredstates=$?

    if [ $mycoffeecontrol -ne 0 -o $mycoffeebluenodes -ne 0 -o $mycoffeeblueedges -ne 0 -o $mycoffeestoredstates -ne 0 ]
    then
    echo   ... failed to build reduced IG correctly
    fi

    result=`expr $result + $mycoffeecontrol + $mycoffeebluenodes + $mycoffeeblueedges + $mycoffeestoredstates`
fi

############################################################################
# reduced IG
############################################################################

# Don't run this test, because -R is buggy.

#mycoffeebluenodes_soll=5
#mycoffeeblueedges_soll=5
#mycoffeestoredstates_soll=5
#mycoffeestates_soll=19
#
#owfn="$DIR/myCoffee.owfn"
#cmd="$FIONA $owfn -r -R -t IG"
#
#if [ "$quiet" != "no" ]; then
#    cmd="$cmd -Q"
#fi
#
#if [ "$memcheck" = "yes" ]; then
#    memchecklog="$owfn.rR.IG.memcheck.log"
#    do_memcheck "$cmd" "$memchecklog"
#    result=$?
#else
#    echo running $cmd
#    OUTPUT=`$cmd 2>&1`
#
#    echo $OUTPUT | grep "net is controllable: YES" > /dev/null
#    mycoffeecontrol=$?
#
#    echo $OUTPUT | grep "number of states calculated: $mycoffeestates_soll" > /dev/null
#    mycoffeestates=$?
#
#    echo $OUTPUT | grep "number of blue nodes: $mycoffeebluenodes_soll" > /dev/null
#    mycoffeebluenodes=$?
#
#    echo $OUTPUT | grep "number of blue edges: $mycoffeeblueedges_soll" > /dev/null
#    mycoffeeblueedges=$?
#
#    echo $OUTPUT | grep "number of states stored in nodes: $mycoffeestoredstates_soll" > /dev/null
#    mycoffeestoredstates=$?
#
#    if [ $mycoffeecontrol -ne 0 -o $mycoffeestates -ne 0 -o $mycoffeebluenodes -ne 0 -o $mycoffeeblueedges -ne 0 -o $mycoffeestoredstates -ne 0 ]
#    then
#    echo   ... failed to build reduced IG correctly
#    fi
#
#    result=`expr $result + $mycoffeecontrol + $mycoffeebluenodes + $mycoffeeblueedges + $mycoffeestoredstates`
#fi

############################################################################
# IG with node reduction
############################################################################

# Don't run this test, because -R is buggy.

#mycoffeebluenodes_soll=5
#mycoffeeblueedges_soll=5
#mycoffeestoredstates_soll=5
#mycoffeestates_soll=19
#
#owfn="$DIR/myCoffee.owfn"
#cmd="$FIONA $owfn -R -t IG"
#
#if [ "$quiet" != "no" ]; then
#    cmd="$cmd -Q"
#fi
#
#if [ "$memcheck" = "yes" ]; then
#    memchecklog="$owfn.R.IG.memcheck.log"
#    do_memcheck "$cmd" "$memchecklog"
#    result=$?
#else
#    echo running $cmd
#    OUTPUT=`$cmd 2>&1`
#
#    echo $OUTPUT | grep "net is controllable: YES" > /dev/null
#    mycoffeecontrol=$?
#
#    echo $OUTPUT | grep "number of states calculated: $mycoffeestates_soll" > /dev/null
#    mycoffeestates=$?
#
#    echo $OUTPUT | grep "number of blue nodes: $mycoffeebluenodes_soll" > /dev/null
#    mycoffeebluenodes=$?
#
#    echo $OUTPUT | grep "number of blue edges: $mycoffeeblueedges_soll" > /dev/null
#    mycoffeeblueedges=$?
#
#    echo $OUTPUT | grep "number of states stored in nodes: $mycoffeestoredstates_soll" > /dev/null
#    mycoffeestoredstates=$?
#
#    if [ $mycoffeecontrol -ne 0 -o $mycoffeestates -ne 0 -o $mycoffeebluenodes -ne 0 -o $mycoffeeblueedges -ne 0 -o $mycoffeestoredstates -ne 0 ]
#    then
#    echo   ... failed to build IG with node reduction correctly
#    fi
#
#    result=`expr $result + $mycoffeecontrol + $mycoffeebluenodes + $mycoffeeblueedges + $mycoffeestoredstates`
#fi

############################################################################

keescoffee1bluenodes_soll=9
keescoffee1blueedges_soll=13
keescoffee1storedstates_soll=100

owfn="$DIR/keesCoffee_condition.owfn"
cmd="$FIONA $owfn -t OG -e 1"

if [ "$quiet" != "no" ]; then
    cmd="$cmd -Q"
fi

if [ "$memcheck" = "yes" ]; then
    memchecklog="$owfn.e1.OG.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$?
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`

    echo $OUTPUT | grep "net is controllable: YES" > /dev/null
    keescoffee1control=$?

    echo $OUTPUT | grep "number of blue nodes: $keescoffee1bluenodes_soll" > /dev/null
    keescoffee1bluenodes=$?

    echo $OUTPUT | grep "number of blue edges: $keescoffee1blueedges_soll" > /dev/null
    keescoffee1blueedges=$?

    echo $OUTPUT | grep "number of states stored in nodes: $keescoffee1storedstates_soll" > /dev/null
    keescoffee1storedstates=$?

    if [ $keescoffee1control -ne 0 -o $keescoffee1bluenodes -ne 0 -o $keescoffee1blueedges -ne 0 -o $keescoffee1storedstates -ne 0 ]
    then
    echo   ... failed to build OG correctly
    fi

    result=`expr $result + $keescoffee1control + $keescoffee1bluenodes + $keescoffee1blueedges + $keescoffee1storedstates`
fi

############################################################################

keescoffee2bluenodes_soll=20
keescoffee2blueedges_soll=41
keescoffee2storedstates_soll=212

owfn="$DIR/keesCoffee_condition.owfn"
cmd="$FIONA $owfn -t OG -e 3 -m 1"

if [ "$quiet" != "no" ]; then
    cmd="$cmd -Q"
fi

if [ "$memcheck" = "yes" ]; then
    memchecklog="$owfn.e2.OG.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$?
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`

    echo $OUTPUT | grep "net is controllable: YES" > /dev/null
    keescoffee2control=$?

    echo $OUTPUT | grep "number of blue nodes: $keescoffee2bluenodes_soll" > /dev/null
    keescoffee2bluenodes=$?

    echo $OUTPUT | grep "number of blue edges: $keescoffee2blueedges_soll" > /dev/null
    keescoffee2blueedges=$?

    echo $OUTPUT | grep "number of states stored in nodes: $keescoffee2storedstates_soll" > /dev/null
    keescoffee2storedstates=$?

    if [ $keescoffee2control -ne 0 -o $keescoffee2bluenodes -ne 0 -o $keescoffee2blueedges -ne 0 -o $keescoffee2storedstates -ne 0 ]
    then
    echo   ... failed to build OG correctly
    fi

    result=`expr $result + $keescoffee2control + $keescoffee2bluenodes + $keescoffee2blueedges + $keescoffee2storedstates`
fi

############################################################################
# OG
############################################################################

shop3bluenodes_soll=12
shop3blueedges_soll=15
shop3storedstates_soll=4530

owfn="$DIR/06-03-23_BPM06_shop_sect_3.owfn"
cmd="$FIONA $owfn -t OG"

if [ "$quiet" != "no" ]; then
    cmd="$cmd -Q"
fi

if [ "$memcheck" = "yes" ]; then
    memchecklog="$owfn.OG.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$?
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
    echo   ... failed to build OG correctly
    fi

    result=`expr $result + $shop3control + $shop3bluenodes + $shop3blueedges + $shop3storedstates`
fi

############################################################################
# OG with node reduction
############################################################################

# Don't run this test, because -R is buggy.

#shop3bluenodes_soll=12
#shop3blueedges_soll=15
#shop3storedstates_soll=978
#
#owfn="$DIR/06-03-23_BPM06_shop_sect_3.owfn"
#cmd="$FIONA $owfn -R -t OG"
#
#if [ "$quiet" != "no" ]; then
#    cmd="$cmd -Q"
#fi
#
#if [ "$memcheck" = "yes" ]; then
#    memchecklog="$owfn.R.OG.memcheck.log"
#    do_memcheck "$cmd" "$memchecklog"
#    result=$?
#else
#    echo running $cmd
#    OUTPUT=`$cmd 2>&1`
#
#    echo $OUTPUT | grep "net is controllable: YES" > /dev/null
#    shop3control=$?
#
#    echo $OUTPUT | grep "number of blue nodes: $shop3bluenodes_soll" > /dev/null
#    shop3bluenodes=$?
#
#    echo $OUTPUT | grep "number of blue edges: $shop3blueedges_soll" > /dev/null
#    shop3blueedges=$?
#
#    echo $OUTPUT | grep "number of states stored in nodes: $shop3storedstates_soll" > /dev/null
#    shop3storedstates=$?
#
#    if [ $shop3control -ne 0 -o $shop3bluenodes -ne 0 -o $shop3blueedges -ne 0 -o $shop3storedstates -ne 0 ]
#    then
#    echo   ... failed to build OG with node reduction correctly
#    fi
#
#    result=`expr $result + $shop3control + $shop3bluenodes + $shop3blueedges + $shop3storedstates`
#fi

############################################################################
# IG
############################################################################

shop3bluenodes_soll=11
shop3blueedges_soll=13
shop3storedstates_soll=728

owfn="$DIR/06-03-23_BPM06_shop_sect_3.owfn"
cmd="$FIONA $owfn -t IG"

if [ "$quiet" != "no" ]; then
    cmd="$cmd -Q"
fi

if [ "$memcheck" = "yes" ]; then
    memchecklog="$owfn.IG.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$?
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
    echo   ... failed to build the IG correctly
    fi

    result=`expr $result + $shop3control + $shop3bluenodes + $shop3blueedges + $shop3storedstates`
fi

############################################################################
# reduced IG with node reduction
############################################################################

# Don't run this test, because -R is buggy.

#shop3bluenodes_soll=11
#shop3blueedges_soll=13
#shop3storedstates_soll=83
#
#owfn="$DIR/06-03-23_BPM06_shop_sect_3.owfn"
#cmd="$FIONA $owfn -R -t IG"
#
#if [ "$quiet" != "no" ]; then
#    cmd="$cmd -Q"
#fi
#
#if [ "$memcheck" = "yes" ]; then
#    memchecklog="$owfn.R.IG.memcheck.log"
#    do_memcheck "$cmd" "$memchecklog"
#    result=$?
#else
#    echo running $cmd
#    OUTPUT=`$cmd 2>&1`
#
#    echo $OUTPUT | grep "net is controllable: YES" > /dev/null
#    shop3control=$?
#
#    echo $OUTPUT | grep "number of blue nodes: $shop3bluenodes_soll" > /dev/null
#    shop3bluenodes=$?
#
#    echo $OUTPUT | grep "number of blue edges: $shop3blueedges_soll" > /dev/null
#    shop3blueedges=$?
#
#    echo $OUTPUT | grep "number of states stored in nodes: $shop3storedstates_soll" > /dev/null
#    shop3storedstates=$?
#
#    if [ $shop3control -ne 0 -o $shop3bluenodes -ne 0 -o $shop3blueedges -ne 0 -o $shop3storedstates -ne 0 ]
#    then
#    echo   ... failed to build the IG with node reduction correctly
#    fi
#
#    result=`expr $result + $shop3control + $shop3bluenodes + $shop3blueedges + $shop3storedstates`
#fi

############################################################################
# IG reduced
############################################################################

shop3bluenodes_soll=8 #7
shop3blueedges_soll=8 #6
shop3storedstates_soll=351 #329

owfn="$DIR/06-03-23_BPM06_shop_sect_3.owfn"
cmd="$FIONA $owfn -t IG -r"

if [ "$quiet" != "no" ]; then
    cmd="$cmd -Q"
fi

if [ "$memcheck" = "yes" ]; then
    memchecklog="$owfn.r.IG.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$?
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
    echo   ... failed to build the reduced IG correctly
    fi

    result=`expr $result + $shop3control + $shop3bluenodes + $shop3blueedges + $shop3storedstates`
fi

############################################################################
# reduced IG with node reduction
############################################################################

# Don't run this test, because -R is buggy.

#shop3bluenodes_soll=8
#shop3blueedges_soll=8
#shop3storedstates_soll=33
#
#owfn="$DIR/06-03-23_BPM06_shop_sect_3.owfn"
#cmd="$FIONA $owfn -t IG -r -R"
#
#if [ "$quiet" != "no" ]; then
#    cmd="$cmd -Q"
#fi
#
#if [ "$memcheck" = "yes" ]; then
#    memchecklog="$owfn.rR.IG.memcheck.log"
#    do_memcheck "$cmd" "$memchecklog"
#    result=$?
#else
#    echo running $cmd
#    OUTPUT=`$cmd 2>&1`
#
#    echo $OUTPUT | grep "net is controllable: YES" > /dev/null
#    shop3control=$?
#
#    echo $OUTPUT | grep "number of blue nodes: $shop3bluenodes_soll" > /dev/null
#    shop3bluenodes=$?
#
#    echo $OUTPUT | grep "number of blue edges: $shop3blueedges_soll" > /dev/null
#    shop3blueedges=$?
#
#    echo $OUTPUT | grep "number of states stored in nodes: $shop3storedstates_soll" > /dev/null
#    shop3storedstates=$?
#
#    if [ $shop3control -ne 0 -o $shop3bluenodes -ne 0 -o $shop3blueedges -ne 0 -o $shop3storedstates -ne 0 ]
#    then
#    echo   ... failed to build the reduced IG with node reduction correctly
#    fi
#
#    result=`expr $result + $shop3control + $shop3bluenodes + $shop3blueedges + $shop3storedstates`
#fi

############################################################################
# OG
############################################################################

shop6bluenodes_soll=7
shop6blueedges_soll=7
shop6storedstates_soll=6932

owfn="$DIR/06-03-23_BPM06_shop_sect_6.owfn"
cmd="$FIONA $owfn -t OG"

if [ "$quiet" != "no" ]; then
    cmd="$cmd -Q"
fi

if [ "$memcheck" = "yes" ]; then
    memchecklog="$owfn.OG.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$?
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
    echo   ... failed to build OG correctly
    fi

    result=`expr $result + $shop6control + $shop6bluenodes + $shop6blueedges + $shop6storedstates`
fi

############################################################################
# OG with node reduction ... takes too long (more than a minute!)
############################################################################

#shop6bluenodes_soll=7
#shop6blueedges_soll=7
#shop6storedstates_soll=761

#owfn="$DIR/06-03-23_BPM06_shop_sect_6.owfn"
#cmd="$FIONA $owfn -a -t OG"

#if [ "$quiet" != "no" ]; then
#    cmd="$cmd -Q"
#fi

#echo running $cmd
#OUTPUT=`$cmd 2>&1`

#echo $OUTPUT | grep "net is controllable: YES" > /dev/null
#shop6control=$?

#echo $OUTPUT | grep "number of blue nodes: $shop6bluenodes_soll" > /dev/null
#shop6bluenodes=$?

#echo $OUTPUT | grep "number of blue edges: $shop6blueedges_soll" > /dev/null
#shop6blueedges=$?

#echo $OUTPUT | grep "number of states stored in nodes: $shop6storedstates_soll" > /dev/null
#shop6storedstates=$?

#if [ $shop6control -ne 0 -o $shop6bluenodes -ne 0 -o $shop6blueedges -ne 0 -o $shop6storedstates -ne 0 ]
#then
#echo   ... failed to build OG correctly
#fi

#result=`expr $result + $shop6control + $shop6bluenodes + $shop6blueedges + $shop6storedstates`

############################################################################
# IG
############################################################################

shop6bluenodes_soll=6
shop6blueedges_soll=5
shop6storedstates_soll=2491

owfn="$DIR/06-03-23_BPM06_shop_sect_6.owfn"
cmd="$FIONA $owfn -t IG"

if [ "$quiet" != "no" ]; then
    cmd="$cmd -Q"
fi

if [ "$memcheck" = "yes" ]; then
    memchecklog="$owfn.IG.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$?
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
    echo   ... failed to build IG correctly
    fi

    result=`expr $result + $shop6control + $shop6bluenodes + $shop6blueedges + $shop6storedstates`
fi

############################################################################
# IG with node reduction
############################################################################

# Don't run this test, because -R is buggy.

#shop6bluenodes_soll=6
#shop6blueedges_soll=5
#shop6storedstates_soll=314
#
#owfn="$DIR/06-03-23_BPM06_shop_sect_6.owfn"
#cmd="$FIONA $owfn -R -t IG"
#
#if [ "$quiet" != "no" ]; then
#    cmd="$cmd -Q"
#fi
#
#if [ "$memcheck" = "yes" ]; then
#    memchecklog="$owfn.R.IG.memcheck.log"
#    do_memcheck "$cmd" "$memchecklog"
#    result=$?
#else
#    echo running $cmd
#    OUTPUT=`$cmd 2>&1`
#
#    echo $OUTPUT | grep "net is controllable: YES" > /dev/null
#    shop6control=$?
#
#    echo $OUTPUT | grep "number of blue nodes: $shop6bluenodes_soll" > /dev/null
#    shop6bluenodes=$?
#
#    echo $OUTPUT | grep "number of blue edges: $shop6blueedges_soll" > /dev/null
#    shop6blueedges=$?
#
#    echo $OUTPUT | grep "number of states stored in nodes: $shop6storedstates_soll" > /dev/null
#    shop6storedstates=$?
#
#    if [ $shop6control -ne 0 -o $shop6bluenodes -ne 0 -o $shop6blueedges -ne 0 -o $shop6storedstates -ne 0 ]
#    then
#    echo   ... failed to build IG with node reduction correctly
#    fi
#
#    result=`expr $result + $shop6control + $shop6bluenodes + $shop6blueedges + $shop6storedstates`
#fi

############################################################################
# reduced IG
############################################################################

# Don't run this test, because -R is buggy.

#shop6bluenodes_soll=6
#shop6blueedges_soll=5
#shop6storedstates_soll=1109   #1087
#
#owfn="$DIR/06-03-23_BPM06_shop_sect_6.owfn"
#cmd="$FIONA $owfn -t IG -r -R"
#
#if [ "$quiet" != "no" ]; then
#    cmd="$cmd -Q"
#fi
#
#if [ "$memcheck" = "yes" ]; then
#    memchecklog="$owfn.rR.IG.memcheck.log"
#    do_memcheck "$cmd" "$memchecklog"
#    result=$?
#else
#    echo running $cmd
#    OUTPUT=`$cmd 2>&1`
#
#    echo $OUTPUT | grep "net is controllable: YES" > /dev/null
#    shop6control=$?
#
#    echo $OUTPUT | grep "number of blue nodes: $shop6bluenodes_soll" > /dev/null
#    shop6bluenodes=$?
#
#    echo $OUTPUT | grep "number of blue edges: $shop6blueedges_soll" > /dev/null
#    shop6blueedges=$?
#
#    echo $OUTPUT | grep "number of states stored in nodes: $shop6storedstates_soll" > /dev/null
#    shop6storedstates=$?
#
#    if [ $shop6control -ne 0 -o $shop6bluenodes -ne 0 -o $shop6blueedges -ne 0 -o $shop6storedstates -ne 0 ]
#    then
#    echo   ... failed to build reduced IG correctly
#    fi
#
#    result=`expr $result + $shop6control + $shop6bluenodes + $shop6blueedges + $shop6storedstates`
#fi

############################################################################
# reduced IG with node reduction
############################################################################

# Don't run this test, because -R is buggy.

#shop6bluenodes_soll=6
#shop6blueedges_soll=5
#shop6storedstates_soll=146
#
#owfn="$DIR/06-03-23_BPM06_shop_sect_6.owfn"
#cmd="$FIONA $owfn -t IG -r -R"
#
#if [ "$quiet" != "no" ]; then
#    cmd="$cmd -Q"
#fi
#
#if [ "$memcheck" = "yes" ]; then
#    memchecklog="$owfn.rR.IG.memcheck.log"
#    do_memcheck "$cmd" "$memchecklog"
#    result=$?
#else
#    echo running $cmd
#    OUTPUT=`$cmd 2>&1`
#
#    echo $OUTPUT | grep "net is controllable: YES" > /dev/null
#    shop6control=$?
#
#    echo $OUTPUT | grep "number of blue nodes: $shop6bluenodes_soll" > /dev/null
#    shop6bluenodes=$?
#
#    echo $OUTPUT | grep "number of blue edges: $shop6blueedges_soll" > /dev/null
#    shop6blueedges=$?
#
#    echo $OUTPUT | grep "number of states stored in nodes: $shop6storedstates_soll" > /dev/null
#    shop6storedstates=$?
#
#    if [ $shop6control -ne 0 -o $shop6bluenodes -ne 0 -o $shop6blueedges -ne 0 -o $shop6storedstates -ne 0 ]
#    then
#    echo   ... failed to build reduced IG with node reduction correctly
#    fi
#
#    result=`expr $result + $shop6control + $shop6bluenodes + $shop6blueedges + $shop6storedstates`
#fi

############################################################################

echo

exit $result
