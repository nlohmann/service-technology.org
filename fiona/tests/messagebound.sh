#!/usr/bin/env bash

source defaults.sh
source memcheck_helper.sh

echo
echo ---------------------------------------------------------------------
echo running $0
echo

DIR=$testdir/messagebound
FIONA=fiona

#loeschen aller erzeugten Dateien im letzten Durchlauf
rm -f $DIR/*.log

result=0

############################################################################

owfn="$DIR/messageboundtest1.owfn"
cmd="$FIONA $owfn -t OG -e10 -m4"

if [ "$quiet" != "no" ]; then
    cmd="$cmd -Q"
fi

if [ "$memcheck" = "yes" ]; then
    memchecklog="$owfn.e10.m4.OG.memcheck.log"
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
        mb14control=$?
    
        if [ $mb14control -ne 0 ]
        then
            echo   ... failed to build OG correctly
        fi
    
        result=`expr $mb14control`
    fi
fi

############################################################################

mb21bluenodes_soll=3
mb21blueedges_soll=2

owfn="$DIR/messageboundtest2.owfn"
cmd="$FIONA $owfn -t OG -e10 -m1"

if [ "$quiet" != "no" ]; then
    cmd="$cmd -Q"
fi

if [ "$memcheck" = "yes" ]; then
    memchecklog="$owfn.e10.m1.OG.memcheck.log"
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
        mb21control=$?
    
        echo $OUTPUT | grep "number of blue nodes: $mb21bluenodes_soll" > /dev/null
        mb21bluenodes=$?
    
        echo $OUTPUT | grep "number of blue edges: $mb21blueedges_soll" > /dev/null
        mb21blueedges=$?
    
        if [ $mb21control -ne 0 -o $mb21bluenodes -ne 0 -o $mb21blueedges -ne 0 ]
        then
            echo   ... failed to build OG correctly
        fi
    
        result=`expr $result + $mb21control + $mb21bluenodes + $mb21blueedges`
    fi
fi

############################################################################

mb22bluenodes_soll=5
mb22blueedges_soll=5

owfn="$DIR/messageboundtest2.owfn"
cmd="$FIONA $owfn -t OG -e10 -m2"

if [ "$quiet" != "no" ]; then
    cmd="$cmd -Q"
fi

if [ "$memcheck" = "yes" ]; then
    memchecklog="$owfn.e10.m2.OG.memcheck.log"
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
        mb22control=$?
    
        echo $OUTPUT | grep "number of blue nodes: $mb22bluenodes_soll" > /dev/null
        mb22bluenodes=$?
    
        echo $OUTPUT | grep "number of blue edges: $mb22blueedges_soll" > /dev/null
        mb22blueedges=$?
    
        if [ $mb22control -ne 0 -o $mb22bluenodes -ne 0 -o $mb22blueedges -ne 0 ]
        then
            echo   ... failed to build OG correctly
        fi
    
        result=`expr $result + $mb22control + $mb22bluenodes + $mb22blueedges`
    fi
fi

############################################################################

mb22bluenodes_soll=3
mb22blueedges_soll=2

owfn="$DIR/messageboundtest3.owfn"
cmd="$FIONA $owfn -t OG -m3"

if [ "$quiet" != "no" ]; then
    cmd="$cmd -Q"
fi

if [ "$memcheck" = "yes" ]; then
    memchecklog="$owfn.e10.m2.OG.memcheck.log"
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
        mb22control=$?
    
        echo $OUTPUT | grep "number of blue nodes: $mb22bluenodes_soll" > /dev/null
        mb22bluenodes=$?
    
        echo $OUTPUT | grep "number of blue edges: $mb22blueedges_soll" > /dev/null
        mb22blueedges=$?
    
        if [ $mb22control -ne 0 -o $mb22bluenodes -ne 0 -o $mb22blueedges -ne 0 ]
        then
            echo   ... failed to build OG correctly
        fi
    
        result=`expr $result + $mb22control + $mb22bluenodes + $mb22blueedges`
    fi
fi

############################################################################

mb22bluenodes_soll=4
mb22blueedges_soll=3

owfn="$DIR/messageboundtest4.owfn"
cmd="$FIONA $owfn -t OG -m3"

if [ "$quiet" != "no" ]; then
    cmd="$cmd -Q"
fi

if [ "$memcheck" = "yes" ]; then
    memchecklog="$owfn.e10.m2.OG.memcheck.log"
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
        mb22control=$?
    
        echo $OUTPUT | grep "number of blue nodes: $mb22bluenodes_soll" > /dev/null
        mb22bluenodes=$?
    
        echo $OUTPUT | grep "number of blue edges: $mb22blueedges_soll" > /dev/null
        mb22blueedges=$?
    
        if [ $mb22control -ne 0 -o $mb22bluenodes -ne 0 -o $mb22blueedges -ne 0 ]
        then
            echo   ... failed to build OG correctly
        fi
    
        result=`expr $result + $mb22control + $mb22bluenodes + $mb22blueedges`
    fi
fi

############################################################################

owfn="$DIR/messageboundtest5.owfn"
cmd="$FIONA $owfn -t OG -m2"

if [ "$quiet" != "no" ]; then
    cmd="$cmd -Q"
fi

if [ "$memcheck" = "yes" ]; then
    memchecklog="$owfn.e10.m2.OG.memcheck.log"
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
        mb22control=$?
    
        if [ $mb22control -ne 0 ]
        then
            echo   ... failed to build OG correctly
        fi
    
        result=`expr $result + $mb22control`
    fi
fi

############################################################################

owfn="$DIR/unlimited-communication.owfn"
cmd="$FIONA $owfn -t OG -m2"

if [ "$quiet" != "no" ]; then
    cmd="$cmd -Q"
fi

if [ "$memcheck" = "yes" ]; then
    memchecklog="$owfn.e10.m2.OG.memcheck.log"
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
        mb22control=$?
    
        if [ $mb22control -ne 0 ]
        then
            echo   ... failed to build OG correctly
        fi
    
        result=`expr $result + $mb22control`
    fi
fi

############################################################################

echo

exit $result
