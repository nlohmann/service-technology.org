#!/bin/bash

############################################################################
# Copyright 2007 Peter Laufer                                              #
# Copyright 2008 Peter Massuthe                                            #
#                                                                          #
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

SUBDIR=oWFNreduction
DIR=$testdir/$SUBDIR
FIONA=fiona

# for make distcheck: make copy of $owfn and work on it
if [ "$testdir" != "$builddir" ]; then
    if [ ! -e $builddir/$SUBDIR ]; then
        $MKDIR_P $builddir/$SUBDIR
    fi
fi

rm -f $DIR/*.log
rm -f $DIR/060116-misc-coffee.reduced.owfn
rm -f $DIR/phcontrol3.unf.reduced.owfn
rm -f $DIR/06-03-23_BPM06_shop_sect_6.reduced.owfn

result=0

############################################################################
# misc Coffee reduction level 3
############################################################################

nodes_soll=60
transitions_soll=67

owfn="$DIR/060116-misc-coffee"
output="$builddir/$SUBDIR/060116-misc-coffee"
cmd="$FIONA $owfn.owfn -t reduce -o $output -p r4"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$og.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
    fionaExitCode=$?
    `$evaluate $fionaExitCode`
    if [ $? -ne 0 ] 
    then
    result=1
    else


    echo $OUTPUT | grep "|P|=$nodes_soll" > /dev/null
    nodes=$?

    echo $OUTPUT | grep "|T|=$transitions_soll" > /dev/null
    edges=$?

    if [ $nodes -ne 0 -o $edges -ne 0 ]
    then
    echo   ... failed to reduce the oWFN correctly
    fi

    result=`expr $result + $nodes + $edges`
    fi
fi

cmd="$FIONA $owfn.owfn $output.reduced.owfn -t equivalence"

if [ "$quiet" != "no" ]; then
    cmd="$cmd -Q"
fi

if [ "$memcheck" = "yes" ]; then
    memchecklog="$owfn.owfn.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
    fionaExitCode=$?
    `$evaluate $fionaExitCode`
    if [ $? -ne 0 ] 
    then
    result=1
    else


    echo $OUTPUT | grep "are equivalent: YES" > /dev/null
    resultSIM=$?
    if [ $resultSIM -ne 0 ]; then
        let "result += 1"
        echo ... equivalence check of the owfn and it's reduced version's OGs failed.
    fi
    fi
fi


############################################################################
# phcontrol3 reduction
############################################################################

nodes_soll=18
transitions_soll=7

owfn="$DIR/phcontrol3.unf"
output="$builddir/$SUBDIR/phcontrol3.unf"
cmd="$FIONA $owfn.owfn -t reduce -o $output"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$og.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
    fionaExitCode=$?
    `$evaluate $fionaExitCode`
    if [ $? -ne 0 ] 
    then
    result=1
    else


    echo $OUTPUT | grep "|P|=$nodes_soll" > /dev/null
    nodes=$?

    echo $OUTPUT | grep "|T|=$transitions_soll" > /dev/null
    edges=$?

    if [ $nodes -ne 0 -o $edges -ne 0 ]
    then
    echo   ... failed to reduce the oWFN correctly
    fi

    result=`expr $result + $nodes + $edges`
    fi
fi

cmd="$FIONA $owfn.owfn $output.reduced.owfn -t equivalence"

if [ "$quiet" != "no" ]; then
    cmd="$cmd -Q"
fi

if [ "$memcheck" = "yes" ]; then
    memchecklog="$owfn.owfn.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
    fionaExitCode=$?
    `$evaluate $fionaExitCode`
    if [ $? -ne 0 ] 
    then
    result=1
    else


    echo $OUTPUT | grep "are equivalent: NO" > /dev/null
    resultSIM=$?
    if [ $resultSIM -ne 0 ]; then
        let "result += 1"
        echo ... the net and the reduced net are equivalent so the PNapi was probably updated to handle finalconditions.
    fi
    fi
fi


############################################################################


############################################################################
# BPM06 Shop 6 reduction
############################################################################

nodes_soll=66
transitions_soll=68

owfn="$DIR/06-03-23_BPM06_shop_sect_6"
output="$builddir/$SUBDIR/06-03-23_BPM06_shop_sect_6"
cmd="$FIONA $owfn.owfn -t reduce -o $output"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$og.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
    fionaExitCode=$?
    `$evaluate $fionaExitCode`
    if [ $? -ne 0 ] 
    then
    result=1
    else


    echo $OUTPUT | grep "|P|=$nodes_soll" > /dev/null
    nodes=$?

    echo $OUTPUT | grep "|T|=$transitions_soll" > /dev/null
    edges=$?

    if [ $nodes -ne 0 -o $edges -ne 0 ]
    then
    echo   ... failed to reduce the oWFN correctly
    fi

    result=`expr $result + $nodes + $edges`
    fi
fi

cmd="$FIONA $owfn.owfn $output.reduced.owfn -t equivalence"

if [ "$quiet" != "no" ]; then
    cmd="$cmd -Q"
fi

if [ "$memcheck" = "yes" ]; then
    memchecklog="$owfn.owfn.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
    fionaExitCode=$?
    `$evaluate $fionaExitCode`
    if [ $? -ne 0 ] 
    then
    result=1
    else


    echo $OUTPUT | grep "are equivalent: YES" > /dev/null
    resultSIM=$?
    if [ $resultSIM -ne 0 ]; then
        let "result += 1"
        echo ... equivalence check of the owfn and it's reduced version's OGs failed.
    fi
    fi
fi


############################################################################

echo

exit $result

