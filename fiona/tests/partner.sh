#!/usr/bin/env bash

source defaults.sh
source memcheck_helper.sh

echo
echo ---------------------------------------------------------------------
echo running $0
echo

if test "$PETRIFY" = "not found"; then
  echo "Note: Fiona was built without Petrify support!"
  echo "      See file README."
  echo

	# The exit 77 is the magic value which tells Automake's `make check'
	# that this test was skipped.
  exit 77
fi

SUBDIR=partner
DIR=$testdir/$SUBDIR
FIONA=fiona

result=0

#loeschen aller erzeugten Dateien im letzten Durchlauf
rm -f $DIR/bug13405.owfn.*
rm -f $DIR/bug13405-partner.dot
rm -f $DIR/bug13405-partner.owfn
rm -f $DIR/bug13405-partner.png
rm -f $DIR/sequence3.owfn.i*
rm -f $DIR/sequence3.owfn.o*
rm -f $DIR/sequence3-partner.dot
rm -f $DIR/sequence3-partner.owfn
rm -f $DIR/sequence3-partner.png
rm -f $DIR/coffee.owfn.i*
rm -f $DIR/coffee.owfn.o*
rm -f $DIR/coffee-partner.dot
rm -f $DIR/coffee-partner.owfn
rm -f $DIR/coffee-partner.png
rm -f $DIR/coffee2.owfn.i*
rm -f $DIR/coffee2.owfn.o*
rm -f $DIR/coffee2-partner.dot
rm -f $DIR/coffee2-partner.owfn
rm -f $DIR/coffee2-partner.png
rm -f $DIR/coffee_extended.owfn.i*
rm -f $DIR/coffee_extended.owfn.o*
rm -f $DIR/coffee_extended-partner.dot
rm -f $DIR/coffee_extended-partner.owfn
rm -f $DIR/coffee_extended-partner.png

############################################################################
# Building most permissive partner for bug13405 out of oWFN with allnodes  #
############################################################################
input="$DIR/bug13405.owfn"
inputCopy="$builddir/$SUBDIR/bug13405.owfn"
output="$builddir/$SUBDIR/bug13405-partner.owfn"
outputExpected="$testdir/$SUBDIR/bug13405-partner.expected.owfn"


# for make distcheck: create directory $builddir/$SUBDIR for writing output files to
if [ "$testdir" != "$builddir" ]; then
    if [ ! -e $builddir/$SUBDIR ]; then
        $MKDIR_P $builddir/$SUBDIR
    fi

    # make copy of $input and work on it
    cp $input $inputCopy
fi

cmd="$FIONA $inputCopy -t mostpermissivepartner -s allnodes"
check="$FIONA $output $outputExpected -t equivalence"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$input.memcheck.log"
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
        echo running $check
        OUTPUT=`$check 2>&1`
        fionaExitCode=$?
        $evaluate $fionaExitCode
        if [ $? -ne 0 ] 
        then
            result=1
        else
            echo $OUTPUT | grep "are equivalent: YES" > /dev/null
            if [ $? -ne 0 ]; then
                let "result += 1"
                echo ... computed partner not equivalent to expected partner.
            fi
        fi
    fi
fi


############################################################################
# Building most permissive partner for bug13405 out of oWFN               #
############################################################################
input="$DIR/bug13405.owfn"
inputCopy="$builddir/$SUBDIR/bug13405.owfn"
output="$builddir/$SUBDIR/bug13405-partner.owfn"
outputExpected="$testdir/$SUBDIR/bug13405-partner.expected.owfn"


# for make distcheck: create directory $builddir/$SUBDIR for writing output files to
if [ "$testdir" != "$builddir" ]; then
    if [ ! -e $builddir/$SUBDIR ]; then
        $MKDIR_P $builddir/$SUBDIR
    fi

    # make copy of $input and work on it
    cp $input $inputCopy
fi

