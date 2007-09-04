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

DIR=$testdir/cyclic
FIONA=fiona

#loeschen aller erzeugten Dateien im letzten Durchlauf
rm -f $DIR/*.out
rm -f $DIR/*.OG.png
rm -f $DIR/*.IG.png
rm -f $DIR/*.og
rm -f $DIR/*.log
rm -f $DIR/*.dot

result=0

############################################################################

zyklusRbluenodes_soll=7
zyklusRblueedges_soll=6
zyklusRstoredstates_soll=0

owfn="$DIR/zyklusR.owfn"
cmd="$FIONA -t OG $owfn -e5 -m5"

if [ "$quiet" != "no" ]; then
    cmd="$cmd -Q"
fi

if [ "$memcheck" = "yes" ]; then
    memchecklog="$owfn.e5.OG.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`

    echo $OUTPUT | grep "net is controllable: YES" > /dev/null
    zyklusRcontrol=$?

    echo $OUTPUT | grep "number of blue nodes: $zyklusRbluenodes_soll" > /dev/null
    zyklusRbluenodes=$?

    echo $OUTPUT | grep "number of blue edges: $zyklusRblueedges_soll" > /dev/null
    zyklusRblueedges=$?

    if [ $zyklusRcontrol -ne 0 -o $zyklusRbluenodes -ne 0 -o $zyklusRblueedges -ne 0 ]
    then
    echo   ... failed to build OG correctly
    fi

    result=`expr $result + $zyklusRcontrol + $zyklusRbluenodes + $zyklusRblueedges`
fi

#############################################################################

owfn="$DIR/zyklusP.owfn"
cmd="$FIONA -t OG $owfn -e10 -m5"

if [ "$quiet" != "no" ]; then
    cmd="$cmd -Q"
fi

if [ "$memcheck" = "yes" ]; then
    memchecklog="$owfn.e10.m5.OG.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`

    echo $OUTPUT | grep "net is controllable: NO" > /dev/null
    zyklusPcontrol=$?

    if [ $zyklusPcontrol -ne 0 ]
    then
    echo   ... failed to build OG correctly
    fi

    result=`expr $result + $zyklusPcontrol`
fi

############################################################################

zyklusPcommitbluenodes_soll=4
zyklusPcommitblueedges_soll=5
zyklusPcommitstoredstates_soll=11

owfn="$DIR/zyklusPmitCommit.owfn"
cmd="$FIONA -t OG $owfn -e2 -m1"

if [ "$quiet" != "no" ]; then
    cmd="$cmd -Q"
fi

if [ "$memcheck" = "yes" ]; then
    memchecklog="$owfn.e2.m1.OG.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`

    echo $OUTPUT | grep "net is controllable: YES" > /dev/null
    zyklusPcommitcontrol=$?

    echo $OUTPUT | grep "number of blue nodes: $zyklusPcommitbluenodes_soll" > /dev/null
    zyklusPcommitbluenodes=$?

    echo $OUTPUT | grep "number of blue edges: $zyklusPcommitblueedges_soll" > /dev/null
    zyklusPcommitblueedges=$?

    echo $OUTPUT | grep "number of states stored in nodes: $zyklusPcommitstoredstates_soll" > /dev/null
    zyklusPcommitblueedges=$?

    if [ $zyklusPcommitcontrol -ne 0 -o $zyklusPcommitbluenodes -ne 0 -o $zyklusPcommitblueedges -ne 0 -o $zyklusPcommitblueedges -ne 0 ]
    then
    echo   ... failed to build OG correctly
    fi

    result=`expr $result + $zyklusPcommitcontrol + $zyklusPcommitbluenodes + $zyklusPcommitblueedges + $zyklusPcommitblueedges`
fi

#############################################################################

echo

exit $result
