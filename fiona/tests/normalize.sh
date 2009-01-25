#!/usr/bin/env bash

source defaults.sh
source memcheck_helper.sh

echo
echo ---------------------------------------------------------------------
echo running $0
echo

SUBDIR=normalize
DIR=$testdir/$SUBDIR
FIONA=fiona

result=0


############################################################################
# normalizing abnormal_input.owfn                                          #
############################################################################

input="$DIR/abnormal_input.owfn"
inputCopy="$builddir/$SUBDIR/abnormal_input.owfn"
output="$builddir/$SUBDIR/abnormal_input.normalized.owfn"
outputExpected="$testdir/$SUBDIR/abnormal_input.normalized.expected.owfn"


# for make distcheck: create directory $builddir/$SUBDIR for writing output files to
if [ "$testdir" != "$builddir" ]; then
    if [ ! -e $builddir/$SUBDIR ]; then
        $MKDIR_P $builddir/$SUBDIR
    fi

    # make copy of $input and work on it
    cp $input $inputCopy
fi

cmd="$FIONA $inputCopy -t normalize"
check="$FIONA $output $outputExpected -t equivalence"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$input.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
    if [ $? -ne 0 ]; then
        let "result += 1"
        echo "... failed: $FIONA exited with non-zero return value."
    else
        echo running $check
        OUTPUT=`$check 2>&1`
        echo $OUTPUT | grep "are equivalent: YES" > /dev/null
        if [ $? -ne 0 ]; then
            let "result += 1"
            echo ... computed normalization not equivalent to expected normalization.
        fi
    fi
fi

############################################################################
# normalizing abnormal_output.owfn                                         #
############################################################################

input="$DIR/abnormal_output.owfn"
inputCopy="$builddir/$SUBDIR/abnormal_output.owfn"
output="$builddir/$SUBDIR/abnormal_output.normalized.owfn"
outputExpected="$testdir/$SUBDIR/abnormal_output.normalized.expected.owfn"


# for make distcheck: create directory $builddir/$SUBDIR for writing output files to
if [ "$testdir" != "$builddir" ]; then
    if [ ! -e $builddir/$SUBDIR ]; then
        $MKDIR_P $builddir/$SUBDIR
    fi

    # make copy of $input and work on it
    cp $input $inputCopy
fi

cmd="$FIONA $inputCopy -t normalize"
check="$FIONA $output $outputExpected -t equivalence"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$input.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
    if [ $? -ne 0 ]; then
        let "result += 1"
        echo "... failed: $FIONA exited with non-zero return value."
    else
        echo running $check
        OUTPUT=`$check 2>&1`
        echo $OUTPUT | grep "are equivalent: YES" > /dev/null
        if [ $? -ne 0 ]; then
            let "result += 1"
            echo ... computed normalization not equivalent to expected normalization.
        fi
    fi
fi

############################################################################
# normalizing abnormal_both.owfn                                           #
############################################################################

input="$DIR/abnormal_both.owfn"
inputCopy="$builddir/$SUBDIR/abnormal_both.owfn"
output="$builddir/$SUBDIR/abnormal_both.normalized.owfn"
outputExpected="$testdir/$SUBDIR/abnormal_both.normalized.expected.owfn"


# for make distcheck: create directory $builddir/$SUBDIR for writing output files to
if [ "$testdir" != "$builddir" ]; then
    if [ ! -e $builddir/$SUBDIR ]; then
        $MKDIR_P $builddir/$SUBDIR
    fi

    # make copy of $input and work on it
    cp $input $inputCopy
fi

cmd="$FIONA $inputCopy -t normalize"
check="$FIONA $output $outputExpected -t equivalence"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$input.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
    if [ $? -ne 0 ]; then
        let "result += 1"
        echo "... failed: $FIONA exited with non-zero return value."
    else
        echo running $check
        OUTPUT=`$check 2>&1`
        echo $OUTPUT | grep "are equivalent: YES" > /dev/null
        if [ $? -ne 0 ]; then
            let "result += 1"
            echo ... computed normalization not equivalent to expected normalization.
        fi
    fi
fi

############################################################################
# normalizing abnormal_both2.owfn                                          #
############################################################################

input="$DIR/abnormal_both2.owfn"
inputCopy="$builddir/$SUBDIR/abnormal_both2.owfn"
output="$builddir/$SUBDIR/abnormal_both2.normalized.owfn"
outputExpected="$testdir/$SUBDIR/abnormal_both2.normalized.expected.owfn"


# for make distcheck: create directory $builddir/$SUBDIR for writing output files to
if [ "$testdir" != "$builddir" ]; then
    if [ ! -e $builddir/$SUBDIR ]; then
        $MKDIR_P $builddir/$SUBDIR
    fi

    # make copy of $input and work on it
    cp $input $inputCopy
fi

cmd="$FIONA $inputCopy -t normalize"
check="$FIONA $output $outputExpected -t equivalence"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$input.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
    if [ $? -ne 0 ]; then
        let "result += 1"
        echo "... failed: $FIONA exited with non-zero return value."
    else
        echo running $check
        OUTPUT=`$check 2>&1`
        echo $OUTPUT | grep "are equivalent: YES" > /dev/null
        if [ $? -ne 0 ]; then
            let "result += 1"
            echo ... computed normalization not equivalent to expected normalization.
        fi
    fi
fi

############################################################################
# normalizing coffee.owfn                                                  #
############################################################################

input="$DIR/coffee.owfn"
inputCopy="$builddir/$SUBDIR/coffee.owfn"
output="$builddir/$SUBDIR/coffee.normalized.owfn"
outputExpected="$testdir/$SUBDIR/coffee.normalized.expected.owfn"


# for make distcheck: create directory $builddir/$SUBDIR for writing output files to
if [ "$testdir" != "$builddir" ]; then
    if [ ! -e $builddir/$SUBDIR ]; then
        $MKDIR_P $builddir/$SUBDIR
    fi

    # make copy of $input and work on it
    cp $input $inputCopy
fi

cmd="$FIONA $inputCopy -t normalize"
check="$FIONA $output $outputExpected -t equivalence"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$input.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
    if [ $? -ne 0 ]; then
        let "result += 1"
        echo "... failed: $FIONA exited with non-zero return value."
    else
        echo running $check
        OUTPUT=`$check 2>&1`
        echo $OUTPUT | grep "are equivalent: YES" > /dev/null
        if [ $? -ne 0 ]; then
            let "result += 1"
            echo ... computed normalization not equivalent to expected normalization.
        fi
    fi
fi


############################################################################

# remove all created files
rm -f $DIR/abnormal_input.normalized.owfn
rm -f $DIR/abnormal_output.normalized.owfn
rm -f $DIR/abnormal_both.normalized.owfn
rm -f $DIR/abnormal_both2.normalized.owfn
rm -f $DIR/coffee.normalized.owfn

if [ $result -eq 0 ]; then
    if [ "$testdir" != "$builddir" ]; then
        rm -rf $builddir/$SUBDIR
    fi
fi

echo

exit $result
