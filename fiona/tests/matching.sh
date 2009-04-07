#!/usr/bin/env bash

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

############################################################################
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
    fionaExitCode=$?
    $evaluate $fionaExitCode
    if [ $? -ne 0 ] 
    then
        result=1
    else
        echo $OUTPUT | grep ": YES" > /dev/null
        if [ $? -ne 0 ]; then
            echo ... oWFN does not match with OG although it should
            result=1
        fi
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
    fionaExitCode=$?
    $evaluate $fionaExitCode
    if [ $? -ne 0 ] 
    then
        result=1
    else
        echo $OUTPUT | grep ": NO" > /dev/null
        if [ $? -ne 0 ]; then
            echo ...  although it should not
            result=1
        fi
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
    fionaExitCode=$?
    $evaluate $fionaExitCode
    if [ $? -ne 0 ] 
    then
        result=1
    else
        echo $OUTPUT | grep ": NO" > /dev/null
        if [ $? -ne 0 ]; then
            echo ...  although it should not
            result=1
        fi
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
    fionaExitCode=$?
    $evaluate $fionaExitCode
    if [ $? -ne 0 ] 
    then
        result=1
    else
        #echo $OUTPUT | grep "ends or receives more than one message" > /dev/null
        #if [ $? -ne 0 ]; then
        #    echo ... Parsing should have failed due to a transition reading or \
        #        writing more than one message, but it seems there was a different \
        #        reason.
        #    result=1
        #fi
        echo $OUTPUT | grep ": NO" > /dev/null
        if [ $? -ne 0 ]; then
            echo ...  although it should not
            result=1
        fi
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
    fionaExitCode=$?
    $evaluate $fionaExitCode
    if [ $? -ne 0 ] 
    then
        result=1
    else
        echo $OUTPUT | grep ": NO" > /dev/null
        if [ $? -ne 0 ]; then
            echo ...  although it should not
            result=1
        fi
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
    fionaExitCode=$?
    $evaluate $fionaExitCode
    if [ $? -ne 0 ] 
    then
        result=1
    else
        echo $OUTPUT | grep ": NO" > /dev/null
        if [ $? -ne 0 ]; then
            echo ...  although it should not
            result=1
        fi
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
    fionaExitCode=$?
    $evaluate $fionaExitCode
    if [ $? -ne 0 ] 
    then
        result=1
    else
        echo $OUTPUT | grep ": NO" > /dev/null
        if [ $? -ne 0 ]; then
            echo ...  although it should not
            result=1
        fi
    fi
fi

############################################################################
og="$DIR/interface_rcv-a_snd-b.og"
############################################################################

owfn="$DIR/interface_rcv-a_snd-by.owfn"
cmd="$FIONA $owfn -t match $og"
if [ "$memcheck" = "yes" ]; then
    memchecklog="$owfn.match.memcheck.log"
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
        echo $OUTPUT | grep ": NO" > /dev/null
        if [ $? -ne 0 ]; then
            echo ...  although it should not
            result=1
        fi
    fi
fi

############################################################################

owfn="$DIR/interface_rcv-ax_snd-b.owfn"
cmd="$FIONA $owfn -t match $og"
if [ "$memcheck" = "yes" ]; then
    memchecklog="$owfn.match.memcheck.log"
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
        echo $OUTPUT | grep ": NO" > /dev/null
        if [ $? -ne 0 ]; then
            echo ...  although it should not
            result=1
        fi
    fi
fi

############################################################################
og="$DIR/internal.og"
############################################################################

owfn="$DIR/internal_match.owfn"
cmd="$FIONA $owfn -t match $og"
if [ "$memcheck" = "yes" ]; then
    memchecklog="$owfn.match.memcheck.log"
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
        echo $OUTPUT | grep ": YES" > /dev/null
        if [ $? -ne 0 ]; then
            echo ... oWFN does not match with OG although it should
            result=1
        fi
    fi
fi

############################################################################
og="$DIR/interface_match.og"
############################################################################

owfn="$DIR/interface-partner.owfn"
cmd="$FIONA $owfn -t match $og"
if [ "$memcheck" = "yes" ]; then
    memchecklog="$owfn.match.memcheck.log"
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
        echo $OUTPUT | grep ": YES" > /dev/null
        if [ $? -ne 0 ]; then
            echo ... oWFN does not match with OG although it should
            result=1
        fi
    fi
fi

############################################################################
og="$DIR/interface_nomatch.og"
############################################################################

owfn="$DIR/interface-partner.owfn"
cmd="$FIONA $owfn -t match $og"
if [ "$memcheck" = "yes" ]; then
    memchecklog="$owfn.match.memcheck.log"
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
        echo $OUTPUT | grep ": NO" > /dev/null
        if [ $? -ne 0 ]; then
            echo ...  although it should not
            result=1
        fi
    fi
fi

############################################################################
owfn="$DIR/small_cycle.owfn"
############################################################################

