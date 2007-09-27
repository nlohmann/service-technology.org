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

SUBDIR=formula
DIR=$testdir/$SUBDIR
FIONA=fiona

#loeschen aller erzeugten Dateien im letzten Durchlauf
rm -f $DIR/*output*

result=0

############################################################################
resultSingle=0
owfn="$DIR/1.owfn"
outputPrefix="$builddir/formula/1.owfn.output"
outputExpected="$testdir/formula/1.owfn.expected.og"

# for make distcheck: make copy of $owfn and work on it
if [ "$testdir" != "$builddir" ]; then
    if [ ! -e $builddir/$SUBDIR ]; then
        $MKDIR_P $builddir/$SUBDIR
    fi
fi

cmd="$FIONA -t og -o $outputPrefix $owfn"

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
