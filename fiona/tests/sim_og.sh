#!/usr/bin/env bash

source defaults.sh
source memcheck_helper.sh

echo
echo ---------------------------------------------------------------------
echo running $0
echo

DIR=$testdir/sim_og
FIONA=fiona

#loeschen aller erzeugten Dateien im letzten Durchlauf
rm -f $DIR/*.log

############################################################################

result=0

og1="$DIR/cyclic1.og"
og2="$DIR/cyclic2.og"

cmd="$FIONA ${og1} ${og2} -t simulation"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$og1.1.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
    fionaExitCode=$?
    $evaluate $fionaExitCode
    if [ $? -ne 0 ] 
    then
        result=1
    else
        echo $OUTPUT | grep "The second OG characterizes at least one strategy that is" > /dev/null
        resultSIM=$?
        if [ $resultSIM -ne 0 ]; then
            let "result += 1"
            echo ... Simulation succeded, although it should not.
        fi
    fi
fi

#############################################################################

og1="$DIR/cyclic2.og"
og2="$DIR/cyclic1.og"

cmd="$FIONA ${og1} ${og2} -t simulation"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$og1.1.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
    
    fionaExitCode=$?
    $evaluate $fionaExitCode
    if [ $? -ne 0 ] 
    then
        result=1
    else
        echo $OUTPUT | grep "The first OG characterizes all strategies of the second one." > /dev/null
        resultSIM=$?
        if [ $resultSIM -ne 0 ]; then
            let "result += 1"
            echo ... Simulation failed, although it should not.
        fi
    fi
fi

#############################################################################

og1="$DIR/trafo_rule3_N7.owfn"
og2="$DIR/trafo_rule3_N8.owfn"

cmd="$FIONA ${og1} ${og2} -t equivalence"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$og1.3.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
    fionaExitCode=$?
    $evaluate $fionaExitCode
    if [ $? -ne 0 ] 
    then
        result=1
    else
        echo $OUTPUT | grep "are equivalent: YES" > /dev/null
        resultSIM=$?
        if [ $resultSIM -ne 0 ]; then
            let "result += 1"
            echo ... OGs determined not equivalent, although they are.
        fi
    fi
fi

#############################################################################

og1="$DIR/trafo_rule4_N9.owfn"
og2="$DIR/trafo_rule4_N10.owfn"

cmd="$FIONA ${og1} ${og2} -t equivalence"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$og1.3.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
    fionaExitCode=$?
    $evaluate $fionaExitCode
    if [ $? -ne 0 ] 
    then
        result=1
    else
        echo $OUTPUT | grep "are equivalent: YES" > /dev/null
        resultSIM=$?
        if [ $resultSIM -ne 0 ]; then
            let "result += 1"
            echo ... OGs determined not equivalent, although they are.
        fi
    fi
fi

#############################################################################

og1="$DIR/trafo_wsfm_M1.owfn"
og2="$DIR/trafo_wsfm_M0.owfn"

cmd="$FIONA ${og1} ${og2} -t simulation"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$og1.3.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
    fionaExitCode=$?
    $evaluate $fionaExitCode
    if [ $? -ne 0 ] 
    then
        result=1
    else
        echo $OUTPUT | grep "The first OG characterizes all strategies of the second one." > /dev/null
        resultSIM=$?
        if [ $resultSIM -ne 0 ]; then
            let "result += 1"
            echo ... OGs determined not equivalent, although they are.
        fi
    fi
fi

#############################################################################

og1="$DIR/trafo_wsfm_M2.owfn"
og2="$DIR/trafo_wsfm_M0.owfn"

cmd="$FIONA ${og1} ${og2} -t simulation"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$og1.3.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
    fionaExitCode=$?
    $evaluate $fionaExitCode
    if [ $? -ne 0 ] 
    then
        result=1
    else
        echo $OUTPUT | grep "The first OG characterizes all strategies of the second one." > /dev/null
        resultSIM=$?
        if [ $resultSIM -ne 0 ]; then
            let "result += 1"
            echo ... OGs determined not equivalent, although they are.
        fi
    fi
fi

#############################################################################

og1="$DIR/trafo_wsfm_M3.owfn"
og2="$DIR/trafo_wsfm_M0.owfn"

cmd="$FIONA ${og1} ${og2} -t simulation"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$og1.3.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
    fionaExitCode=$?
    $evaluate $fionaExitCode
    if [ $? -ne 0 ] 
    then
        result=1
    else
        echo $OUTPUT | grep "The first OG characterizes all strategies of the second one." > /dev/null
        resultSIM=$?
        if [ $resultSIM -ne 0 ]; then
            let "result += 1"
            echo ... OGs determined not equivalent, although they are.
        fi
    fi
fi

#############################################################################


og1="$DIR/coarse.og"
og2="$DIR/coarse-chopped.og"

cmd="$FIONA ${og1} ${og2} -t simulation"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$og1.1.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
    fionaExitCode=$?
    $evaluate $fionaExitCode
    if [ $? -ne 0 ] 
    then
        result=1
    else
        echo $OUTPUT | grep "The first OG characterizes all strategies of the second one." > /dev/null
        resultSIM=$?
        if [ $resultSIM -ne 0 ]; then
            let "result += 1"
            echo ... Simulation failed, although it should not.
        fi
    fi
fi

#############################################################################

og1="$DIR/coarse.og"
og2="$DIR/compact.og"

cmd="$FIONA ${og1} ${og2} -t equivalence"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$og1.3.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
    fionaExitCode=$?
    $evaluate $fionaExitCode
    if [ $? -ne 0 ] 
    then
        result=1
    else
        echo $OUTPUT | grep "are equivalent: YES" > /dev/null
        resultSIM=$?
        if [ $resultSIM -ne 0 ]; then
            let "result += 1"
            echo ... OGs determined not equivalent, although they are.
        fi
    fi
fi

############################################################################

echo

exit $result
