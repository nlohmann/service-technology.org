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

DIR=$testdir/matching
FIONA=fiona

#loeschen aller erzeugten Dateien im letzten Durchlauf
rm -f $DIR/*.log

result=0

og="$DIR/shop.og"

############################################################################

owfn="$DIR/client_match_1.owfn"
cmd="$FIONA $owfn -t match $og"
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

    echo $OUTPUT | grep "oWFN matches with OG: YES" > /dev/null
    if [ $? -ne 0 ]; then
        echo ... oWFN does not match with OG although it should
        result=1
    fi
fi

############################################################################

owfn="$DIR/client_nomatch_1.owfn"
cmd="$FIONA $owfn -t match $og"
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

    echo $OUTPUT | grep "oWFN matches with OG: NO" > /dev/null
    if [ $? -ne 0 ]; then
        echo ... oWFN matches with OG although it should not
        result=1
    fi
fi

############################################################################

owfn="$DIR/client_nomatch_2.owfn"
cmd="$FIONA $owfn -t match $og"
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

    echo $OUTPUT | grep "oWFN matches with OG: NO" > /dev/null
    if [ $? -ne 0 ]; then
        echo ... oWFN matches with OG although it should not
        result=1
    fi
fi

############################################################################

owfn="$DIR/client_nosupport_1.owfn"
cmd="$FIONA $owfn -t match $og"
if [ "$memcheck" = "yes" ]; then
    memchecklog="$owfn.match.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
    if [ $? -eq 0 ]; then
        echo ... fiona exited with zero return value although it should not
        result=1
    fi

    echo $OUTPUT | grep "ends or receives more than one message" > /dev/null
    if [ $? -ne 0 ]; then
        echo ... Parsing should have failed due to a transition reading or \
            writing more than one message, but it seems there was a different \
            reason.
        result=1
    fi
fi

############################################################################
og="$DIR/shop_rednodes.og"
############################################################################

owfn="$DIR/client_match_1.owfn"
cmd="$FIONA $owfn -t match $og"
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

    echo $OUTPUT | grep "oWFN matches with OG: NO" > /dev/null
    if [ $? -ne 0 ]; then
        echo ... oWFN matches with OG although it should not
        result=1
    fi
fi

############################################################################
og="$DIR/agency_traveler.og"
############################################################################

owfn="$DIR/airline.owfn"
cmd="$FIONA $owfn -t match $og"
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

    echo $OUTPUT | grep "oWFN matches with OG: NO" > /dev/null
    if [ $? -ne 0 ]; then
        echo ... oWFN does not match with OG although it should
        result=1
    fi
fi

############################################################################
og="$DIR/airline.og"
############################################################################

owfn="$DIR/agency_traveler.owfn"
cmd="$FIONA $owfn -t match $og"
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

    echo $OUTPUT | grep "oWFN matches with OG: NO" > /dev/null
    if [ $? -ne 0 ]; then
        echo ... oWFN does not match with OG although it should
        result=1
    fi
fi
############################################################################
echo

exit $result
