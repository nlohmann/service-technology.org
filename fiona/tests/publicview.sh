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

SUBDIR=publicview
DIR=$testdir/$SUBDIR
FIONA=fiona

# for make distcheck: make copy of $owfn and work on it
if [ "$testdir" != "$builddir" ]; then
    if [ ! -e $builddir/$SUBDIR ]; then
        $MKDIR_P $builddir/$SUBDIR
    fi
fi

#loeschen aller erzeugten Dateien im letzten Durchlauf
rm -f $DIR/NonFinalTauNode1.pv.sa.out
rm -f $DIR/NonFinalTauNode1.pv.sa.png
rm -f $DIR/NonFinalTauNode1.pv.owfn
rm -f $DIR/NonFinalTauNode2.pv.sa.out
rm -f $DIR/NonFinalTauNode2.pv.sa.png
rm -f $DIR/NonFinalTauNode2.pv.owfn
rm -f $DIR/NonFinalTauNode3.pv.sa.out
rm -f $DIR/NonFinalTauNode3.pv.sa.png
rm -f $DIR/NonFinalTauNode3.pv.owfn
rm -f $DIR/NonFinalTauNode4.pv.sa.out
rm -f $DIR/NonFinalTauNode4.pv.sa.png
rm -f $DIR/NonFinalTauNode4.pv.owfn
rm -f $DIR/NonFinalTauNode1not.pv.sa.out
rm -f $DIR/NonFinalTauNode1not.pv.sa.png
rm -f $DIR/NonFinalTauNode1not.pv.owfn
rm -f $DIR/NonFinalTauNode2not.pv.sa.out
rm -f $DIR/NonFinalTauNode2not.pv.sa.png
rm -f $DIR/NonFinalTauNode2not.pv.owfn
rm -f $DIR/non_empty_true_node2.pv.sa.out
rm -f $DIR/non_empty_true_node2.pv.sa.png
rm -f $DIR/non_empty_true_node2.pv.owfn
rm -f $DIR/non_empty_true_node3.pv.sa.out
rm -f $DIR/non_empty_true_node3.pv.sa.png
rm -f $DIR/non_empty_true_node3.pv.owfn
rm -f $DIR/complexPV.pv.sa.out
rm -f $DIR/complexPV.pv.sa.png
rm -f $DIR/complexPV.pv.owfn
rm -f $DIR/*.log

service="$DIR/non_empty_true_node2"

result=0

############################################################################
# complexPV equivalence
############################################################################

service="$DIR/complexPV"
output="$builddir/$SUBDIR/complexPV"
cmd="$FIONA $service.owfn -t pv -o $output -p no-png"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$service.owfn.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
    exitcode=$?
fi

cmd="$FIONA $output.pv.owfn $service.owfn -t equivalence"

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

############################################################################
# non_empty_true_node2 equivalence
############################################################################

service="$DIR/non_empty_true_node2"
output="$builddir/$SUBDIR/non_empty_true_node2"
cmd="$FIONA $service.owfn -t pv -o $output -p no-png"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$service.owfn.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
    exitcode=$?
fi

cmd="$FIONA $output.pv.owfn $service.owfn -t equivalence"

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

############################################################################
# non_empty_true_node3 equivalence
############################################################################

service="$DIR/non_empty_true_node3"
output="$builddir/$SUBDIR/non_empty_true_node3"
cmd="$FIONA $service.owfn -t pv -o $output -p no-png"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$service.owfn.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
    exitcode=$?
fi

cmd="$FIONA $output.pv.owfn $service.owfn -t equivalence"

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
        echo ... equivalence check of the service's and its public view's OGs succeeded, which is not expected for this owfn.
    fi
fi


############################################################################
# start of the outcommented detailed test                                  #
############################################################################
<<OUTCOMMENTED

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

############################################################################
# NonFinalTauNode2 equivalence
############################################################################

service="$DIR/NonFinalTauNode2"
cmd="$FIONA $service.owfn -t pv -p no-png"

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

############################################################################
# NonFinalTauNode3 equivalence
############################################################################

service="$DIR/NonFinalTauNode3"
cmd="$FIONA $service.owfn -t pv -p no-png"

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

############################################################################
# NonFinalTauNode4 equivalence
############################################################################

service="$DIR/NonFinalTauNode4"
cmd="$FIONA $service.owfn -t pv -p no-png"

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

############################################################################
# NonFinalTauNode1not equivalence
############################################################################

service="$DIR/NonFinalTauNode1not"
cmd="$FIONA $service.owfn -t pv -p no-png"

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

############################################################################
# NonFinalTauNode2not equivalence
############################################################################

service="$DIR/NonFinalTauNode2not"
cmd="$FIONA $service.owfn -t pv -p no-png"

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


############################################################################
# end of the outcommented detailed test                                    #
############################################################################
OUTCOMMENTED


echo

exit $result

