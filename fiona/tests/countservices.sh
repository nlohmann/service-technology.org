#!/bin/bash

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

    fionaExitCode=$?
    $evaluate $fionaExitCode
    if [ $? -ne 0 ] 
    then
        result=1
    else
        echo $OUTPUT | grep "The given OG is acyclic" > /dev/null
        resultAcyclic=$?
    
        if [ $resultAcyclic -ne 0 ]; then
            result=1
            echo   ... acyclic OG is reported as cyclic.
        fi
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
    
    fionaExitCode=$?
    $evaluate $fionaExitCode
    if [ $? -ne 0 ] 
    then
        result=1
    else
        echo $OUTPUT | grep "The given OG is NOT ayclic" > /dev/null
        resultCyclic=$?
    
        if [ $resultCyclic -ne 0 ]; then
            result=1
            echo   ... cyclic OG is reported as acyclic.
        fi
    fi
fi

############################################################################
# count services                                                           #
############################################################################
og="$DIR/false_nodes.og"
cmd="$FIONA $og -t count"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$og.memcheck.log"
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
        echo $OUTPUT | grep "Computed number of matching tree service automata: 5" > /dev/null
        resultCyclic=$?
    
        if [ $resultCyclic -ne 0 ]; then
            result=1
            echo   ... expected number of services differs.
        fi
    fi
fi

############################################################################
og="$DIR/multiple_arcs.og"
cmd="$FIONA $og -t count"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$og.memcheck.log"
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
        echo $OUTPUT | grep "Computed number of matching tree service automata: 9" > /dev/null
        resultCyclic=$?
    
        if [ $resultCyclic -ne 0 ]; then
            result=1
            echo   ... expected number of services differs.
        fi
    fi
fi

############################################################################
og="$DIR/sequence4.owfn.og"
cmd="$FIONA $og -t count"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$og.memcheck.log"
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
        echo $OUTPUT | grep "Computed number of matching tree service automata: 1.67772e+07" > /dev/null
        resultCyclic=$?
    
        if [ $resultCyclic -ne 0 ]; then
            result=1
            echo   ... expected number of services differs.
        fi
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
    
    fionaExitCode=$?
    $evaluate $fionaExitCode
    if [ $? -ne 0 ] 
    then
        result=1
    else
        echo $OUTPUT | grep "The computed number resulted 0, which implies that the service is not controllable." > /dev/null
        resultCyclic=$?
        if [ $resultCyclic -ne 0 ]; then
            result=1
            echo   ... expected number of services differs.
        fi
    fi
fi

echo

exit $result
