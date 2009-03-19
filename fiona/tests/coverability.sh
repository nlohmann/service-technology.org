#!/usr/bin/env bash

source defaults.sh
source memcheck_helper.sh

echo
echo ---------------------------------------------------------------------
echo running $0
echo

SUBDIR=coverability
DIR=$testdir/coverability
FIONA=fiona

# loeschen aller erzeugten Dateien im letzten Durchlauf
rm -f $DIR/*.out
rm -f $DIR/*.png
rm -f $DIR/*.log

result=0

# for make distcheck: create directory $builddir/$SUBDIR for writing output files to
if [ "$testdir" != "$builddir" ]; then
    if [ ! -e $builddir/$SUBDIR ]; then
        $MKDIR_P $builddir/$SUBDIR
    fi
fi

###########################################################################
# creating the OG's 
###########################################################################

owfn="$DIR/Nc1.owfn"
cmd="$FIONA $owfn -t og -p cover -s empty"

if [ "$quiet" != "no" ]; then
    cmd="$cmd -Q"
fi

if [ "$memcheck" = "yes" ]; then
    memchecklog="$owfn.match.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
    if [ $? -ne 0 ]; then
        echo ... fiona exited with nonzero return value although it should not
        result=1
    fi

    echo $OUTPUT | grep "net is controllable: YES" > /dev/null
    if [ $? -ne 0 ]; then
        echo ... net is not controllable although it should! 
        result=1
    fi
fi

############################################################################

owfn="$DIR/Nc2.owfn"
cmd="$FIONA $owfn -t og -p cover -s empty"

if [ "$quiet" != "no" ]; then
    cmd="$cmd -Q"
fi

if [ "$memcheck" = "yes" ]; then
    memchecklog="$owfn.match.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
    if [ $? -ne 0 ]; then
        echo ... fiona exited with nonzero return value although it should not
        result=1
    fi

    echo $OUTPUT | grep "net is controllable: YES" > /dev/null
    if [ $? -ne 0 ]; then
        echo ... net is not controllable although it should! 
        result=1
    fi
fi

############################################################################

owfn="$DIR/Nc3.owfn"
cmd="$FIONA $owfn -t og -p cover -s empty"

if [ "$quiet" != "no" ]; then
    cmd="$cmd -Q"
fi

if [ "$memcheck" = "yes" ]; then
    memchecklog="$owfn.match.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
    if [ $? -ne 1 ]; then
        echo ... fiona exited with a return value other than 1 - although it should not
        result=1
    fi

    echo $OUTPUT | grep "no_cover_internal" > /dev/null
    if [ $? -ne 0 ]; then
        echo ... net is controllable although it should not!
        result=1
    fi
fi

############################################################################
# Matching with Nc1
############################################################################

og="$DIR/Nc1.owfn.covog"
owfn="$DIR/M1.owfn"
cmd="$FIONA $owfn -t match -p cover $og"
if [ "$memcheck" = "yes" ]; then
    memchecklog="$owfn.match.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
    if [ $? -ne 0 ]; then
        echo ... fiona exited with nonzero return value although it should not
        result=1
    fi

    echo $OUTPUT | grep ": YES" > /dev/null
    if [ $? -ne 0 ]; then
        echo ... oWFN does not match with OG although it should
        result=1
    fi
fi

############################################################################

owfn="$DIR/M2.owfn"
cmd="$FIONA $owfn -t match -p cover $og"
if [ "$memcheck" = "yes" ]; then
    memchecklog="$owfn.match.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
    if [ $? -ne 0 ]; then
        echo ... fiona exited with nonzero return value although it should not
        result=1
    fi

    echo $OUTPUT | grep ": YES" > /dev/null
    if [ $? -ne 0 ]; then
        echo ... oWFN does not match with OG although it should
        result=1
    fi
fi

############################################################################
# Matching with Nc2
############################################################################

og="$DIR/Nc2.owfn.covog"
owfn="$DIR/M1.owfn"
cmd="$FIONA $owfn -t match -p cover $og"
if [ "$memcheck" = "yes" ]; then
    memchecklog="$owfn.match.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
    if [ $? -ne 1 ]; then
        echo ... fiona exited with a return value other than 1 - although it should not
        result=1
    fi

    echo $OUTPUT | grep ": NO" > /dev/null
    if [ $? -ne 0 ]; then
        echo ... oWFN matches with OG although it should not
        result=1
    fi
fi

############################################################################

owfn="$DIR/M2.owfn"
cmd="$FIONA $owfn -t match -p cover $og"
if [ "$memcheck" = "yes" ]; then
    memchecklog="$owfn.match.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
    if [ $? -ne 0 ]; then
        echo ... fiona exited with nonzero return value although it should not
        result=1
    fi

    echo $OUTPUT | grep ": YES" > /dev/null
    if [ $? -ne 0 ]; then
        echo ... oWFN does not match with OG although it should
        result=1
    fi
fi

############################################################################
echo

exit $result