cmd="$FIONA $inputCopy -t mostpermissivepartner"
check="$FIONA $output $outputExpected -t equivalence"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$input.memcheck.log"
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
        echo running $check
        OUTPUT=`$check 2>&1`
        fionaExitCode=$?
        $evaluate $fionaExitCode
        if [ $? -ne 0 ] 
        then
            result=1
        else
            echo $OUTPUT | grep "are equivalent: YES" > /dev/null
            if [ $? -ne 0 ]; then
                let "result += 1"
                echo ... computed partner not equivalent to expected partner.
            fi
        fi
    fi
fi

#rm -f $DIR/bug13405.owfn.* we need the OG for the next test
rm -f $DIR/bug13405-partner.dot
rm -f $DIR/bug13405-partner.owfn
rm -f $DIR/bug13405-partner.png

############################################################################
# Building most permissive partner for bug13405 out of OG                 #
############################################################################
input="$builddir/$SUBDIR/bug13405.owfn.og"
inputCopy="$builddir/$SUBDIR/bug13405.owfn.og"
output="$builddir/$SUBDIR/bug13405-partner.owfn"
outputExpected="$testdir/$SUBDIR/bug13405-partner.expected.owfn"


if [ "$testdir" != "$builddir" ]; then
    if [ ! -e $builddir/$SUBDIR ]; then
        $MKDIR_P $builddir/$SUBDIR
    fi

#    cp $input $inputCopy
fi

cmd="$FIONA $inputCopy -t mostpermissivepartner"
check="$FIONA $output $outputExpected -t equivalence"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$input.memcheck.log"
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
        echo running $check
        OUTPUT=`$check 2>&1`
        fionaExitCode=$?
        $evaluate $fionaExitCode
        if [ $? -ne 0 ] 
        then
            result=1
        else
            echo $OUTPUT | grep "are equivalent: YES" > /dev/null
            if [ $? -ne 0 ]; then
                let "result += 1"
                echo ... computed partner not equivalent to expected partner.
            fi
        fi
    fi
fi

############################################################################
# Building small partner for coffee and coffee2 out of oWFN                #
############################################################################
input="$DIR/coffee.owfn"
inputCopy="$builddir/$SUBDIR/coffee.owfn"
input2="$DIR/coffee2.owfn"
inputCopy2="$builddir/$SUBDIR/coffee2.owfn"

output="$builddir/$SUBDIR/coffee-partner.owfn"
outputExpected="$testdir/$SUBDIR/coffee-partner.expected.owfn"
output2="$builddir/$SUBDIR/coffee2-partner.owfn"
outputExpected2="$testdir/$SUBDIR/coffee2-partner.expected.owfn"


if [ "$testdir" != "$builddir" ]; then
    if [ ! -e $builddir/$SUBDIR ]; then
        $MKDIR_P $builddir/$SUBDIR
    fi

    cp $input $inputCopy
    cp $input2 $inputCopy2
fi

cmd="$FIONA $inputCopy $inputCopy2 -t smallpartner"
check="$FIONA $output $outputExpected -t equivalence"
check2="$FIONA $output2 $outputExpected2 -t equivalence"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$input.memcheck.log"
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
        echo running $check
        OUTPUT=`$check 2>&1`
        fionaExitCode=$?
        $evaluate $fionaExitCode
        if [ $? -ne 0 ] 
        then
            result=1
        else
            echo $OUTPUT | grep "are equivalent: YES" > /dev/null
            if [ $? -ne 0 ]; then
                let "result += 1"
                echo ... computed partner not equivalent to expected partner.
            fi
            echo running $check2
            OUTPUT=`$check2 2>&1`
            fionaExitCode=$?
            $evaluate $fionaExitCode
            if [ $? -ne 0 ] 
            then
                result=1
            else
                echo $OUTPUT | grep "are equivalent: YES" > /dev/null
                if [ $? -ne 0 ]; then
                    let "result += 1"
                    echo ... computed partner not equivalent to expected partner.
                fi
            fi
        fi        
    fi
