#!/bin/bash

source defaults.sh
source memcheck_helper.sh

echo
echo ---------------------------------------------------------------------
echo running $0
echo

SUBDIR=sequence_suite
DIR=$testdir/$SUBDIR
FIONA=fiona

#loeschen aller erzeugten Dateien im letzten Durchlauf
rm -f $DIR/*.log

result=0
#exit $result

############################################################################

owfn="$DIR/sequence5.owfn"
cmd="$FIONA $owfn -b 4 -t OG"
if [ "$quiet" != "no" ]; then
    cmd="$cmd -Q"
fi

if [ "$memcheck" = "yes" ]; then
    memchecklog="$owfn.b4.IG.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    $cmd  2>/dev/null 1>/dev/null
    result5=$?
    if [ $result5 -ne 0 ] ; then
        echo ... failed to build BDD
        result=1
    fi
fi

############################################################################

nodes_mp=28
nodes_ann=16

owfn="$DIR/sequence3.owfn"
cmd="$FIONA $owfn -b 4 -t OG"
if [ "$quiet" != "no" ]; then
    cmd="$cmd -Q"
fi

if [ "$memcheck" = "yes" ]; then
    memchecklog="$owfn.b4.IG.memcheck.log"
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
        echo $OUTPUT | grep "BDD_MP: number of nodes: $nodes_mp" > /dev/null
        resultNodesMP=$?
        echo $OUTPUT | grep "BDD_ANN: number of nodes: $nodes_ann" > /dev/null
        resultNodesANN=$?
    
        if [ $resultNodesMP -ne 0 ] ; then
            echo ... failed to build BDD_MP correct
            result=1
        fi
    
        if [ $resultNodesANN -ne 0 ] ; then
            echo ... failed to build BDD_ANN correctly
            result=1
        fi
    fi
fi

############################################################################

owfn="$DIR/sequence3.owfn"
owfn_reference="$testdir/bdd_ref/sequence3_reference.owfn"

# for make distcheck: make copy of $owfn and work on it
if [ "$testdir" != "$builddir" ]; then
    if [ ! -e $builddir/$SUBDIR ]; then
        $MKDIR_P $builddir/$SUBDIR
    fi

    cp $owfn $builddir/$SUBDIR
fi

owfn="$builddir/$SUBDIR/sequence3.owfn"

cmd="$FIONA $owfn -b 4 -t OG"
$cmd 2>/dev/null 1>/dev/null

cmd="$FIONA $owfn $owfn_reference -t equivalence -b1"


if [ "$memcheck" = "yes" ]; then
    memchecklog="$owfn.n.n.x.memcheck.log"
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
        echo $OUTPUT | grep "The two operating guidelines are equal: YES" > /dev/null
        resultEqual=$?
    
        if [ $resultEqual -ne 0 ] ; then
            echo "... failed (the two operating guidelines are not equal although they should be)"
            result=1
        fi
    fi
fi

if [ "$testdir" != "$builddir" ]; then
    rm -rf $builddir/$SUBDIR
fi

############################################################################
############################################################################

nodes_mp=1562
nodes_ann=361

owfn="$testdir/philosophers/phcontrol4.unf.owfn"
cmd="$FIONA $owfn -b 4 -t OG"
if [ "$quiet" != "no" ]; then
    cmd="$cmd -Q"
fi

if [ "$memcheck" = "yes" ]; then
    memchecklog="$owfn.b4.IG.memcheck.log"
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
        echo $OUTPUT | grep "BDD_MP: number of nodes: $nodes_mp" > /dev/null
        resultNodesMP=$?
        echo $OUTPUT | grep "BDD_ANN: number of nodes: $nodes_ann" > /dev/null
        resultNodesANN=$?
    
        if [ $resultNodesMP -ne 0 ] ; then
            echo ... failed to build BDD_MP correctly
            result=1
        fi
    
        if [ $resultNodesANN -ne 0 ] ; then
            echo ... failed to build BDD_ANN correctly
            result=1
        fi
    fi
fi

############################################################################

owfn="$testdir/philosophers/phcontrol4.unf.owfn"
owfn_reference="$testdir/bdd_ref/phcontrol4.unf_reference.owfn"

# for make distcheck: make copy of $owfn and work on it
SUBDIR=philosophers
if [ "$testdir" != "$builddir" ]; then
    if [ ! -e $builddir/$SUBDIR ]; then
        $MKDIR_P $builddir/$SUBDIR
    fi

    cp $owfn $builddir/$SUBDIR
fi

owfn="$builddir/$SUBDIR/phcontrol4.unf.owfn"
cmd="$FIONA $owfn -b 4 -t OG"
$cmd 2>/dev/null 1>/dev/null

cmd="$FIONA $owfn $owfn_reference -t equivalence -b1"


if [ "$memcheck" = "yes" ]; then
    memchecklog="$owfn.n.n.x.memcheck.log"
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
        echo $OUTPUT | grep "The two operating guidelines are equal: YES" > /dev/null
        resultEqual=$?
    
        if [ $resultEqual -ne 0 ] ; then
            echo "... failed (the two operating guidelines are not equal although they should be)"
            result=1
        fi
    fi
fi

if [ "$testdir" != "$builddir" ]; then
    rm -rf $builddir/$SUBDIR
fi

############################################################################

echo

exit $result