og="$DIR/small_cycle_1a.og"
cmd="$FIONA $owfn -t match $og"
if [ "$memcheck" = "yes" ]; then
    memchecklog="$owfn.match.memcheck.log"
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
        echo $OUTPUT | grep ": NO" > /dev/null
        if [ $? -ne 0 ]; then
            echo ...  although it should not
            result=1
        fi
    fi
fi
############################################################################
og="$DIR/small_cycle_1b.og"
cmd="$FIONA $owfn -t match $og"
if [ "$memcheck" = "yes" ]; then
    memchecklog="$owfn.match.memcheck.log"
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
        echo $OUTPUT | grep ": NO" > /dev/null
        if [ $? -ne 0 ]; then
            echo ...  although it should not
            result=1
        fi
    fi
fi
############################################################################
og="$DIR/small_cycle_2a.og"
cmd="$FIONA $owfn -t match $og"
if [ "$memcheck" = "yes" ]; then
    memchecklog="$owfn.match.memcheck.log"
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
        echo $OUTPUT | grep ": NO" > /dev/null
        if [ $? -ne 0 ]; then
            echo ...  although it should not
            result=1
        fi
    fi
fi
############################################################################
og="$DIR/small_cycle_2b.og"
cmd="$FIONA $owfn -t match $og"
if [ "$memcheck" = "yes" ]; then
    memchecklog="$owfn.match.memcheck.log"
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
        echo $OUTPUT | grep ": NO" > /dev/null
        if [ $? -ne 0 ]; then
            echo ...  although it should not
            result=1
        fi
    fi
fi

############################################################################
## The following tests check the recursion inside the matching algorithm. ##
############################################################################

# This is a workaround for bug #12901 (see https://gna.org/bugs/?12901).
# Some tests rely on specific debug messages and if fiona was build without
# them, these tests have to be skipped.
check="$FIONA --bug"
OUTPUT=`$check 2>&1`
echo $OUTPUT | grep "config ASSERT:" | grep "yes" > /dev/null
if [ $? -ne 0 ]; then
  echo "Note: Fiona was built without assertions/debug messages, so we skip some tests!"
  echo

	# The exit 77 is the magic value which tells Automake's `make check'
	# that this test was skipped.
  exit 77
fi

############################################################################
og="$DIR/cycle_1.og"
############################################################################

owfn="$DIR/cycle_1.owfn"
cmd="$FIONA -t match $owfn $og"
if [ "$memcheck" = "yes" ]; then
    memchecklog="$owfn.match.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd -d 4 | grep "start cycle" | wc -l 2>&1`
    #echo start cycles: $OUTPUT
    if [ $OUTPUT -ne 2 ] 
    then
        echo ... something went wrong inside the matching algorithm
        result=1
    fi
fi

owfn="$DIR/cycle_2.owfn"
cmd="$FIONA -t match $owfn $og"
if [ "$memcheck" = "yes" ]; then
    memchecklog="$owfn.match.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd -d 4 | grep "start cycle" | wc -l 2>&1`
    #echo start cycles: $OUTPUT
    if [ $OUTPUT -ne 2 ] 
    then
        echo ... something went wrong inside the matching algorithm
        result=1
    fi
fi

############################################################################
og="$DIR/cycle_2.og"
############################################################################

owfn="$DIR/cycle_1.owfn"
cmd="$FIONA -t match $owfn $og"
if [ "$memcheck" = "yes" ]; then
    memchecklog="$owfn.match.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd -d 4 | grep "start cycle" | wc -l 2>&1`
    #echo start cycles: $OUTPUT
    if [ $OUTPUT -ne 4 ] 
    then
        echo ... something went wrong inside the matching algorithm
        result=1
    fi
fi

owfn="$DIR/cycle_2.owfn"
cmd="$FIONA -t match $owfn $og"
if [ "$memcheck" = "yes" ]; then
    memchecklog="$owfn.match.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd -d 4 | grep "start cycle" | wc -l 2>&1`
    #echo start cycles: $OUTPUT
    if [ $OUTPUT -ne 4 ] 
    then
        echo ... something went wrong inside the matching algorithm
        result=1
    fi
fi

############################################################################
og="$DIR/cycle_3.og"
############################################################################

owfn="$DIR/cycle_1.owfn"
cmd="$FIONA -t match $owfn $og"
if [ "$memcheck" = "yes" ]; then
    memchecklog="$owfn.match.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd -d 4 | grep "start cycle" | wc -l 2>&1`
    #echo start cycles: $OUTPUT
    if [ $OUTPUT -ne 6 ] 
    then
        echo ... something went wrong inside the matching algorithm
        result=1
    fi
fi

owfn="$DIR/cycle_2.owfn"
cmd="$FIONA -t match $owfn $og"
if [ "$memcheck" = "yes" ]; then
    memchecklog="$owfn.match.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd -d 4 | grep "start cycle" | wc -l 2>&1`
    #echo start cycles: $OUTPUT
    if [ $OUTPUT -ne 6 ] 
    then
        echo ... something went wrong inside the matching algorithm
        result=1
    fi
fi

############################################################################
echo
exit $result
