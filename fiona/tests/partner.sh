#!/bin/bash

############################################################################
# Copyright 2007 Jan Bretschneider                                         #
#                                                                          #
# This file is part of Fiona.                                              #
#                                                                          #
# Fiona is free software; you can redistribute it and/or modify it         #
# under the terms of the GNU General Public License as published by the    #
# Free Software Foundation; either version 2 of the License, or (at your   #
# option) any later version.                                               #
#                                                                          #
# Fiona is distributed in the hope that it will be useful, but WITHOUT     #
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or    #
# FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for #
# more details.                                                            #
#                                                                          #
# You should have received a copy of the GNU General Public License along  #
# with Fiona; if not, write to the Free Software Foundation, Inc., 51      #
# Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.                     #
############################################################################

source defaults.sh
source memcheck_helper.sh

echo
echo ---------------------------------------------------------------------
echo running $0
echo

if test "$PETRIFY" = "not found"; then
  echo Note: fiona was built without Petrify support!
  echo
  echo skipping $0
  exit 0
fi

SUBDIR=partner
DIR=$testdir/$SUBDIR
FIONA=fiona

result=0

#loeschen aller erzeugten Dateien im letzten Durchlauf
rm -f $DIR/sequence3.owfn.*
rm -f $DIR/sequence3-partner.dot
rm -f $DIR/sequence3-partner.owfn
rm -f $DIR/sequence3-partner.png
rm -f $DIR/coffee.owfn.*
rm -f $DIR/coffee-partner.dot
rm -f $DIR/coffee-partner.owfn
rm -f $DIR/coffee-partner.png
rm -f $DIR/coffee2.owfn.*
rm -f $DIR/coffee2-partner.dot
rm -f $DIR/coffee2-partner.owfn
rm -f $DIR/coffee2-partner.png
rm -f $DIR/coffee_extended.owfn.*
rm -f $DIR/coffee_extended-partner.dot
rm -f $DIR/coffee_extended-partner.owfn
rm -f $DIR/coffee_extended-partner.png

############################################################################
# Building most permissive partner for sequence3 out of oWFN               #
############################################################################
#resultSingle=0
input="$DIR/sequence3.owfn"
inputCopy="$builddir/$SUBDIR/sequence3.owfn"
output="$builddir/$SUBDIR/sequence3-partner.owfn"
outputExpected="$testdir/$SUBDIR/sequence3-partner.expected.owfn"


# for make distcheck: make copy of $input and work on it
if [ "$testdir" != "$builddir" ]; then
    if [ ! -e $builddir/$SUBDIR ]; then
        $MKDIR_P $builddir/$SUBDIR
    fi

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
    if [ $? -ne 0 ]; then
        let "result += 1"
        echo "... failed: $FIONA exited with non-zero return value."
        #resultSingle=1
    else
        echo running $check
        OUTPUT=`$check 2>&1`
        echo $OUTPUT | grep "are equivalent: YES" > /dev/null
        if [ $? -ne 0 ]; then
            let "result += 1"
            echo ... computed partner not equivalent to expected partner.
        #else
            #if ! diff "$output" "$outputExpected" >/dev/null ; then
            #    echo "... failed: Output and expected output differ. Compare " \
            #        "$output" "$outputExpected"
            #    resultSingle=1
            #fi
        fi
    fi
fi

#if [ $resultSingle -ne 0 ]; then
#    result=1
#fi

#rm -f $DIR/sequence3.owfn.* we need the OG for the next test
rm -f $DIR/sequence3-partner.dot
rm -f $DIR/sequence3-partner.owfn
rm -f $DIR/sequence3-partner.png

############################################################################
# Building most permissive partner for sequence3 out of OG                 #
############################################################################
input="$builddir/$SUBDIR/sequence3.owfn.og"
inputCopy="$builddir/$SUBDIR/sequence3.owfn.og"
output="$builddir/$SUBDIR/sequence3-partner.owfn"
outputExpected="$testdir/$SUBDIR/sequence3-partner.expected.owfn"


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
    if [ $? -ne 0 ]; then
        let "result += 1"
        echo "... failed: $FIONA exited with non-zero return value."
    else
        echo running $check
        OUTPUT=`$check 2>&1`
        echo $OUTPUT | grep "are equivalent: YES" > /dev/null
        if [ $? -ne 0 ]; then
            let "result += 1"
            echo ... computed partner not equivalent to expected partner.
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
    if [ $? -ne 0 ]; then
        let "result += 1"
        echo "... failed: $FIONA exited with non-zero return value."
    else
        echo running $check
        OUTPUT=`$check 2>&1`
        echo $OUTPUT | grep "are equivalent: YES" > /dev/null
        if [ $? -ne 0 ]; then
            let "result += 1"
            echo ... computed partner not equivalent to expected partner.
        fi
        echo running $check2
        OUTPUT=`$check2 2>&1`
        echo $OUTPUT | grep "are equivalent: YES" > /dev/null
        if [ $? -ne 0 ]; then
            let "result += 1"
            echo ... computed partner not equivalent to expected partner.
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
    if [ $? -ne 0 ]; then
        let "result += 1"
        echo "... failed: $FIONA exited with non-zero return value."
    else
        echo running $check
        OUTPUT=`$check 2>&1`
        echo $OUTPUT | grep "are equivalent: YES" > /dev/null
        if [ $? -ne 0 ]; then
            let "result += 1"
            echo ... computed partner not equivalent to expected partner.
        fi
        echo running $check2
        OUTPUT=`$check2 2>&1`
        echo $OUTPUT | grep "are equivalent: YES" > /dev/null
        if [ $? -ne 0 ]; then
            let "result += 1"
            echo ... computed partner not equivalent to expected partner.
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
    if [ $? -ne 0 ]; then
        let "result += 1"
        echo "... failed: $FIONA exited with non-zero return value."
    else
        echo running $check
        OUTPUT=`$check 2>&1`
        echo $OUTPUT | grep "are equivalent: YES" > /dev/null
        if [ $? -ne 0 ]; then
            let "result += 1"
            echo ... computed partner not equivalent to expected partner.
        fi
    fi
fi

###########################################################################
if [ $result -ne 0 ]; then
    if [ "$testdir" != "$builddir" ]; then
        rm -rf $builddir/$SUBDIR
    fi
fi

#echo $result
exit $result
