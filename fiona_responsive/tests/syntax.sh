#!/bin/bash

############################################################################
# Copyright 2005, 2006 Peter Massuthe, Daniela Weinberg, Dennis Reinert,   #
#                      Jan Bretschneider and Christian Gierds              #
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

DIR=$testdir/syntax
FIONA=fiona

#loeschen aller erzeugten Dateien im letzten Durchlauf
rm -f $DIR/formula.owfn.output.og
rm -f $DIR/formula.owfn.output.og.out
rm -f $DIR/*.log

result=0

############################################################################

owfn="$DIR/max_occurrence.owfn"
cmd="$FIONA $owfn -t OG"
if [ "$quiet" != "no" ]; then
    cmd="$cmd -Q"
fi

if [ "$memcheck" = "yes" ]; then
    memchecklog="$owfn.OG.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
    if [ $? -ne 0 ]; then
        echo ... fiona exited with nonzero return value although it should not
        result=1
    fi
fi

############################################################################

owfn="$DIR/syntax_example.owfn"
cmd="$FIONA $owfn -t IG"

if [ "$quiet" != "no" ]; then
    cmd="$cmd -Q"
fi

if [ "$memcheck" = "yes" ]; then
    memchecklog="$owfn.OG.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`

    echo $OUTPUT | grep "net is controllable: YES" > /dev/null
    syntaxexample=$?


    if [ $syntaxexample -ne 0 ]
    then
    echo   ... failed to build OG correctly
    fi

    result=`expr $result + $syntaxexample`
fi

############################################################################

owfn="$DIR/syntax_example_new.owfn"
cmd="$FIONA $owfn -t IG"

if [ "$quiet" != "no" ]; then
    cmd="$cmd -Q"
fi

if [ "$memcheck" = "yes" ]; then
    memchecklog="$owfn.OG.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`

    echo $OUTPUT | grep "net is controllable: YES" > /dev/null
    syntaxexamplenew=$?


    if [ $syntaxexamplenew -ne 0 ]
    then
    echo   ... failed to build OG correctly
    fi

    result=`expr $result + $syntaxexamplenew`
fi

############################################################################

owfn="$DIR/syntax_example_new_short.owfn"
cmd="$FIONA $owfn -t IG"

if [ "$quiet" != "no" ]; then
    cmd="$cmd -Q"
fi

if [ "$memcheck" = "yes" ]; then
    memchecklog="$owfn.OG.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`

    echo $OUTPUT | grep "net is controllable: YES" > /dev/null
    syntaxexamplenew=$?


    if [ $syntaxexamplenew -ne 0 ]
    then
    echo   ... failed to build OG correctly
    fi

    result=`expr $result + $syntaxexamplenew`
fi

############################################################################
resultSingle=0
owfn="$DIR/formula.owfn"
outputPrefix="$builddir/syntax/formula.owfn.output"
outputExpected="$testdir/syntax/formula.owfn.expected.og"

# for make distcheck: make copy of $owfn and work on it
if [ "$testdir" != "$builddir" ]; then
    if [ ! -e $builddir/$SUBDIR ]; then
        $MKDIR_P $builddir/$SUBDIR
    fi
fi

cmd="$FIONA $owfn -t og -o $outputPrefix -p no-png"

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
        echo "running diff  $outputPrefix.og $outputExpected"
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

echo

exit $result
