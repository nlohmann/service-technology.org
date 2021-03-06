#!/usr/bin/env bash

source defaults.sh
source memcheck_helper.sh

echo
echo ---------------------------------------------------------------------
echo running $0
echo

#loeschen aller erzeugten Dateien im letzten Durchlauf
rm -f $DIR/*.log

############################################################################
# OG
############################################################################

DIR=$testdir/samples
FIONA=fiona

result=0
i=9

    owfn="$DIR/sequence${i}.owfn"
    cmd="$FIONA $owfn -t IG"

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
            resultIG=$?
            echo $OUTPUT | grep "number of states stored in nodes: $(((i+1)*(i+2)/2))" > /dev/null
            resultIGNOSC=$?
            echo $OUTPUT | grep "number of nodes: $((i+1))" > /dev/null
            resultIGNON=$?
            echo $OUTPUT | grep "number of edges: $i" > /dev/null
            resultIGNOE=$?
            echo $OUTPUT | grep "number of blue nodes: $((i+1))" > /dev/null
            resultIGNOBN=$?
            echo $OUTPUT | grep "number of blue edges: $i" > /dev/null
            resultIGNOBE=$?
    
            if [ $resultIG -ne 0 -o $resultIGNOSC -ne 0 -o $resultIGNON -ne 0 -o $resultIGNOE -ne 0 -o $resultIGNOBN -ne 0 -o $resultIGNOBE -ne 0 ]
            then
                result=1
                echo   ... failed to build IG correctly
            fi
        fi
    fi
    

    cmd="$FIONA $owfn -t OG"

    if [ "$quiet" != "no" ]; then
        cmd="$cmd -Q"
    fi

    if [ "$memcheck" = "yes" ]; then
        memchecklog="$owfn.OG.memcheck.log"
        do_memcheck "$cmd" "$memchecklog"
        result=$(($result | $?))
    else
        echo running $cmd
        OUTPUT=`$cmd  2>&1`
        fionaExitCode=$?
        $evaluate $fionaExitCode
        if [ $? -ne 0 ] 
        then
            result=1
        else
            echo $OUTPUT | grep "net is controllable: YES" > /dev/null
            resultOG=$?
            echo $OUTPUT | grep "number of states stored in nodes: $(((2**(i+1))-1))" > /dev/null
            resultOGNOSC=$?
            echo $OUTPUT | grep "number of nodes: $((2**i))" > /dev/null
            resultOGNON=$?
            echo $OUTPUT | grep "number of edges: $((i*(2**(i-1))))" > /dev/null
            resultOGNOE=$?
            echo $OUTPUT | grep "number of blue nodes: $((2**i))" > /dev/null
            resultOGNOBN=$?
            echo $OUTPUT | grep "number of blue edges: $((i*(2**(i-1))))" > /dev/null
            resultOGNOBE=$?
            if [ $resultOG -ne 0 -o $resultOGNOSC -ne 0 -o $resultOGNON -ne 0 -o $resultOGNOE -ne 0 -o $resultOGNOBN -ne 0 -o $resultOGNOBE -ne 0 ]
            then
                result=1
                echo   ... failed to build OG
            fi
        fi
    fi


############################################################################
# OG
############################################################################

mycoffeebluenodes_soll=7
mycoffeeblueedges_soll=9
mycoffeestoredstates_soll=32

owfn="$DIR/myCoffee.owfn"
cmd="$FIONA $owfn -t OG"

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
        mycoffeecontrol=$?
    
        echo $OUTPUT | grep "number of blue nodes: $mycoffeebluenodes_soll" > /dev/null
        mycoffeebluenodes=$?
    
        echo $OUTPUT | grep "number of blue edges: $mycoffeeblueedges_soll" > /dev/null
        mycoffeeblueedges=$?
    
        echo $OUTPUT | grep "number of states stored in nodes: $mycoffeestoredstates_soll" > /dev/null
        mycoffeestoredstates=$?
    
        if [ $mycoffeecontrol -ne 0 -o $mycoffeebluenodes -ne 0 -o $mycoffeeblueedges -ne 0 -o $mycoffeestoredstates -ne 0 ]
        then
        echo   ... failed to build OG correctly
        fi
    
        result=`expr $result + $mycoffeecontrol + $mycoffeebluenodes + $mycoffeeblueedges + $mycoffeestoredstates`
    fi
fi


############################################################################
# IG
############################################################################

mycoffeebluenodes_soll=5
mycoffeeblueedges_soll=5
mycoffeestoredstates_soll=12

owfn="$DIR/myCoffee.owfn"
cmd="$FIONA $owfn -t IG"

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
        mycoffeecontrol=$?
    
        echo $OUTPUT | grep "number of blue nodes: $mycoffeebluenodes_soll" > /dev/null
        mycoffeebluenodes=$?
    
        echo $OUTPUT | grep "number of blue edges: $mycoffeeblueedges_soll" > /dev/null
        mycoffeeblueedges=$?
    
        echo $OUTPUT | grep "number of states stored in nodes: $mycoffeestoredstates_soll" > /dev/null
        mycoffeestoredstates=$?
    
        if [ $mycoffeecontrol -ne 0 -o $mycoffeebluenodes -ne 0 -o $mycoffeeblueedges -ne 0 -o $mycoffeestoredstates -ne 0 ]
        then
            echo   ... failed to build IG correctly
        fi
    
        result=`expr $result + $mycoffeecontrol + $mycoffeebluenodes + $mycoffeeblueedges + $mycoffeestoredstates`
    fi
fi

############################################################################
# reduced IG
############################################################################

mycoffeebluenodes_soll=5
mycoffeeblueedges_soll=5
mycoffeestoredstates_soll=12

owfn="$DIR/myCoffee.owfn"
cmd="$FIONA $owfn -r -t IG"

if [ "$quiet" != "no" ]; then
    cmd="$cmd -Q"
fi

if [ "$memcheck" = "yes" ]; then
    memchecklog="$owfn.r.IG.memcheck.log"
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
        mycoffeecontrol=$?
    
        echo $OUTPUT | grep "number of blue nodes: $mycoffeebluenodes_soll" > /dev/null
        mycoffeebluenodes=$?
    
        echo $OUTPUT | grep "number of blue edges: $mycoffeeblueedges_soll" > /dev/null
        mycoffeeblueedges=$?
    
        echo $OUTPUT | grep "number of states stored in nodes: $mycoffeestoredstates_soll" > /dev/null
        mycoffeestoredstates=$?
    
        if [ $mycoffeecontrol -ne 0 -o $mycoffeebluenodes -ne 0 -o $mycoffeeblueedges -ne 0 -o $mycoffeestoredstates -ne 0 ]
        then
            echo   ... failed to build reduced IG correctly
        fi
    
        result=`expr $result + $mycoffeecontrol + $mycoffeebluenodes + $mycoffeeblueedges + $mycoffeestoredstates`
    fi
fi


############################################################################

keescoffee1bluenodes_soll=9
keescoffee1blueedges_soll=13
keescoffee1storedstates_soll=100

owfn="$DIR/keesCoffee_condition.owfn"
cmd="$FIONA $owfn -t OG -e 1"

if [ "$quiet" != "no" ]; then
    cmd="$cmd -Q"
fi

if [ "$memcheck" = "yes" ]; then
    memchecklog="$owfn.e1.OG.memcheck.log"
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
        keescoffee1control=$?
    
        echo $OUTPUT | grep "number of blue nodes: $keescoffee1bluenodes_soll" > /dev/null
        keescoffee1bluenodes=$?
    
        echo $OUTPUT | grep "number of blue edges: $keescoffee1blueedges_soll" > /dev/null
        keescoffee1blueedges=$?
    
        echo $OUTPUT | grep "number of states stored in nodes: $keescoffee1storedstates_soll" > /dev/null
        keescoffee1storedstates=$?
    
        if [ $keescoffee1control -ne 0 -o $keescoffee1bluenodes -ne 0 -o $keescoffee1blueedges -ne 0 -o $keescoffee1storedstates -ne 0 ]
        then
            echo   ... failed to build OG correctly
        fi
    
        result=`expr $result + $keescoffee1control + $keescoffee1bluenodes + $keescoffee1blueedges + $keescoffee1storedstates`
    fi
fi

############################################################################

keescoffee2bluenodes_soll=20
keescoffee2blueedges_soll=41
keescoffee2storedstates_soll=212

owfn="$DIR/keesCoffee_condition.owfn"
cmd="$FIONA $owfn -t OG -e 3 -m 1"

if [ "$quiet" != "no" ]; then
    cmd="$cmd -Q"
fi

if [ "$memcheck" = "yes" ]; then
    memchecklog="$owfn.e2.OG.memcheck.log"
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
        keescoffee2control=$?
    
        echo $OUTPUT | grep "number of blue nodes: $keescoffee2bluenodes_soll" > /dev/null
        keescoffee2bluenodes=$?
    
        echo $OUTPUT | grep "number of blue edges: $keescoffee2blueedges_soll" > /dev/null
        keescoffee2blueedges=$?
    
        echo $OUTPUT | grep "number of states stored in nodes: $keescoffee2storedstates_soll" > /dev/null
        keescoffee2storedstates=$?
    
        if [ $keescoffee2control -ne 0 -o $keescoffee2bluenodes -ne 0 -o $keescoffee2blueedges -ne 0 -o $keescoffee2storedstates -ne 0 ]
        then
            echo   ... failed to build OG correctly
        fi
    
        result=`expr $result + $keescoffee2control + $keescoffee2bluenodes + $keescoffee2blueedges + $keescoffee2storedstates`
    fi
fi

############################################################################
# OG
############################################################################

shop3bluenodes_soll=12
shop3blueedges_soll=15
shop3storedstates_soll=4530

owfn="$DIR/06-03-23_BPM06_shop_sect_3.owfn"
cmd="$FIONA $owfn -t OG"

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
        shop3control=$?
    
        echo $OUTPUT | grep "number of blue nodes: $shop3bluenodes_soll" > /dev/null
        shop3bluenodes=$?
    
        echo $OUTPUT | grep "number of blue edges: $shop3blueedges_soll" > /dev/null
        shop3blueedges=$?
    
        echo $OUTPUT | grep "number of states stored in nodes: $shop3storedstates_soll" > /dev/null
        shop3storedstates=$?
    
        if [ $shop3control -ne 0 -o $shop3bluenodes -ne 0 -o $shop3blueedges -ne 0 -o $shop3storedstates -ne 0 ]
        then
            echo   ... failed to build OG correctly
        fi
    
        result=`expr $result + $shop3control + $shop3bluenodes + $shop3blueedges + $shop3storedstates`
    fi
fi


############################################################################
# IG
############################################################################

shop3bluenodes_soll=11
shop3blueedges_soll=13
shop3storedstates_soll=657 #687 #728

owfn="$DIR/06-03-23_BPM06_shop_sect_3.owfn"
cmd="$FIONA $owfn -t IG"

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
        shop3control=$?
    
        echo $OUTPUT | grep "number of blue nodes: $shop3bluenodes_soll" > /dev/null
        shop3bluenodes=$?
    
        echo $OUTPUT | grep "number of blue edges: $shop3blueedges_soll" > /dev/null
        shop3blueedges=$?
    
        echo $OUTPUT | grep "number of states stored in nodes: $shop3storedstates_soll" > /dev/null
        shop3storedstates=$?
    
        if [ $shop3control -ne 0 -o $shop3bluenodes -ne 0 -o $shop3blueedges -ne 0 -o $shop3storedstates -ne 0 ]
        then
            echo   ... failed to build the IG correctly
        fi
    
        result=`expr $result + $shop3control + $shop3bluenodes + $shop3blueedges + $shop3storedstates`
    fi
fi


############################################################################
# IG reduced
############################################################################

shop3bluenodes_soll=8 #7
shop3blueedges_soll=8 #6
shop3storedstates_soll=351 #329

owfn="$DIR/06-03-23_BPM06_shop_sect_3.owfn"
cmd="$FIONA $owfn -t IG -r"

if [ "$quiet" != "no" ]; then
    cmd="$cmd -Q"
fi

if [ "$memcheck" = "yes" ]; then
    memchecklog="$owfn.r.IG.memcheck.log"
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
        shop3control=$?
    
        echo $OUTPUT | grep "number of blue nodes: $shop3bluenodes_soll" > /dev/null
        shop3bluenodes=$?
    
        echo $OUTPUT | grep "number of blue edges: $shop3blueedges_soll" > /dev/null
        shop3blueedges=$?
    
        echo $OUTPUT | grep "number of states stored in nodes: $shop3storedstates_soll" > /dev/null
        shop3storedstates=$?
    
        if [ $shop3control -ne 0 -o $shop3bluenodes -ne 0 -o $shop3blueedges -ne 0 -o $shop3storedstates -ne 0 ]
        then
            echo   ... failed to build the reduced IG correctly
        fi
    
        result=`expr $result + $shop3control + $shop3bluenodes + $shop3blueedges + $shop3storedstates`
    fi
fi


############################################################################
# OG
############################################################################

shop6bluenodes_soll=7
shop6blueedges_soll=7
shop6storedstates_soll=6932

owfn="$DIR/06-03-23_BPM06_shop_sect_6.owfn"
cmd="$FIONA $owfn -t OG"

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
        shop6control=$?
    
        echo $OUTPUT | grep "number of blue nodes: $shop6bluenodes_soll" > /dev/null
        shop6bluenodes=$?
    
        echo $OUTPUT | grep "number of blue edges: $shop6blueedges_soll" > /dev/null
        shop6blueedges=$?
    
        echo $OUTPUT | grep "number of states stored in nodes: $shop6storedstates_soll" > /dev/null
        shop6storedstates=$?
    
        if [ $shop6control -ne 0 -o $shop6bluenodes -ne 0 -o $shop6blueedges -ne 0 -o $shop6storedstates -ne 0 ]
        then
            echo   ... failed to build OG correctly
        fi
    
        result=`expr $result + $shop6control + $shop6bluenodes + $shop6blueedges + $shop6storedstates`
    fi
fi


############################################################################
# IG
############################################################################

shop6bluenodes_soll=6
shop6blueedges_soll=5
#shop6storedstates_soll=2491 --> try new number of states:
#shop6storedstates_soll=4027

owfn="$DIR/06-03-23_BPM06_shop_sect_6.owfn"
cmd="$FIONA $owfn -t IG"

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
        shop6control=$?
    
        echo $OUTPUT | grep "number of blue nodes: $shop6bluenodes_soll" > /dev/null
        shop6bluenodes=$?
    
        echo $OUTPUT | grep "number of blue edges: $shop6blueedges_soll" > /dev/null
        shop6blueedges=$?
    
        echo $OUTPUT | grep "number of states stored in nodes: $shop6storedstates_soll" > /dev/null
        shop6storedstates=$?
    
        if [ $shop6control -ne 0 -o $shop6bluenodes -ne 0 -o $shop6blueedges -ne 0 ]
        #-o $shop6storedstates -ne 0 ]
        then
            echo   ... failed to build IG correctly
        fi
    
        result=`expr $result + $shop6control + $shop6bluenodes + $shop6blueedges` # + $shop6storedstates`
    fi
fi


############################################################################
# reduced IG
############################################################################

shop6bluenodes_soll=6
shop6blueedges_soll=5
shop6storedstates_soll=1109   #1087

owfn="$DIR/06-03-23_BPM06_shop_sect_6.owfn"
cmd="$FIONA $owfn -t IG -r"

if [ "$quiet" != "no" ]; then
    cmd="$cmd -Q"
fi

if [ "$memcheck" = "yes" ]; then
    memchecklog="$owfn.rR.IG.memcheck.log"
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
        shop6control=$?
    
        echo $OUTPUT | grep "number of blue nodes: $shop6bluenodes_soll" > /dev/null
        shop6bluenodes=$?
    
        echo $OUTPUT | grep "number of blue edges: $shop6blueedges_soll" > /dev/null
        shop6blueedges=$?
    
        echo $OUTPUT | grep "number of states stored in nodes: $shop6storedstates_soll" > /dev/null
        shop6storedstates=$?
    
        if [ $shop6control -ne 0 -o $shop6bluenodes -ne 0 -o $shop6blueedges -ne 0 -o $shop6storedstates -ne 0 ]
        then
	        echo   ... failed to build reduced IG correctly
        fi
    
        result=`expr $result + $shop6control + $shop6bluenodes + $shop6blueedges + $shop6storedstates`
    fi
fi

############################################################################

############################################################################
# OG
############################################################################
# DKE-Shops brauchen zu lang (ca. 60 bzw 80 Sekunden)
#
#dkeshopbluenodes_soll=83
#dkeshopblueedges_soll=176
#dkeshopstoredstates_soll=97688
#
#dkeshopbluenodes=0
#dkeshopblueedges=0
#dkeshopstoredstates=0
#
#owfn="$DIR/dke07_shop_sect_3.owfn"
#cmd="$FIONA $owfn -t OG -e 1"
#
#if [ "$quiet" != "no" ]; then
#    cmd="$cmd -Q"
#fi
#
#if [ "$memcheck" = "yes" ]; then
#    memchecklog="$owfn.OG.memcheck.log"
#    do_memcheck "$cmd" "$memchecklog"
#    result=$(($result | $?))
#else
#    echo running $cmd
#    OUTPUT=`$cmd 2>&1`
#
#    echo $OUTPUT | grep "net is controllable: YES" > /dev/null
#    dkeshopcontrol=$?
#
#    echo $OUTPUT | grep "number of blue nodes: $dkeshopbluenodes_soll" > /dev/null
#    dkeshopbluenodes=$?
#
#    echo $OUTPUT | grep "number of blue edges: $dkeshopblueedges_soll" > /dev/null
#    dkeshopblueedges=$?
#
#    echo $OUTPUT | grep "number of states stored in nodes: $dkeshopstoredstates_soll" > /dev/null
#    dkeshopstoredstates=$?
#
#    if [ $dkeshopcontrol -ne 0 -o $dkeshopbluenodes -ne 0 -o $dkeshopblueedges -ne 0 -o $dkeshopstoredstates -ne 0 ]
#    then
#    echo   ... failed to build OG correctly
#    fi
#
#    result=`expr $result + $dkeshopcontrol + $dkeshopbluenodes + $dkeshopblueedges + $dkeshopstoredstates`
#fi

############################################################################
# OG
############################################################################
# DKE-Shops brauchen zu lang (ca. 60 bzw 80 Sekunden)
#
#dkeshopbluenodes_soll=5
#dkeshopblueedges_soll=4
#dkeshopstoredstates_soll=76976
#
#dkeshopbluenodes=0
#dkeshopblueedges=0
#dkeshopstoredstates=0
#
#owfn="$DIR/dke07_shop_sect_6.owfn"
#cmd="$FIONA $owfn -t OG -e 1"
#
#if [ "$quiet" != "no" ]; then
#    cmd="$cmd -Q"
#fi
#
#if [ "$memcheck" = "yes" ]; then
#    memchecklog="$owfn.OG.memcheck.log"
#    do_memcheck "$cmd" "$memchecklog"
#    result=$(($result | $?))
#else
#    echo running $cmd
#    OUTPUT=`$cmd 2>&1`
#
#    echo $OUTPUT | grep "net is controllable: YES" > /dev/null
#    dkeshopcontrol=$?
#
#    echo $OUTPUT | grep "number of blue nodes: $dkeshopbluenodes_soll" > /dev/null
#    dkeshopbluenodes=$?
#
#    echo $OUTPUT | grep "number of blue edges: $dkeshopblueedges_soll" > /dev/null
#    dkeshopblueedges=$?
#
#    echo $OUTPUT | grep "number of states stored in nodes: $dkeshopstoredstates_soll" > /dev/null
#    dkeshopstoredstates=$?
#
#    if [ $dkeshopcontrol -ne 0 -o $dkeshopbluenodes -ne 0 -o $dkeshopblueedges -ne 0 -o $dkeshopstoredstates -ne 0 ]
#    then
#    echo   ... failed to build OG correctly
#    fi
#
#    result=`expr $result + $dkeshopcontrol + $dkeshopbluenodes + $dkeshopblueedges + $dkeshopstoredstates`
#fi

echo

exit $result
