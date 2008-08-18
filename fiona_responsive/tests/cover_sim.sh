#!/bin/bash

############################################################################
# Copyright 2005 - 2007 Peter Massuthe, Daniela Weinberg, Dennis Reinert,  #
#                    Jan Bretschneider, Christian Gierds and Robert Danitz #
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

#loeschen aller erzeugten Dateien im letzten Durchlauf
rm -f $DIR/*.log

result=0

############################################################################


og1="$DIR/cov1.og"
og2="$DIR/cov2.og"

cmd="$FIONA ${og1} ${og2} -t cover_simulation"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$og1.1.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
    echo $OUTPUT | grep "The first OG characterizes all strategies of the second one while covering all external transitions." > /dev/null
    resultSIM=$?
    if [ $resultSIM -ne 0 ]; then
        let "result += 1"
        echo ... Simulation failed, although it should not.
    fi
fi

#############################################################################

og1="$DIR/cov2.og"
og2="$DIR/cov1.og"

cmd="$FIONA ${og1} ${og2} -t cover_simulation"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$og1.1.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
    echo $OUTPUT | grep "The first OG characterizes all strategies of the second one while covering all external transitions." > /dev/null
    resultSIM=$?
    if [ $resultSIM -ne 0 ]; then
        let "result += 1"
        echo ... Simulation failed, although it should not.
    fi
fi

#############################################################################

og1="$DIR/cov3.og"
og2="$DIR/cov4.og"

cmd="$FIONA ${og1} ${og2} -t cover_simulation"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$og1.2.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
    echo $OUTPUT | grep "The first OG characterizes all strategies of the second one." > /dev/null
    resultSIM=$?
    if [ $resultSIM -ne 0 ]; then
        let "result += 1"
        echo ... Simulation failed, although it should not.
    fi
fi

#############################################################################

og1="$DIR/cov4.og"
og2="$DIR/cov3.og"

cmd="$FIONA ${og1} ${og2} -t cover_simulation"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$og1.1.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
    echo $OUTPUT | grep "The second OG characterizes at least one strategy that is" > /dev/null
    resultSIM=$?
    if [ $resultSIM -ne 0 ]; then
        let "result += 1"
        echo ... Simulation succeded, although it should not.
    fi
fi

echo

exit $result
