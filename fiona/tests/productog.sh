#!/bin/bash

source defaults.sh
source memcheck_helper.sh

echo
echo ---------------------------------------------------------------------
echo running $0
echo

SUBDIR=productog
DIR=$testdir/$SUBDIR
FIONA=fiona

#loeschen aller erzeugten Dateien im letzten Durchlauf
rm -f $DIR/1x2.output.og
rm -f $DIR/1x2.output.out
rm -f $DIR/1x2.output.png
rm -f $DIR/1x2x3.output.og
rm -f $DIR/1x2x3.output.png
rm -f $DIR/1x2x3.output.out
rm -f $DIR/3x1x2.output.og
rm -f $DIR/3x1x2.output.png
rm -f $DIR/3x1x2.output.out
rm -f $DIR/*.log

result=0

############################################################################
# Building product of two OGs.                                             #
############################################################################
resultSingle=0
og1="$DIR/1.og"
og2="$DIR/2.og"
outputPrefix="$builddir/productog/1x2.output"
outputExpected="$testdir/productog/1x2.expected.og"

# for make distcheck: make copy of $owfn and work on it
if [ "$testdir" != "$builddir" ]; then
    if [ ! -e $builddir/$SUBDIR ]; then
        $MKDIR_P $builddir/$SUBDIR
    fi
fi

cmd="$FIONA $og1 $og2 -t productog -p no-dot -o $outputPrefix"

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
        if [ $resultSingle -eq 0 ] ; then
            echo "diff $outputPrefix.og $outputExpected"
            if ! diff "$outputPrefix.og" "$outputExpected" >/dev/null ; then
                echo "... failed: Output and expected output differ. Compare " \
                     "$outputPrefix.og" "$outputExpected"
                resultSingle=1
            fi
        fi
    fi
fi

if [ $resultSingle -ne 0 ]; then
    result=1
fi

############################################################################
# Building product of three OGs.                                           #
############################################################################
resultSingle=0
og1="$DIR/1.og"
og2="$DIR/2.og"
og3="$DIR/3.og"
outputPrefix="$builddir/productog/1x2x3.output"
outputExpected="$testdir/productog/1x2x3.expected.og"

# for make distcheck: make copy of $owfn and work on it
if [ "$testdir" != "$builddir" ]; then
    if [ ! -e $builddir/$SUBDIR ]; then
        $MKDIR_P $builddir/$SUBDIR
    fi
fi

cmd="$FIONA $og1 $og2 $og3 -t productog -p no-dot -o $outputPrefix"

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
        if [ $resultSingle -eq 0 ] ; then
            echo "diff $outputPrefix.og $outputExpected"
            if ! diff "$outputPrefix.og" "$outputExpected" >/dev/null ; then
                echo "... failed: Output and expected output differ. Compare " \
                     "$outputPrefix.og" "$outputExpected"
                resultSingle=1
            fi
        fi
    fi
fi

if [ $resultSingle -ne 0 ]; then
    result=1
fi

##############################################################################
# Building product of three OGs where the product of the first two is empty. #
##############################################################################
resultSingle=0
og1="$DIR/1.og"
og2="$DIR/2.og"
og3="$DIR/3.og"
outputPrefix="$builddir/productog/3x1x2.output"
outputExpected="$testdir/productog/3x1x2.expected.og"

# for make distcheck: make copy of $owfn and work on it
if [ "$testdir" != "$builddir" ]; then
    if [ ! -e $builddir/$SUBDIR ]; then
        $MKDIR_P $builddir/$SUBDIR
    fi
fi

cmd="$FIONA $og3 $og1 $og2 -t productog -p no-dot -o $outputPrefix"

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
        if [ $resultSingle -eq 0 ] ; then
            echo "diff $outputPrefix.og $outputExpected"
            if ! diff "$outputPrefix.og" "$outputExpected" >/dev/null ; then
                echo "... failed: Output and expected output differ. Compare " \
                     "$outputPrefix.og" "$outputExpected"
                resultSingle=1
            fi
        fi
    fi
fi

if [ $resultSingle -ne 0 ]; then
    result=1
fi

###########################################################################
if [ $result -ne 0 ]; then
    if [ "$testdir" != "$builddir" ]; then
        rm -rf $builddir/$SUBDIR
    fi
fi

echo
exit $result
