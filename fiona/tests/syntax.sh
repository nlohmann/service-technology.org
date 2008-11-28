#!/bin/bash

source defaults.sh
source memcheck_helper.sh

echo
echo ---------------------------------------------------------------------
echo running $0
echo

SUBDIR=syntax
DIR=$testdir/$SUBDIR
FIONA=fiona

# for make distcheck: create directory $builddir/$SUBDIR for writing output files to
if [ "$testdir" != "$builddir" ]; then
    if [ ! -e $builddir/$SUBDIR ]; then
        $MKDIR_P $builddir/$SUBDIR
    fi
fi


#loeschen aller erzeugten Dateien im letzten Durchlauf
rm -f $DIR/formula.owfn.output.og
rm -f $DIR/formula.owfn.output.og.out
rm -f $DIR/max_occurrence.owfn.og
rm -f $DIR/max_occurrence.owfn.og.out
rm -f $DIR/*.log

result=0

############################################################################

maxoccurrencesbluenodes_soll=16
maxoccurrencesblueedges_soll=15
maxoccurrencesstoredstates_soll=73

owfn="$DIR/max_occurrence.owfn"
cmd="$FIONA $owfn -t OG -m5 -E -e 3"

if [ "$quiet" != "no" ]; then
    cmd="$cmd -Q"
fi

if [ "$memcheck" = "yes" ]; then
    memchecklog="$owfn.OG.memcheck.log"
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
        echo $OUTPUT | grep "net is controllable: YES" > /dev/null
        maxoccurrencescontrol=$?
    
        echo $OUTPUT | grep "number of blue nodes: $maxoccurrencesbluenodes_soll" > /dev/null
        maxoccurrencesbluenodes=$?
    
        echo $OUTPUT | grep "number of blue edges: $maxoccurrencesblueedges_soll" > /dev/null
        maxoccurrencesblueedges=$?
    
        echo $OUTPUT | grep "number of states stored in nodes: $maxoccurrencesstoredstates_soll" > /dev/null
        maxoccurrencesstoredstates=$?
    
        if [ $maxoccurrencescontrol -ne 0 -o $maxoccurrencesbluenodes -ne 0 -o $maxoccurrencesblueedges -ne 0 -o $maxoccurrencesstoredstates -ne 0 ]
        then
            echo   ... failed to build OG correctly
        fi
    
        result=`expr $result + $maxoccurrencescontrol + $maxoccurrencesbluenodes + $maxoccurrencesblueedges + $maxoccurrencesstoredstates`
    fi
fi

############################################################################

maxoccurrencesbluenodes_soll=21
maxoccurrencesblueedges_soll=20
maxoccurrencesstoredstates_soll=81

owfn="$DIR/max_occurrence.owfn"
cmd="$FIONA $owfn -t OG -m5 -e -1"

if [ "$quiet" != "no" ]; then
    cmd="$cmd -Q"
fi

if [ "$memcheck" = "yes" ]; then
    memchecklog="$owfn.OG.memcheck.log"
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
        echo $OUTPUT | grep "net is controllable: YES" > /dev/null
        maxoccurrencescontrol=$?
    
        echo $OUTPUT | grep "number of blue nodes: $maxoccurrencesbluenodes_soll" > /dev/null
        maxoccurrencesbluenodes=$?
    
        echo $OUTPUT | grep "number of blue edges: $maxoccurrencesblueedges_soll" > /dev/null
        maxoccurrencesblueedges=$?
    
        echo $OUTPUT | grep "number of states stored in nodes: $maxoccurrencesstoredstates_soll" > /dev/null
        maxoccurrencesstoredstates=$?
    
        if [ $maxoccurrencescontrol -ne 0 -o $maxoccurrencesbluenodes -ne 0 -o $maxoccurrencesblueedges -ne 0 -o $maxoccurrencesstoredstates -ne 0 ]
        then
          echo   ... failed to build OG correctly
        fi
    
        result=`expr $result + $maxoccurrencescontrol + $maxoccurrencesbluenodes + $maxoccurrencesblueedges + $maxoccurrencesstoredstates`
    fi
fi

############################################################################

owfn="$DIR/syntax_example.owfn"
cmd="$FIONA $owfn -t IG"

if [ "$quiet" != "no" ]; then
    cmd="$cmd -Q"
fi

if [ "$memcheck" = "yes" ]; then
    memchecklog="$owfn.OG.memcheck.log"
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
        echo $OUTPUT | grep "net is controllable: YES" > /dev/null
        syntaxexample=$?
    
    
        if [ $syntaxexample -ne 0 ]
        then
        echo   ... failed to build OG correctly
        fi
    
        result=`expr $result + $syntaxexample`
    fi
fi

############################################################################

owfn="$DIR/syntax_example_new.owfn"
cmd="$FIONA $owfn -t IG"

if [ "$quiet" != "no" ]; then
    cmd="$cmd -Q"
fi

if [ "$memcheck" = "yes" ]; then
    memchecklog="$owfn.OG.memcheck.log"
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
        echo $OUTPUT | grep "net is controllable: YES" > /dev/null
        syntaxexamplenew=$?
    
        if [ $syntaxexamplenew -ne 0 ]
        then
        echo   ... failed to build OG correctly
        fi
    
        result=`expr $result + $syntaxexamplenew`
    fi
fi

############################################################################

owfn="$DIR/syntax_example_new_short.owfn"
cmd="$FIONA $owfn -t IG"

if [ "$quiet" != "no" ]; then
    cmd="$cmd -Q"
fi

if [ "$memcheck" = "yes" ]; then
    memchecklog="$owfn.OG.memcheck.log"
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
        echo $OUTPUT | grep "net is controllable: YES" > /dev/null
        syntaxexamplenew=$?
    
        if [ $syntaxexamplenew -ne 0 ]
        then
        echo   ... failed to build OG correctly
        fi
    
        result=`expr $result + $syntaxexamplenew`
    fi
fi

############################################################################
resultSingle=0
owfn="$DIR/formula.owfn"
outputPrefix="$builddir/syntax/formula.owfn.output"
outputExpected="$testdir/syntax/formula.owfn.expected.og"

cmd="$FIONA $owfn -t og -o $outputPrefix -p no-dot"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$outputPrefix.memcheck.log"
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
        echo "running diff -b $outputPrefix.og $outputExpected"
        if ! diff -b "$outputPrefix.og" "$outputExpected" >/dev/null ; then
            echo "... failed: Output and expected output differ. Compare " \
                 "$outputPrefix.og" "$outputExpected"
            resultSingle=1
        fi
    fi
fi

if [ $resultSingle -ne 0 ]; then
    result=1
fi

############################################################################

echo

exit $result

