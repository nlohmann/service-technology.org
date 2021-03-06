#!/usr/bin/env bash

source defaults.sh
source memcheck_helper.sh

echo
echo ---------------------------------------------------------------------
echo running $0
echo

DIR=$testdir/responsive
FIONA=fiona

#loeschen aller erzeugten Dateien im letzten Durchlauf
rm -f $DIR/*.out
rm -f $DIR/*.og
rm -f $DIR/*.log

############################################################################

result=0

owfn="$DIR/controllable_and_responsive_controllable.owfn"
cmd="$FIONA $owfn -t OG -p responsive"

if [ "$quiet" != "no" ]; then
cmd="$cmd -Q"
fi

if [ "$memcheck" = "yes" ]; then
	memchecklog="$owfn.IG.memcheck.log"
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
        resultOG=$?
        echo $OUTPUT | grep "number of blue nodes: 2" > /dev/null
        resultOGNOBN=$?
        echo $OUTPUT | grep "number of blue edges: 1" > /dev/null
        resultOGNOBE=$?
    
        if [ $resultOG -ne 0 -o $resultOGNOBN -ne 0 -o $resultOGNOBE -ne 0 ]
        then
            result=1
            echo   ... failed to build OG correctly
        fi
	fi
fi

owfn="$DIR/controllable_not_responsive_controllable.owfn"
cmd="$FIONA $owfn -t OG -p responsive"

if [ "$quiet" != "no" ]; then
cmd="$cmd -Q"
fi

if [ "$memcheck" = "yes" ]; then
	memchecklog="$owfn.IG.memcheck.log"
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
        echo $OUTPUT | grep "net is controllable: NO" > /dev/null
        resultOG=$?
        echo $OUTPUT | grep "number of blue nodes: 0" > /dev/null
        resultOGNOBN=$?
        echo $OUTPUT | grep "number of blue edges: 0" > /dev/null
        resultOGNOBE=$?
    
        if [ $resultOG -ne 0 -o $resultOGNOBN -ne 0 -o $resultOGNOBE -ne 0 ]
        then
            result=1
            echo   ... failed to build OG correctly
        fi
	fi
fi

owfn="$DIR/finalmarking_in_TSCC.owfn"
cmd="$FIONA $owfn -t OG -p responsive"

if [ "$quiet" != "no" ]; then
cmd="$cmd -Q"
fi

if [ "$memcheck" = "yes" ]; then
	memchecklog="$owfn.IG.memcheck.log"
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
        resultOG=$?
        echo $OUTPUT | grep "number of blue nodes: 2" > /dev/null
        resultOGNOBN=$?
        echo $OUTPUT | grep "number of blue edges: 1" > /dev/null
        resultOGNOBE=$?
    
        if [ $resultOG -ne 0 -o $resultOGNOBN -ne 0 -o $resultOGNOBE -ne 0 ]
        then
            result=1
            echo   ... failed to build OG correctly
        fi
	fi
fi

owfn="$DIR/2_TSCCs_in_1_node_with_communication_1.owfn"
cmd="$FIONA $owfn -t OG -p responsive"

if [ "$quiet" != "no" ]; then
cmd="$cmd -Q"
fi

if [ "$memcheck" = "yes" ]; then
	memchecklog="$owfn.IG.memcheck.log"
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
        resultOG=$?
        echo $OUTPUT | grep "number of blue nodes: 6" > /dev/null
        resultOGNOBN=$?
        echo $OUTPUT | grep "number of blue edges: 7" > /dev/null
        resultOGNOBE=$?
    
        if [ $resultOG -ne 0 -o $resultOGNOBN -ne 0 -o $resultOGNOBE -ne 0 ]
        then
            result=1
            echo   ... failed to build OG correctly
        fi
	fi
fi

owfn="$DIR/2_TSCCs_in_1_node_with_communication_2.owfn"
cmd="$FIONA $owfn -t OG -p responsive"

if [ "$quiet" != "no" ]; then
cmd="$cmd -Q"
fi

if [ "$memcheck" = "yes" ]; then
	memchecklog="$owfn.IG.memcheck.log"
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
        resultOG=$?
        echo $OUTPUT | grep "number of blue nodes: 18" > /dev/null
        resultOGNOBN=$?
        echo $OUTPUT | grep "number of blue edges: 27" > /dev/null
        resultOGNOBE=$?
    
        if [ $resultOG -ne 0 -o $resultOGNOBN -ne 0 -o $resultOGNOBE -ne 0 ]
        then
            result=1
            echo   ... failed to build OG correctly
        fi
	fi
fi


owfn="$DIR/cgvExample.owfn"
cmd="$FIONA $owfn -t OG -p responsive"

if [ "$quiet" != "no" ]; then
cmd="$cmd -Q"
fi

if [ "$memcheck" = "yes" ]; then
	memchecklog="$owfn.IG.memcheck.log"
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
        echo $OUTPUT | grep "net is controllable: NO" > /dev/null
        resultOG=$?
        echo $OUTPUT | grep "number of blue nodes: 0" > /dev/null
        resultOGNOBN=$?
        echo $OUTPUT | grep "number of blue edges: 0" > /dev/null
        resultOGNOBE=$?
    
        if [ $resultOG -ne 0 -o $resultOGNOBN -ne 0 -o $resultOGNOBE -ne 0 ]
        then
            result=1
            echo   ... failed to build OG correctly
        fi
	fi
fi

owfn="$DIR/controllable_and_responsive_controllable.owfn"
cmd="$FIONA $owfn -t IG -p responsive"

if [ "$quiet" != "no" ]; then
cmd="$cmd -Q"
fi

if [ "$memcheck" = "yes" ]; then
	memchecklIG="$owfn.IG.memcheck.lIG"
	do_memcheck "$cmd" "$memchecklIG"
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
        resultIG=$?
        echo $OUTPUT | grep "number of blue nodes: 2" > /dev/null
        resultIGNOBN=$?
        echo $OUTPUT | grep "number of blue edges: 1" > /dev/null
        resultIGNOBE=$?
    
        if [ $resultIG -ne 0 -o $resultIGNOBN -ne 0 -o $resultIGNOBE -ne 0 ]
        then
            result=1
            echo   ... failed to build IG correctly
        fi
	fi
fi

owfn="$DIR/controllable_not_responsive_controllable.owfn"
cmd="$FIONA $owfn -t IG -p responsive"

if [ "$quiet" != "no" ]; then
cmd="$cmd -Q"
fi

if [ "$memcheck" = "yes" ]; then
	memchecklIG="$owfn.IG.memcheck.lIG"
	do_memcheck "$cmd" "$memchecklIG"
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
        echo $OUTPUT | grep "net is controllable: NO" > /dev/null
        resultIG=$?
        echo $OUTPUT | grep "number of blue nodes: 0" > /dev/null
        resultIGNOBN=$?
        echo $OUTPUT | grep "number of blue edges: 0" > /dev/null
        resultIGNOBE=$?
    
        if [ $resultIG -ne 0 -o $resultIGNOBN -ne 0 -o $resultIGNOBE -ne 0 ]
        then
            result=1
            echo   ... failed to build IG correctly
        fi
	fi
fi

owfn="$DIR/finalmarking_in_TSCC.owfn"
cmd="$FIONA $owfn -t IG -p responsive"

if [ "$quiet" != "no" ]; then
cmd="$cmd -Q"
fi

if [ "$memcheck" = "yes" ]; then
	memchecklIG="$owfn.IG.memcheck.lIG"
	do_memcheck "$cmd" "$memchecklIG"
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
        resultIG=$?
        echo $OUTPUT | grep "number of blue nodes: 2" > /dev/null
        resultIGNOBN=$?
        echo $OUTPUT | grep "number of blue edges: 1" > /dev/null
        resultIGNOBE=$?
    
        if [ $resultIG -ne 0 -o $resultIGNOBN -ne 0 -o $resultIGNOBE -ne 0 ]
        then
            result=1
            echo   ... failed to build IG correctly
        fi
	fi
fi

owfn="$DIR/2_TSCCs_in_1_node_with_communication_1.owfn"
cmd="$FIONA $owfn -t IG -p responsive"

if [ "$quiet" != "no" ]; then
cmd="$cmd -Q"
fi

if [ "$memcheck" = "yes" ]; then
	memchecklIG="$owfn.IG.memcheck.lIG"
	do_memcheck "$cmd" "$memchecklIG"
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
        resultIG=$?
        echo $OUTPUT | grep "number of blue nodes: 6" > /dev/null
        resultIGNOBN=$?
        echo $OUTPUT | grep "number of blue edges: 7" > /dev/null
        resultIGNOBE=$?
    
        if [ $resultIG -ne 0 -o $resultIGNOBN -ne 0 -o $resultIGNOBE -ne 0 ]
        then
            result=1
            echo   ... failed to build IG correctly
        fi
	fi
fi

owfn="$DIR/2_TSCCs_in_1_node_with_communication_2.owfn"
cmd="$FIONA $owfn -t IG -p responsive"

if [ "$quiet" != "no" ]; then
cmd="$cmd -Q"
fi

if [ "$memcheck" = "yes" ]; then
	memchecklIG="$owfn.IG.memcheck.lIG"
	do_memcheck "$cmd" "$memchecklIG"
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
        resultIG=$?
        echo $OUTPUT | grep "number of blue nodes: 18" > /dev/null
        resultIGNOBN=$?
        echo $OUTPUT | grep "number of blue edges: 27" > /dev/null
        resultIGNOBE=$?
    
        if [ $resultIG -ne 0 -o $resultIGNOBN -ne 0 -o $resultIGNOBE -ne 0 ]
        then
            result=1
            echo   ... failed to build IG correctly
        fi
	fi
fi


owfn="$DIR/cgvExample.owfn"
cmd="$FIONA $owfn -t IG -p responsive"

if [ "$quiet" != "no" ]; then
cmd="$cmd -Q"
fi

if [ "$memcheck" = "yes" ]; then
	memchecklIG="$owfn.IG.memcheck.lIG"
	do_memcheck "$cmd" "$memchecklIG"
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
        echo $OUTPUT | grep "net is controllable: NO" > /dev/null
        resultIG=$?
        echo $OUTPUT | grep "number of blue nodes: 0" > /dev/null
        resultIGNOBN=$?
        echo $OUTPUT | grep "number of blue edges: 0" > /dev/null
        resultIGNOBE=$?
    
        if [ $resultIG -ne 0 -o $resultIGNOBN -ne 0 -o $resultIGNOBE -ne 0 ]
        then
            result=1
            echo   ... failed to build IG correctly
        fi
	fi
fi

owfn="$DIR/controllable_and_responsive_controllable.owfn"
cmd="$FIONA $owfn -t IG -r -p responsive"

if [ "$quiet" != "no" ]; then
cmd="$cmd -Q"
fi

if [ "$memcheck" = "yes" ]; then
	memchecklIG="$owfn.IG.memcheck.lIG"
	do_memcheck "$cmd" "$memchecklIG"
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
        resultIG=$?
        echo $OUTPUT | grep "number of blue nodes: 2" > /dev/null
        resultIGNOBN=$?
        echo $OUTPUT | grep "number of blue edges: 1" > /dev/null
        resultIGNOBE=$?
    
        if [ $resultIG -ne 0 -o $resultIGNOBN -ne 0 -o $resultIGNOBE -ne 0 ]
        then
            result=1
            echo   ... failed to build reduced IG correctly
        fi
	fi
fi


owfn="$DIR/controllable_not_responsive_controllable.owfn"
cmd="$FIONA $owfn -t IG -r -p responsive"

if [ "$quiet" != "no" ]; then
cmd="$cmd -Q"
fi

if [ "$memcheck" = "yes" ]; then
	memchecklIG="$owfn.IG.memcheck.lIG"
	do_memcheck "$cmd" "$memchecklIG"
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
        echo $OUTPUT | grep "net is controllable: NO" > /dev/null
        resultIG=$?
        echo $OUTPUT | grep "number of blue nodes: 0" > /dev/null
        resultIGNOBN=$?
        echo $OUTPUT | grep "number of blue edges: 0" > /dev/null
        resultIGNOBE=$?
    
        if [ $resultIG -ne 0 -o $resultIGNOBN -ne 0 -o $resultIGNOBE -ne 0 ]
        then
            result=1
            echo   ... failed to build reduced IG correctly
        fi
	fi
fi

owfn="$DIR/finalmarking_in_TSCC.owfn"
cmd="$FIONA $owfn -t IG -r -p responsive"

if [ "$quiet" != "no" ]; then
cmd="$cmd -Q"
fi

if [ "$memcheck" = "yes" ]; then
	memchecklIG="$owfn.IG.memcheck.lIG"
	do_memcheck "$cmd" "$memchecklIG"
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
        resultIG=$?
        echo $OUTPUT | grep "number of blue nodes: 2" > /dev/null
        resultIGNOBN=$?
        echo $OUTPUT | grep "number of blue edges: 1" > /dev/null
        resultIGNOBE=$?
    
        if [ $resultIG -ne 0 -o $resultIGNOBN -ne 0 -o $resultIGNOBE -ne 0 ]
        then
            result=1
            echo   ... failed to build reduced IG correctly
        fi
	fi
fi

owfn="$DIR/2_TSCCs_in_1_node_with_communication_1.owfn"
cmd="$FIONA $owfn -t IG -r -p responsive"

if [ "$quiet" != "no" ]; then
cmd="$cmd -Q"
fi

if [ "$memcheck" = "yes" ]; then
	memchecklIG="$owfn.IG.memcheck.lIG"
	do_memcheck "$cmd" "$memchecklIG"
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
        resultIG=$?
        echo $OUTPUT | grep "number of blue nodes: 4" > /dev/null
        resultIGNOBN=$?
        echo $OUTPUT | grep "number of blue edges: 3" > /dev/null
        resultIGNOBE=$?
    
        if [ $resultIG -ne 0 -o $resultIGNOBN -ne 0 -o $resultIGNOBE -ne 0 ]
        then
            result=1
            echo   ... failed to build reduced IG correctly
        fi
	fi
fi

owfn="$DIR/2_TSCCs_in_1_node_with_communication_2.owfn"
cmd="$FIONA $owfn -t IG -r -p responsive"

if [ "$quiet" != "no" ]; then
cmd="$cmd -Q"
fi

if [ "$memcheck" = "yes" ]; then
	memchecklIG="$owfn.IG.memcheck.lIG"
	do_memcheck "$cmd" "$memchecklIG"
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
        resultIG=$?
        echo $OUTPUT | grep "number of blue nodes: 8" > /dev/null
        resultIGNOBN=$?
        echo $OUTPUT | grep "number of blue edges: 7" > /dev/null
        resultIGNOBE=$?
    
        if [ $resultIG -ne 0 -o $resultIGNOBN -ne 0 -o $resultIGNOBE -ne 0 ]
        then
            result=1
            echo   ... failed to build reduced IG correctly
        fi
	fi
fi

owfn="$DIR/cgvExample.owfn"
cmd="$FIONA $owfn -t IG -r -p responsive"

if [ "$quiet" != "no" ]; then
cmd="$cmd -Q"
fi

if [ "$memcheck" = "yes" ]; then
	memchecklIG="$owfn.IG.memcheck.lIG"
	do_memcheck "$cmd" "$memchecklIG"
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
        echo $OUTPUT | grep "net is controllable: NO" > /dev/null
        resultIG=$?
        echo $OUTPUT | grep "number of blue nodes: 0" > /dev/null
        resultIGNOBN=$?
        echo $OUTPUT | grep "number of blue edges: 0" > /dev/null
        resultIGNOBE=$?
    
        if [ $resultIG -ne 0 -o $resultIGNOBN -ne 0 -o $resultIGNOBE -ne 0 ]
        then
            result=1
            echo   ... failed to build reduced IG correctly
        fi
	fi
fi

echo

exit $result
