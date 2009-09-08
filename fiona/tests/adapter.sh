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



SUBDIR=adapter
DIR=$testdir/$SUBDIR
FIONA=fiona
result=0


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

#loeschen aller erzeugten Dateien im letzten Durchlauf
rm -f $DIR/*.log

############################################################################
############################################################################
# Building an adapter for a disjoint tea/coffee machine and a coffee drinker
############################################################################
############################################################################

owfn1="$DIR/coffee_disjoint.owfn"
owfn1copy="$builddir/$SUBDIR/coffee_disjoint.owfn"
owfn2="$DIR/machine_disjoint.owfn"
owfn2copy="$builddir/$SUBDIR/machine_disjoint.owfn"
rules="$DIR/vending_disjoint.ar"
rulescopy="$builddir/$SUBDIR/vending_disjoint.ar"
outprefix="$builddir/$SUBDIR/sd_"
outowfn="$builddir/$SUBDIR/sd_adaptertest.owfn"


# for make distcheck: create directory $builddir/$SUBDIR for writing output files to
if [ "$testdir" != "$builddir" ]; then
    if [ ! -e $builddir/$SUBDIR ]; then
        $MKDIR_P $builddir/$SUBDIR
    fi

    # make copy of input and work on it
    cp $owfn1 $owfn1copy
    cp $owfn2 $owfn2copy
    cp $rules $rulescopy
fi

cmd="$FIONA -t smalladapter $owfn1copy $owfn2copy -a $rulescopy -o $outprefix -d 3"
check="$FIONA $outowfn"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$owfn1.memcheck.log"
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
            echo $OUTPUT | grep ": YES" > /dev/null
            if [ $? -ne 0 ]; then
                echo ... composition is not controllable although it should be
                result=1
            fi
        fi
    fi
fi

cmd="$FIONA -t adapter $owfn1copy $owfn2copy -a $rulescopy -o $outprefix -d 3"
check="$FIONA $outowfn"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$owfn1.memcheck.log"
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
            echo $OUTPUT | grep ": YES" > /dev/null
            if [ $? -ne 0 ]; then
                echo ... composition is not controllable although it should be
                result=1
            fi
        fi
    fi
fi

############################################################################
############################################################################
# Building an adapter for a joint tea/coffee machine and a coffee drinker
############################################################################
############################################################################

owfn1="$DIR/coffee_joint.owfn"
owfn1copy="$builddir/$SUBDIR/coffee_joint.owfn"
owfn2="$DIR/machine_joint.owfn"
owfn2copy="$builddir/$SUBDIR/machine_joint.owfn"
rules="$DIR/vending_joint.ar"
rulescopy="$builddir/$SUBDIR/vending_joint.ar"
outprefix="$builddir/$SUBDIR/sd_"
outowfn="$builddir/$SUBDIR/sd_adaptertest.owfn"


# for make distcheck: create directory $builddir/$SUBDIR for writing output files to
if [ "$testdir" != "$builddir" ]; then
    if [ ! -e $builddir/$SUBDIR ]; then
        $MKDIR_P $builddir/$SUBDIR
    fi

    # make copy of input and work on it
    cp $owfn1 $owfn1copy
    cp $owfn2 $owfn2copy
    cp $rules $rulescopy
fi

cmd="$FIONA -t smalladapter $owfn1copy $owfn2copy -a $rulescopy -o $outprefix -d 3"
check="$FIONA $outowfn"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$owfn1.memcheck.log"
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
            echo $OUTPUT | grep ": YES" > /dev/null
            if [ $? -ne 0 ]; then
                echo ... composition is not controllable although it should be
                result=1
            fi
        fi
    fi
fi

cmd="$FIONA -t adapter $owfn1copy $owfn2copy -a $rulescopy -o $outprefix -d 3"
check="$FIONA $outowfn"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$owfn1.memcheck.log"
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
            echo $OUTPUT | grep ": YES" > /dev/null
            if [ $? -ne 0 ]; then
                echo ... composition is not controllable although it should be
                result=1
            fi
        fi
    fi
fi



############################################################################
############################################################################
# Building an adapter for a disjoint tea/coffee machine and a coffee drinker
# with partial communication flow
############################################################################
############################################################################

owfn1="$DIR/coffee_disjoint.owfn"
owfn1copy="$builddir/$SUBDIR/coffee_disjoint.owfn"
owfn2="$DIR/machine_disjoint.owfn"
owfn2copy="$builddir/$SUBDIR/machine_disjoint.owfn"
rules="$DIR/vending_partial0_disjoint.ar"
rulescopy="$builddir/$SUBDIR/vending_partial0_disjoint.ar"
outprefix="$builddir/$SUBDIR/sd_"
outowfn="$builddir/$SUBDIR/sd_adaptertest.owfn"


# for make distcheck: create directory $builddir/$SUBDIR for writing output files to
if [ "$testdir" != "$builddir" ]; then
    if [ ! -e $builddir/$SUBDIR ]; then
        $MKDIR_P $builddir/$SUBDIR
    fi

    # make copy of input and work on it
    cp $owfn1 $owfn1copy
    cp $owfn2 $owfn2copy
    cp $rules $rulescopy
fi

cmd="$FIONA -t smalladapter $owfn1copy $owfn2copy -a $rulescopy -o $outprefix -d 3"
check="$FIONA $outowfn"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$owfn1.memcheck.log"
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
            echo $OUTPUT | grep ": YES" > /dev/null
            if [ $? -ne 0 ]; then
                echo ... composition is not controllable although it should be
                result=1
            fi
        fi
    fi
fi

cmd="$FIONA -t adapter $owfn1copy $owfn2copy -a $rulescopy -o $outprefix -d 3"
check="$FIONA $outowfn"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$owfn1.memcheck.log"
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
            echo $OUTPUT | grep ": YES" > /dev/null
            if [ $? -ne 0 ]; then
                echo ... composition is not controllable although it should be
                result=1
            fi
        fi
    fi
fi

############################################################################

rules="$DIR/vending_partial1_disjoint.ar"
rulescopy="$builddir/$SUBDIR/vending_partial1_disjoint.ar"
outprefix="$builddir/$SUBDIR/sd_"
outowfn="$builddir/$SUBDIR/sd_adaptertest.owfn"


# for make distcheck: create directory $builddir/$SUBDIR for writing output files to
if [ "$testdir" != "$builddir" ]; then
    if [ ! -e $builddir/$SUBDIR ]; then
        $MKDIR_P $builddir/$SUBDIR
    fi

    # make copy of input and work on it
    cp $owfn1 $owfn1copy
    cp $owfn2 $owfn2copy
    cp $rules $rulescopy
fi

cmd="$FIONA -t smalladapter $owfn1copy $owfn2copy -a $rulescopy -o $outprefix -d 3"
check="$FIONA $outowfn"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$owfn1.memcheck.log"
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
            echo $OUTPUT | grep ": YES" > /dev/null
            if [ $? -ne 0 ]; then
                echo ... composition is not controllable although it should be
                result=1
            fi
        fi
    fi
fi

cmd="$FIONA -t adapter $owfn1copy $owfn2copy -a $rulescopy -o $outprefix -d 3"
check="$FIONA $outowfn"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$owfn1.memcheck.log"
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
            echo $OUTPUT | grep ": YES" > /dev/null
            if [ $? -ne 0 ]; then
                echo ... composition is not controllable although it should be
                result=1
            fi
        fi
    fi
fi

############################################################################

rules="$DIR/vending_partial2_disjoint.ar"
rulescopy="$builddir/$SUBDIR/vending_partial2_disjoint.ar"
outprefix="$builddir/$SUBDIR/sd_"
outowfn="$builddir/$SUBDIR/sd_adaptertest.owfn"


# for make distcheck: create directory $builddir/$SUBDIR for writing output files to
if [ "$testdir" != "$builddir" ]; then
    if [ ! -e $builddir/$SUBDIR ]; then
        $MKDIR_P $builddir/$SUBDIR
    fi

    # make copy of input and work on it
    cp $owfn1 $owfn1copy
    cp $owfn2 $owfn2copy
    cp $rules $rulescopy
fi

cmd="$FIONA -t smalladapter $owfn1copy $owfn2copy -a $rulescopy -o $outprefix -d 3"
check="$FIONA $outowfn"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$owfn1.memcheck.log"
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
            echo $OUTPUT | grep ": YES" > /dev/null
            if [ $? -ne 0 ]; then
                echo ... composition is not controllable although it should be
                result=1
            fi
        fi
    fi
fi

cmd="$FIONA -t adapter $owfn1copy $owfn2copy -a $rulescopy -o $outprefix -d 3"
check="$FIONA $outowfn"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$owfn1.memcheck.log"
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
            echo $OUTPUT | grep ": YES" > /dev/null
            if [ $? -ne 0 ]; then
                echo ... composition is not controllable although it should be
                result=1
            fi
        fi
    fi
fi

############################################################################

# remove all created files
rm -f $DIR/sd_*.*

if [ $result -eq 0 ]; then
    if [ "$testdir" != "$builddir" ]; then
        rm -rf $builddir/$SUBDIR
    fi
fi

echo
exit $result
