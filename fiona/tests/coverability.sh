#!/bin/bash

############################################################################
# Copyright 2008 Robert Danitz                                             #
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

DIR=$testdir/coverability
FIONA=fiona

# loeschen aller erzeugten Dateien im letzten Durchlauf
rm -f $DIR/*.out
rm -f $DIR/*.png
rm -f $DIR/*.log

result=0

###########################################################################
# erzeugen der og's

owfn="$DIR/Nc1.owfn"
cmd="$FIONA $owfn -t og -p cover -s empty"

if [ "$quiet" != "no" ]; then
    cmd="$cmd -p no-png"
fi

if [ "$memcheck" = "yes" ]; then
    memchecklog="$owfn.match.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
    if [ $? -ne 0 ]; then
        echo ... fiona exited with nonzero return value although it should not
        result=1
    fi

    echo $OUTPUT | grep "net is controllable: YES" > /dev/null
    if [ $? -ne 0 ]; then
        echo ... net is not controllable although it should! 
        result=1
    fi
fi

############################################################################

owfn="$DIR/Nc2.owfn"
cmd="$FIONA $owfn -t og -p cover -s empty"

if [ "$quiet" != "no" ]; then
    cmd="$cmd -p no-png"
fi

if [ "$memcheck" = "yes" ]; then
    memchecklog="$owfn.match.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
    if [ $? -ne 0 ]; then
        echo ... fiona exited with nonzero return value although it should not
        result=1
    fi

    echo $OUTPUT | grep "net is controllable: YES" > /dev/null
    if [ $? -ne 0 ]; then
        echo ... net is not controllable although it should! 
        result=1
    fi
fi

############################################################################

owfn="$DIR/Nc3.owfn"
cmd="$FIONA $owfn -t og -p cover -s empty"

if [ "$quiet" != "no" ]; then
    cmd="$cmd -p no-png"
fi

if [ "$memcheck" = "yes" ]; then
    memchecklog="$owfn.match.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
    if [ $? -ne 0 ]; then
        echo ... fiona exited with nonzero return value although it should not
        result=1
    fi

    echo $OUTPUT | grep "no_cover_internal" > /dev/null
    if [ $? -ne 0 ]; then
        echo ... net is controllable although it should not!
        result=1
    fi
fi

############################################################################

og="$DIR/Nc1.owfn.covog"

############################################################################

owfn="$DIR/M1.owfn"
cmd="$FIONA $owfn -t match -p cover $og"
if [ "$memcheck" = "yes" ]; then
    memchecklog="$owfn.match.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
    if [ $? -ne 0 ]; then
        echo ... fiona exited with nonzero return value although it should not
        result=1
    fi

    echo $OUTPUT | grep ": YES" > /dev/null
    if [ $? -ne 0 ]; then
        echo ... oWFN does not match with OG although it should
        result=1
    fi
fi

############################################################################

owfn="$DIR/M2.owfn"
cmd="$FIONA $owfn -t match -p cover $og"
if [ "$memcheck" = "yes" ]; then
    memchecklog="$owfn.match.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
    if [ $? -ne 0 ]; then
        echo ... fiona exited with nonzero return value although it should not
        result=1
    fi

    echo $OUTPUT | grep ": YES" > /dev/null
    if [ $? -ne 0 ]; then
        echo ... oWFN does not match with OG although it should
        result=1
    fi
fi

############################################################################

og="$DIR/Nc2.owfn.covog"

############################################################################

owfn="$DIR/M1.owfn"
cmd="$FIONA $owfn -t match -p cover $og"
if [ "$memcheck" = "yes" ]; then
    memchecklog="$owfn.match.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
    if [ $? -ne 0 ]; then
        echo ... fiona exited with nonzero return value although it should not
        result=1
    fi

    echo $OUTPUT | grep ": NO" > /dev/null
    if [ $? -ne 0 ]; then
        echo ... oWFN matches with OG although it should not
        result=1
    fi
fi

############################################################################

owfn="$DIR/M2.owfn"
cmd="$FIONA $owfn -t match -p cover $og"
if [ "$memcheck" = "yes" ]; then
    memchecklog="$owfn.match.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
    if [ $? -ne 0 ]; then
        echo ... fiona exited with nonzero return value although it should not
        result=1
    fi

    echo $OUTPUT | grep ": YES" > /dev/null
    if [ $? -ne 0 ]; then
        echo ... oWFN does not match with OG although it should
        result=1
    fi
fi

############################################################################
echo

exit $result
