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

DIR=$testdir/countservices
FIONA=fiona

#loeschen aller erzeugten Dateien im letzten Durchlauf
rm -f $DIR/*.log

############################################################################

result=0

############################################################################
# acyclic                                                                  #
############################################################################
og="$DIR/acyclic.og"
cmd="$FIONA $og -t isacyclic"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$og.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
    echo $OUTPUT | grep "The given OG is acyclic" > /dev/null
    resultAcyclic=$?

    if [ $resultAcyclic -ne 0 ]; then
        result=1
        echo   ... acyclic OG is reported as cyclic.
    fi
fi

############################################################################
# cyclic                                                                   #
############################################################################
og="$DIR/cyclic.og"
cmd="$FIONA $og -t isacyclic"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$og.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
    echo $OUTPUT | grep "The given OG is NOT ayclic" > /dev/null
    resultCyclic=$?

    if [ $resultCyclic -ne 0 ]; then
        result=1
        echo   ... cyclic OG is reported as acyclic.
    fi
fi

############################################################################
# count services                                                           #
############################################################################
og="$DIR/1.og"
cmd="$FIONA $og -t count"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$og.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
    echo $OUTPUT | grep "Computed number of strategies: 5" > /dev/null
    resultCyclic=$?

    if [ $resultCyclic -ne 0 ]; then
        result=1
        echo   ... expected number of services differs.
    fi
fi

############################################################################
og="$DIR/nostrat.og"
cmd="$FIONA $og -t count"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$og.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
    echo $OUTPUT | grep "Computed number of strategies: 0" > /dev/null
    resultCyclic=$?

    if [ $resultCyclic -ne 0 ]; then
        result=1
        echo   ... expected number of services differs.
    fi
fi

echo

exit $result
