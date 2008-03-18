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

DIR=$testdir/oWFNreduction
FIONA=fiona

result=0

############################################################################
# misc Coffee reduction level 1
############################################################################

nodes_soll=76
transitions_soll=73

owfn="$DIR/060116-misc-coffee"
cmd="$FIONA $owfn.owfn -t reduce -l 1"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$og.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`

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

cmd="$FIONA $owfn.owfn $owfn.reduced.owfn -t equivalence"

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

    echo $OUTPUT | grep "are equivalent: YES" > /dev/null
    resultSIM=$?
    if [ $resultSIM -ne 0 ]; then
        let "result += 1"
        echo ... equivalence check of the owfn and it's reduced version's OGs failed.
    fi
fi


############################################################################
# misc Coffee reduction level 2
############################################################################

nodes_soll=63
transitions_soll=70

owfn="$DIR/060116-misc-coffee"
cmd="$FIONA $owfn.owfn -t reduce -l 2"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$og.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`

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

cmd="$FIONA $owfn.owfn $owfn.reduced.owfn -t equivalence"

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

    echo $OUTPUT | grep "are equivalent: YES" > /dev/null
    resultSIM=$?
    if [ $resultSIM -ne 0 ]; then
        let "result += 1"
        echo ... equivalence check of the owfn and it's reduced version's OGs failed.
    fi
fi


############################################################################
# misc Coffee reduction level 3
############################################################################

nodes_soll=62
transitions_soll=67

owfn="$DIR/060116-misc-coffee"
cmd="$FIONA $owfn.owfn -t reduce -l 3"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$og.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`

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

cmd="$FIONA $owfn.owfn $owfn.reduced.owfn -t equivalence"

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

    echo $OUTPUT | grep "are equivalent: YES" > /dev/null
    resultSIM=$?
    if [ $resultSIM -ne 0 ]; then
        let "result += 1"
        echo ... equivalence check of the owfn and it's reduced version's OGs failed.
    fi
fi


############################################################################
# misc Coffee reduction level 4
############################################################################

nodes_soll=55
transitions_soll=62

owfn="$DIR/060116-misc-coffee"
cmd="$FIONA $owfn.owfn -t reduce -l 4"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$og.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`

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

cmd="$FIONA $owfn.owfn $owfn.reduced.owfn -t equivalence"

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

    echo $OUTPUT | grep "are equivalent: YES" > /dev/null
    resultSIM=$?
    if [ $resultSIM -ne 0 ]; then
        let "result += 1"
        echo ... equivalence check of the owfn and it's reduced version's OGs failed.
    fi
fi


############################################################################
# misc Coffee reduction level 5
############################################################################

nodes_soll=53
transitions_soll=62

owfn="$DIR/060116-misc-coffee"
cmd="$FIONA $owfn.owfn -t reduce -l 5"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$og.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`

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

cmd="$FIONA $owfn.owfn $owfn.reduced.owfn -t equivalence"

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

    echo $OUTPUT | grep "are equivalent: YES" > /dev/null
    resultSIM=$?
    if [ $resultSIM -ne 0 ]; then
        let "result += 1"
        echo ... equivalence check of the owfn and it's reduced version's OGs failed.
    fi
fi


############################################################################
# phcontrol3 reduction
############################################################################

nodes_soll=15
transitions_soll=6

owfn="$DIR/phcontrol3.unf"
cmd="$FIONA $owfn.owfn -t reduce"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$og.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`

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

cmd="$FIONA $owfn.owfn $owfn.reduced.owfn -t equivalence"

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

    echo $OUTPUT | grep "are equivalent: YES" > /dev/null
    resultSIM=$?
    if [ $resultSIM -ne 0 ]; then
        let "result += 1"
        echo ... equivalence check of the owfn and it's reduced version's OGs failed.
    fi
fi


############################################################################


############################################################################
# BPM06 Shop 6 reduction
############################################################################

nodes_soll=37
transitions_soll=20

owfn="$DIR/06-03-23_BPM06_shop_sect_6"
cmd="$FIONA $owfn.owfn -t reduce"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$og.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`

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

cmd="$FIONA $owfn.owfn $owfn.reduced.owfn -t equivalence"

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

    echo $OUTPUT | grep "are equivalent: YES" > /dev/null
    resultSIM=$?
    if [ $resultSIM -ne 0 ]; then
        let "result += 1"
        echo ... equivalence check of the owfn and it's reduced version's OGs failed.
    fi
fi


############################################################################

echo

exit $result
