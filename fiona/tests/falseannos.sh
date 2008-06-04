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

DIR=$testdir/falseannos
FIONA=fiona

#loeschen aller erzeugten Dateien im letzten Durchlauf
rm -f $DIR/*.blue.og
rm -f $DIR/*.log

result=0



############################################################################
# -t removefalsenodes 1
############################################################################

violating="$DIR/violating"

cleanedPerHand="$DIR/cleaned.og"

cmd="$FIONA $violating.og -t removefalsenodes"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$violating.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
fi

cmd="diff $violating.blue.og $cleanedPerHand "


if [ "$memcheck" = "yes" ]; then
    memchecklog="$cleanedPerHand.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`

    if [ "$OUTPUT" != "" ]; then
        let "result += 1"
        echo ... automatically cleaned OG is not equivalent to manually cleaned OG.
    fi
fi

############################################################################
# -t removefalsenodes 2
############################################################################

violating="$DIR/bluefalsenode"

cleanedPerHand="$DIR/bluefalsenode.expected.og"

cmd="$FIONA $violating.og -t removefalsenodes"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$violating.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
fi

cmd="diff $violating.blue.og $cleanedPerHand "


if [ "$memcheck" = "yes" ]; then
    memchecklog="$cleanedPerHand.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`

    if [ "$OUTPUT" != "" ]; then
        let "result += 1"
        echo ... automatically cleaned OG is not equivalent to manually cleaned OG.
    fi
fi

############################################################################
# -t removefalsenodes shorten annotations
############################################################################

violating="$DIR/shorten"

cleanedPerHand="$DIR/shorten.expected.og"

cmd="$FIONA $violating.og -t removefalsenodes"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$violating.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
fi

cmd="diff $violating.blue.og $cleanedPerHand "


if [ "$memcheck" = "yes" ]; then
    memchecklog="$cleanedPerHand.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`

    if [ "$OUTPUT" != "" ]; then
        let "result += 1"
        echo ... automatically cleaned OG is not equivalent to manually cleaned OG.
    fi
fi

############################################################################
# -t checkfalsenodes
############################################################################


violating="$DIR/2ndviolating"
cmd="$FIONA $violating.og -t removefalsenodes"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$violating.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`

    cmd="$FIONA $violating.blue.og -t checkfalsenodes"

    echo running $cmd
    OUTPUT=`$cmd 2>&1`

    echo $OUTPUT | grep "No nodes with false annotation found" > /dev/null
    ok=$?

    if [ $ok -ne 0 ]
    then
    echo   ... false annotated nodes found after removal.
    fi

    result=`expr $result + $ok`
fi

############################################################################

echo

exit $result
