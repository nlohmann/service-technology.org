#!/bin/bash

source defaults.sh
source memcheck_helper.sh

echo
echo ---------------------------------------------------------------------
echo running $0
echo

SUBDIR=png
DIR=$testdir/$SUBDIR
FIONA=fiona

#loeschen aller erzeugten Dateien im letzten Durchlauf
rm -f $DIR/big.owfn.output.dot
rm -f $DIR/big.owfn.output.out
rm -f $DIR/big.owfn.output.png
rm -f $DIR/big.owfn.output.tex
rm -f $DIR/big.owfn.output.og
rm -f $DIR/big.owfn.output.og.dot
rm -f $DIR/big.owfn.output.og.out
rm -f $DIR/big.owfn.output.og.png
rm -f $DIR/big.owfn.output.og.tex
rm -f $DIR/06-03-23_BPM06_shop_sect_3.owfn.og.output.dot
rm -f $DIR/06-03-23_BPM06_shop_sect_3.owfn.og.output.out
rm -f $DIR/06-03-23_BPM06_shop_sect_3.owfn.og.output.png
rm -f $DIR/06-03-23_BPM06_shop_sect_3.owfn.og.output.tex
rm -f $DIR/*.log

result=0

############################################################################
resultSingle=0
og="$DIR/06-03-23_BPM06_shop_sect_3.owfn.og"
outputPrefix="$builddir/png/06-03-23_BPM06_shop_sect_3.owfn.og.output"

# for make distcheck: make copy of $owfn and work on it
if [ "$testdir" != "$builddir" ]; then
    if [ ! -e $builddir/$SUBDIR ]; then
        $MKDIR_P $builddir/$SUBDIR
    fi
fi

cmd="$FIONA $og -t png -p tex -o $outputPrefix"

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
        echo $OUTPUT | grep "06-03-23_BPM06_shop_sect_3.owfn.og.output.tex generated" > /dev/null
        resultSingle=$?
    
        if [ $resultSingle -ne 0 ]; then
            result=1
            echo "... failed: no tex file was generated."
        fi
    
    
        echo $OUTPUT | grep "06-03-23_BPM06_shop_sect_3.owfn.og.output.png generated" > /dev/null
        resultSingle=$?
    
        if [ $resultSingle -ne 0 ]; then
            result=1
            echo "... failed: no png was generated."
        fi
    fi
fi


############################################################################
resultSingle=0
owfn="$DIR/big.owfn"
outputPrefix="$builddir/png/big.owfn.output"

# for make distcheck: make copy of $owfn and work on it
if [ "$testdir" != "$builddir" ]; then
    if [ ! -e $builddir/$SUBDIR ]; then
        $MKDIR_P $builddir/$SUBDIR
    fi
fi

cmd="$FIONA $owfn -t png -p tex -o $outputPrefix"

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
        echo $OUTPUT | grep "big.owfn.output.tex generated" > /dev/null
        resultSingle=$?
    
        if [ $resultSingle -ne 0 ]; then
            result=1
            echo "... failed: no tex file was generated."
        fi
    
        echo $OUTPUT | grep "big.owfn.output.png generated" > /dev/null
        resultSingle=$?
    
        if [ $resultSingle -ne 0 ]; then
            result=1
            echo "... failed: no png was generated."
        fi
    fi
fi


############################################################################
resultSingle=0
owfn="$DIR/big.owfn"
outputPrefix="$builddir/png/big.owfn.output"

# for make distcheck: make copy of $owfn and work on it
if [ "$testdir" != "$builddir" ]; then
    if [ ! -e $builddir/$SUBDIR ]; then
        $MKDIR_P $builddir/$SUBDIR
    fi
fi

cmd="$FIONA $owfn -t og -p tex -o $outputPrefix"

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

        echo $OUTPUT | grep "big.owfn.output.og.tex generated" > /dev/null
        resultSingle=$?
    
        if [ $resultSingle -ne 0 ]; then
            result=1
            echo "... failed: no tex file was generated."
        fi
    
        echo $OUTPUT | grep "big.owfn.output.og.png generated" > /dev/null
        resultSingle=$?
    
        if [ $resultSingle -ne 0 ]; then
            result=1
            echo "... failed: no png was generated."
        fi
    
    fi
fi


###########################################################################
if [ $result -ne 0 ]; then
    if [ "$testdir" != "$builddir" ]; then
        rm -rf $builddir/$SUBDIR
    fi
fi

echo
exit $result
