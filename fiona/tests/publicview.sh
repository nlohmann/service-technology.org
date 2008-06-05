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

DIR=$testdir/publicview
FIONA=fiona

#loeschen aller erzeugten Dateien im letzten Durchlauf
rm -f $DIR/*.out
rm -f $DIR/*.png
rm -f $DIR/*.log

result=0

############################################################################
# NonFinalTauNode1 equivalence
############################################################################

service="$DIR/NonFinalTauNode1"
cmd="$FIONA $service.owfn -t pv"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$service.owfn.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
    exitcode=$?
fi

cmd="$FIONA $service.pv.owfn $service.owfn -t equivalence"

if [ "$quiet" != "no" ]; then
    cmd="$cmd -Q"
fi

if [ "$memcheck" = "yes" ]; then
    memchecklog="$service.sa.owfn.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
    exitcode=$?

    echo $OUTPUT | grep "are equivalent: YES" > /dev/null
    resultSIM=$?
    if [ $exitcode -ne 0 -o $resultSIM -ne 0 ]; then
        let "result += 1"
        echo ... equivalence check of the service's and its public view's OGs failed.
    fi
fi

rm $service.pv.sa.out
rm $service.pv.sa.png
rm $service.pv.owfn

############################################################################
# NonFinalTauNode2 equivalence
############################################################################

service="$DIR/NonFinalTauNode2"
cmd="$FIONA $service.owfn -t pv"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$service.owfn.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
    exitcode=$?
fi

cmd="$FIONA $service.pv.owfn $service.owfn -t equivalence"

if [ "$quiet" != "no" ]; then
    cmd="$cmd -Q"
fi

if [ "$memcheck" = "yes" ]; then
    memchecklog="$service.sa.owfn.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
    exitcode=$?

    echo $OUTPUT | grep "are equivalent: YES" > /dev/null
    resultSIM=$?
    if [ $exitcode -ne 0 -o $resultSIM -ne 0 ]; then
        let "result += 1"
        echo ... equivalence check of the service's and its public view's OGs failed.
    fi
fi

rm $service.pv.sa.out
rm $service.pv.sa.png
rm $service.pv.owfn

############################################################################
# NonFinalTauNode3 equivalence
############################################################################

service="$DIR/NonFinalTauNode3"
cmd="$FIONA $service.owfn -t pv"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$service.owfn.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
    exitcode=$?
fi

cmd="$FIONA $service.pv.owfn $service.owfn -t equivalence"

if [ "$quiet" != "no" ]; then
    cmd="$cmd -Q"
fi

if [ "$memcheck" = "yes" ]; then
    memchecklog="$service.sa.owfn.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
    exitcode=$?

    echo $OUTPUT | grep "are equivalent: YES" > /dev/null
    resultSIM=$?
    if [ $exitcode -ne 0 -o $resultSIM -ne 0 ]; then
        let "result += 1"
        echo ... equivalence check of the service's and its public view's OGs failed.
    fi
fi

rm $service.pv.sa.out
rm $service.pv.sa.png
rm $service.pv.owfn

############################################################################
# NonFinalTauNode4 equivalence
############################################################################

service="$DIR/NonFinalTauNode4"
cmd="$FIONA $service.owfn -t pv"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$service.owfn.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
    exitcode=$?
fi

cmd="$FIONA $service.pv.owfn $service.owfn -t equivalence"

if [ "$quiet" != "no" ]; then
    cmd="$cmd -Q"
fi

if [ "$memcheck" = "yes" ]; then
    memchecklog="$service.sa.owfn.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
    exitcode=$?

    echo $OUTPUT | grep "are equivalent: YES" > /dev/null
    resultSIM=$?
    if [ $exitcode -ne 0 -o $resultSIM -ne 0 ]; then
        let "result += 1"
        echo ... equivalence check of the service's and its public view's OGs failed.
    fi
fi

rm $service.pv.sa.out
rm $service.pv.sa.png
rm $service.pv.owfn

############################################################################
# NonFinalTauNode1not equivalence
############################################################################

service="$DIR/NonFinalTauNode1not"
cmd="$FIONA $service.owfn -t pv"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$service.owfn.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
    exitcode=$?
fi

cmd="$FIONA $service.pv.owfn $service.owfn -t equivalence"

if [ "$quiet" != "no" ]; then
    cmd="$cmd -Q"
fi

if [ "$memcheck" = "yes" ]; then
    memchecklog="$service.sa.owfn.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
    exitcode=$?

    echo $OUTPUT | grep "are equivalent: YES" > /dev/null
    resultSIM=$?
    if [ $exitcode -ne 0 -o $resultSIM -ne 0 ]; then
        let "result += 1"
        echo ... equivalence check of the service's and its public view's OGs failed.
    fi
fi

rm $service.pv.sa.out
rm $service.pv.sa.png
rm $service.pv.owfn

############################################################################
# NonFinalTauNode2not equivalence
############################################################################

service="$DIR/NonFinalTauNode2not"
cmd="$FIONA $service.owfn -t pv"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$service.owfn.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
    exitcode=$?
fi

cmd="$FIONA $service.pv.owfn $service.owfn -t equivalence"

if [ "$quiet" != "no" ]; then
    cmd="$cmd -Q"
fi

if [ "$memcheck" = "yes" ]; then
    memchecklog="$service.sa.owfn.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
    exitcode=$?

    echo $OUTPUT | grep "are equivalent: YES" > /dev/null
    resultSIM=$?
    if [ $exitcode -ne 0 -o $resultSIM -ne 0 ]; then
        let "result += 1"
        echo ... equivalence check of the service's and its public view's OGs failed.
    fi
fi

rm $service.pv.sa.out
rm $service.pv.sa.png
rm $service.pv.owfn

############################################################################
# non_empty_true_node equivalence
############################################################################

service="$DIR/non_empty_true_node"
cmd="$FIONA $service.owfn -t pv"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$service.owfn.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
    exitcode=$?
fi

cmd="$FIONA $service.pv.owfn $service.owfn -t equivalence"

if [ "$quiet" != "no" ]; then
    cmd="$cmd -Q"
fi

if [ "$memcheck" = "yes" ]; then
    memchecklog="$service.sa.owfn.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
    exitcode=$?

    echo $OUTPUT | grep "are equivalent: YES" > /dev/null
    resultSIM=$?
    if [ $exitcode -ne 0 -o $resultSIM -ne 0 ]; then
        let "result += 1"
        echo ... equivalence check of the service's and its public view's OGs failed.
    fi
fi

rm $service.pv.sa.out
rm $service.pv.sa.png
rm $service.pv.owfn

############################################################################
# non_empty_true_node2 equivalence
############################################################################

service="$DIR/non_empty_true_node2"
cmd="$FIONA $service.owfn -t pv"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$service.owfn.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
    exitcode=$?
fi

cmd="$FIONA $service.pv.owfn $service.owfn -t equivalence"

if [ "$quiet" != "no" ]; then
    cmd="$cmd -Q"
fi

if [ "$memcheck" = "yes" ]; then
    memchecklog="$service.sa.owfn.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
    exitcode=$?

    echo $OUTPUT | grep "are equivalent: NO" > /dev/null
    resultSIM=$?
    if [ $exitcode -ne 0 -o $resultSIM -ne 0 ]; then
        let "result += 1"
        echo ... equivalence check of the service's and its public view's OGs failed.
    fi
fi

rm $service.pv.sa.out
rm $service.pv.sa.png
rm $service.pv.owfn

############################################################################
# twoWays equivalence
############################################################################

service="$DIR/twoWays"
cmd="$FIONA $service.owfn -t pv"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$service.owfn.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
    exitcode=$?
fi

cmd="$FIONA $service.pv.owfn $service.owfn -t equivalence"

if [ "$quiet" != "no" ]; then
    cmd="$cmd -Q"
fi

if [ "$memcheck" = "yes" ]; then
    memchecklog="$service.sa.owfn.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
    exitcode=$?

    echo $OUTPUT | grep "are equivalent: YES" > /dev/null
    resultSIM=$?
    if [ $exitcode -ne 0 -o $resultSIM -ne 0 ]; then
        let "result += 1"
        echo ... equivalence check of the service's and its public view's OGs failed.
    fi
fi

rm $service.pv.sa.out
rm $service.pv.sa.png
rm $service.pv.owfn

############################################################################
# complexPV equivalence
############################################################################

service="$DIR/complexPV"
cmd="$FIONA $service.owfn -t pv"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$service.owfn.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
    exitcode=$?
fi

cmd="$FIONA $service.pv.owfn $service.owfn -t equivalence"

if [ "$quiet" != "no" ]; then
    cmd="$cmd -Q"
fi

if [ "$memcheck" = "yes" ]; then
    memchecklog="$service.sa.owfn.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
    exitcode=$?

    echo $OUTPUT | grep "are equivalent: YES" > /dev/null
    resultSIM=$?
    if [ $exitcode -ne 0 -o $resultSIM -ne 0 ]; then
        let "result += 1"
        echo ... equivalence check of the service's and its public view's OGs failed.
    fi
fi

rm $service.pv.sa.out
rm $service.pv.sa.png
rm $service.pv.owfn

echo

exit $result
