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

DIR=$testdir/sim_og
FIONA=fiona

#loeschen aller erzeugten Dateien im letzten Durchlauf
rm -f $DIR/*.out
rm -f $DIR/*.png
#rm -f $DIR/*.og
#rm -f $DIR/*.log

############################################################################

result=0

for i in 1 2;
do
    #echo running test$i

    owfn1="$DIR/test${i}1.owfn"
    owfn2="$DIR/test${i}2.owfn"

    #OUTPUT=`$FIONA $owfn1 -t OG`
    #OUTPUT=`$FIONA $owfn2 -t OG`

    cmd="$FIONA ${owfn1} ${owfn2} -t simulation"

    if [ "$memcheck" = "yes" ]; then
        memchecklog="$owfn1.memcheck.log"
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
done

for i in 3 4;
do
    #echo running test$i

    owfn1="$DIR/test${i}1.owfn"
    owfn2="$DIR/test${i}2.owfn"

    #OUTPUT=`$FIONA $owfn1 -t OG`
    #OUTPUT=`$FIONA $owfn2 -t OG`

    cmd="$FIONA ${owfn1} ${owfn2}.og -t simulation"

    if [ "$memcheck" = "yes" ]; then
        memchecklog="$owfn1.memcheck.log"
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
done

for i in 5 6;
do
    #echo running test$i

    owfn1="$DIR/test${i}1.owfn"
    owfn2="$DIR/test${i}2.owfn"

    #OUTPUT=`$FIONA $owfn1 -t OG`
    #OUTPUT=`$FIONA $owfn2 -t OG`

    cmd="$FIONA ${owfn1}.og ${owfn2} -t simulation"

    if [ "$memcheck" = "yes" ]; then
        memchecklog="$owfn1.memcheck.log"
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
done

for i in 8;
do
    #echo running test$i

    owfn1="$DIR/test${i}1.owfn"
    owfn2="$DIR/test${i}2.owfn"

    #OUTPUT=`$FIONA $owfn1 -t OG`
    #OUTPUT=`$FIONA $owfn2 -t OG`

    cmd="$FIONA ${owfn1}.og ${owfn2}.og -t simulation"

    if [ "$memcheck" = "yes" ]; then
        memchecklog="$owfn1.memcheck.log"
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
done

for i in 7 9;
do
    #echo running test$i

    owfn1="$DIR/test${i}1.owfn"
    owfn2="$DIR/test${i}2.owfn"

    #OUTPUT=`$FIONA $owfn1 -t OG`
    #OUTPUT=`$FIONA $owfn2 -t OG`

    cmd="$FIONA ${owfn1}.og ${owfn2}.og -t simulation"

    if [ "$memcheck" = "yes" ]; then
        memchecklog="$owfn1.memcheck.log"
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
done

#############################################################################

og1="$DIR/compact.og"
og2="$DIR/coarse.og"

cmd="$FIONA ${og1} ${og2} -t simulation"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$og1.1.memcheck.log"
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

og1="$DIR/coarse.og"
og2="$DIR/compact.og"

cmd="$FIONA ${og1} ${og2} -t simulation"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$og1.1.memcheck.log"
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

og1="$DIR/coarse.og"
og2="$DIR/coarse-chopped.og"

cmd="$FIONA ${og1} ${og2} -t simulation"

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

og1="$DIR/coarse-chopped.og"
og2="$DIR/coarse.og"

cmd="$FIONA ${og1} ${og2} -t simulation"

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

#############################################################################

og1="$DIR/compact.og"
og2="$DIR/coarse-chopped.og"

cmd="$FIONA ${og1} ${og2} -t simulation"

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

og1="$DIR/coarse-chopped.og"
og2="$DIR/compact.og"

cmd="$FIONA ${og1} ${og2} -t simulation"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$og1.2.memcheck.log"
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

#############################################################################

og1="$DIR/compact.og"
og2="$DIR/coarse.og"

cmd="$FIONA ${og1} ${og2} -t equivalence"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$og1.3.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
    echo $OUTPUT | grep "The two OGs characterize the same strategies." > /dev/null
    resultSIM=$?
    if [ $resultSIM -ne 0 ]; then
        let "result += 1"
        echo ... OGs determined not equivalent, although they are.
    fi
fi

#############################################################################

og1="$DIR/mixed.owfn"
og2="$DIR/mixed.owfn"

cmd="$FIONA ${og1}.og ${og2} -t equivalence"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$og1.3.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
    echo $OUTPUT | grep "The two OGs characterize the same strategies." > /dev/null
    resultSIM=$?
    if [ $resultSIM -ne 0 ]; then
        let "result += 1"
        echo ... OGs determined not equivalent, although they are.
    fi
fi

#############################################################################

og1="$DIR/mixed.owfn"
og2="$DIR/mixed.owfn"

cmd="$FIONA ${og1} ${og2}.og -t equivalence"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$og1.3.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
    echo $OUTPUT | grep "The two OGs characterize the same strategies." > /dev/null
    resultSIM=$?
    if [ $resultSIM -ne 0 ]; then
        let "result += 1"
        echo ... OGs determined not equivalent, although they are.
    fi
fi

#############################################################################

og1="$DIR/coarse.og"
og2="$DIR/coarse-chopped.og"

cmd="$FIONA ${og1} ${og2} -t equivalence"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$og1.3.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
    echo $OUTPUT | grep "characterizes all strategies of " > /dev/null
    resultSIM=$?
    if [ $resultSIM -ne 0 ]; then
        let "result += 1"
        echo ... first OG not found to simulate the second one, although it does.
    fi
fi

#############################################################################

og1="$DIR/compact.og"
og2="$DIR/coarse-chopped.og"

cmd="$FIONA ${og1} ${og2} -t equivalence"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$og1.4.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
    echo $OUTPUT | grep "characterizes all strategies of " > /dev/null
    resultSIM=$?
    if [ $resultSIM -ne 0 ]; then
        let "result += 1"
        echo ... first OG not found to simulate the second one, although it does.
    fi
fi

echo

exit $result
