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

source memcheck_helper.sh

echo
echo ---------------------------------------------------------------------
echo running $0
echo

testdir=.
DIR=$testdir/sequence_suite
FIONA=fiona

#loeschen aller erzeugten Dateien im letzten Durchlauf
rm -f $DIR/*.out
rm -f $DIR/*.png
#rm -f $DIR/*.cudd
rm -f $DIR/*.og
rm -f $DIR/*.log

############################################################################

owfn="$DIR/sequence5.owfn"
cmd="$FIONA -n $owfn -a -b 4 -t OG"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$owfn.a.b4.IG.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$?
else
    echo running $cmd
    $cmd  2>/dev/null 1>/dev/null
    result5=$?

    if [ $result5 -ne 0 ] ; then
        echo     ... FAILED
        echo
    fi

    if test \( $result5 -eq 0 \) -a \
        \( -f $DIR/sequence5.owfn.a.OG.png -a -f $DIR/sequence5.owfn.a.OG.out \)
    then
      result=0
    else
      result=1
    fi
fi

############################################################################

nodes_mp=28
nodes_ann=16

owfn="$DIR/sequence3.owfn"
cmd="$FIONA -n $owfn -a -b 4 -t OG"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$owfn.a.b4.IG.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$?
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
    echo $OUTPUT | grep "BDD_MP: number of nodes: $nodes_mp" > /dev/null
    resultNodesMP=$?
    echo $OUTPUT | grep "BDD_ANN: number of nodes: $nodes_ann" > /dev/null
    resultNodesANN=$?
    
    if [ $resultNodesMP -ne 0 ] ; then
        echo     ... FAILED
        echo Number of nodes of BDD_MP is incorrect.
        echo
        result=1
    fi

    if [ $resultNodesANN -ne 0 ] ; then
        echo     ... FAILED
        echo Number of nodes of BDD_ANN is incorrect.
        echo
        result=1
    fi

fi

############################################################################

owfn="$DIR/sequence3.owfn"
owfn_reference="$testdir/bdd_ref/sequence3_reference.owfn"
cmd="$FIONA -n $owfn -a -b 4 -t OG"
$cmd 2>/dev/null 1>/dev/null

cmd="$FIONA -n $owfn -n $owfn_reference -x"


if [ "$memcheck" = "yes" ]; then
    memchecklog="$owfn.n.n.x.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$?
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
    echo $OUTPUT | grep "The two operating guidelines are equal: YES" > /dev/null
    resultEqual=$?
    
    if [ $resultEqual -ne 0 ] ; then
        echo     ... FAILED
        echo The two operating guidelines are not equal although they should be.
        echo
        result=1
    fi
fi

############################################################################
############################################################################

nodes_mp=1562
nodes_ann=361

owfn="$testdir/philosophers/phcontrol4.unf.owfn"
cmd="$FIONA -n $owfn -a -b 4 -t OG"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$owfn.a.b4.IG.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$?
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
    echo $OUTPUT | grep "BDD_MP: number of nodes: $nodes_mp" > /dev/null
    resultNodesMP=$?
    echo $OUTPUT | grep "BDD_ANN: number of nodes: $nodes_ann" > /dev/null
    resultNodesANN=$?
    
    if [ $resultNodesMP -ne 0 ] ; then
        echo     ... FAILED
        echo Number of nodes of BDD_MP is incorrect.
        echo
        result=1
    fi

    if [ $resultNodesANN -ne 0 ] ; then
        echo     ... FAILED
        echo Number of nodes of BDD_ANN is incorrect.
        echo
        result=1
    fi

fi

############################################################################

owfn="$testdir/philosophers/phcontrol4.unf.owfn"
owfn_reference="$testdir/bdd_ref/phcontrol4.unf_reference.owfn"
cmd="$FIONA -n $owfn -a -b 4 -t OG"
$cmd 2>/dev/null 1>/dev/null

cmd="$FIONA -n $owfn -n $owfn_reference -x"


if [ "$memcheck" = "yes" ]; then
    memchecklog="$owfn.n.n.x.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$?
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
    echo $OUTPUT | grep "The two operating guidelines are equal: YES" > /dev/null
    resultEqual=$?
    
    if [ $resultEqual -ne 0 ] ; then
        echo     ... FAILED
        echo The two operating guidelines are not equal although they should be.
        echo
        result=1
    fi
fi

############################################################################

echo

exit $result
