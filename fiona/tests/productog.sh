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

SUBDIR=productog
DIR=$testdir/$SUBDIR
FIONA=fiona

#loeschen aller erzeugten Dateien im letzten Durchlauf
rm -f $DIR/*output*

result=0

############################################################################
# Building product of two OGs.                                             #
############################################################################
resultSingle=0
og1="$DIR/1.og"
og2="$DIR/2.og"
outputPrefix="$builddir/productog/1_X_2.output"
outputExpected="$builddir/productog/1_X_2.expected.og"

# for make distcheck: make copy of $owfn and work on it
if [ "$testdir" != "$builddir" ]; then
    if [ ! -e $builddir/$SUBDIR ]; then
        $MKDIR_P $builddir/$SUBDIR
    fi
fi

cmd="$FIONA -t productog -o $outputPrefix $og1 $og2"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$outputPrefix.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
    if [ $? -ne 0 ]; then
        echo "... failed: $FIONA exited with non-zero return value."
        resultSingle=1
    fi

    if [ $resultSingle -eq 0 ] ; then
        if ! diff "$outputPrefix.og" "$outputExpected" >/dev/null ; then
            echo "... failed: Output and expected output differ. Compare " \
                 "$outputPrefix.og" "$outputExpected"
            resultSingle=1
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
outputPrefix="$builddir/productog/1_X_2_X_3.output"
outputExpected="$builddir/productog/1_X_2_X_3.expected.og"

# for make distcheck: make copy of $owfn and work on it
if [ "$testdir" != "$builddir" ]; then
    if [ ! -e $builddir/$SUBDIR ]; then
        $MKDIR_P $builddir/$SUBDIR
    fi
fi

cmd="$FIONA -t productog -o $outputPrefix $og1 $og2 $og3"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$outputPrefix.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
    if [ $? -ne 0 ]; then
        echo "... failed: $FIONA exited with non-zero return value."
        resultSingle=1
    fi

    if [ $resultSingle -eq 0 ] ; then
        if ! diff "$outputPrefix.og" "$outputExpected" >/dev/null ; then
            echo "... failed: Output and expected output differ. Compare " \
                 "$outputPrefix.og" "$outputExpected"
            resultSingle=1
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
outputPrefix="$builddir/productog/3_X_1_X_2.output"
outputExpected="$builddir/productog/3_X_1_X_2.expected.og"

# for make distcheck: make copy of $owfn and work on it
if [ "$testdir" != "$builddir" ]; then
    if [ ! -e $builddir/$SUBDIR ]; then
        $MKDIR_P $builddir/$SUBDIR
    fi
fi

cmd="$FIONA -t productog -o $outputPrefix $og3 $og1 $og2"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$outputPrefix.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
    if [ $? -ne 0 ]; then
        echo "... failed: $FIONA exited with non-zero return value."
        resultSingle=1
    fi

    if [ $resultSingle -eq 0 ] ; then
        if ! diff "$outputPrefix.og" "$outputExpected" >/dev/null ; then
            echo "... failed: Output and expected output differ. Compare " \
                 "$outputPrefix.og" "$outputExpected"
            resultSingle=1
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