fi

#rm -f $DIR/coffee.owfn.*
rm -f $DIR/coffee-partner.dot
rm -f $DIR/coffee-partner.owfn
rm -f $DIR/coffee-partner.png
#rm -f $DIR/coffee2.owfn.*
rm -f $DIR/coffee2-partner.dot
rm -f $DIR/coffee2-partner.owfn
rm -f $DIR/coffee2-partner.png

############################################################################
# Building small partner for coffee and coffee_extended out of OG          #
############################################################################
input="$builddir/$SUBDIR/coffee.owfn.ig.og"
inputCopy="$builddir/$SUBDIR/coffee.owfn.ig.og"
input2="$builddir/$SUBDIR/coffee2.owfn.ig.og"
inputCopy2="$builddir/$SUBDIR/coffee2.owfn.ig.og"

output="$builddir/$SUBDIR/coffee-partner.owfn"
outputExpected="$testdir/$SUBDIR/coffee-partner.expected.owfn"
output2="$builddir/$SUBDIR/coffee2-partner.owfn"
outputExpected2="$testdir/$SUBDIR/coffee2-partner.expected.owfn"


if [ "$testdir" != "$builddir" ]; then
    if [ ! -e $builddir/$SUBDIR ]; then
        $MKDIR_P $builddir/$SUBDIR
    fi

#    cp $input $inputCopy
#    cp $input2 $inputCopy2
fi

cmd="$FIONA $inputCopy $inputCopy2 -t smallpartner"
check="$FIONA $output $outputExpected -t equivalence"
check2="$FIONA $output2 $outputExpected2 -t equivalence"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$input.memcheck.log"
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
        echo running $check
        OUTPUT=`$check 2>&1`
        fionaExitCode=$?
        $evaluate $fionaExitCode
        if [ $? -ne 0 ] 
        then
            result=1
        else
            echo $OUTPUT | grep "are equivalent: YES" > /dev/null
            if [ $? -ne 0 ]; then
                let "result += 1"
                echo ... computed partner not equivalent to expected partner.
            fi
            echo running $check2
            OUTPUT=`$check2 2>&1`
            fionaExitCode=$?
            $evaluate $fionaExitCode
            if [ $? -ne 0 ] 
            then
                result=1
            else
                echo $OUTPUT | grep "are equivalent: YES" > /dev/null
                if [ $? -ne 0 ]; then
                    let "result += 1"
                    echo ... computed partner not equivalent to expected partner.
                fi
            fi
        fi
    fi
fi

############################################################################
# Building reduced small partner for coffee_extended out of oWFN           #
############################################################################
input="$DIR/coffee_extended.owfn"
inputCopy="$builddir/$SUBDIR/coffee_extended.owfn"
output="$builddir/$SUBDIR/coffee_extended-partner.owfn"
outputExpected="$testdir/$SUBDIR/coffee_extended-partner.expected.owfn"


if [ "$testdir" != "$builddir" ]; then
    if [ ! -e $builddir/$SUBDIR ]; then
        $MKDIR_P $builddir/$SUBDIR
    fi

    cp $input $inputCopy
fi

cmd="$FIONA $inputCopy -t smallpartner -r"
check="$FIONA $output $outputExpected -t equivalence"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$input.memcheck.log"
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
        echo running $check
        OUTPUT=`$check 2>&1`
        fionaExitCode=$?
        $evaluate $fionaExitCode
        if [ $? -ne 0 ] 
        then
            result=1
        else
            echo $OUTPUT | grep "are equivalent: YES" > /dev/null
            if [ $? -ne 0 ]; then
                let "result += 1"
                echo ... computed partner not equivalent to expected partner.
            fi
        fi
    fi
fi

###########################################################################
if [ $result -eq 0 ]; then
    if [ "$testdir" != "$builddir" ]; then
        rm -rf $builddir/$SUBDIR
    fi
fi

echo

exit $result
