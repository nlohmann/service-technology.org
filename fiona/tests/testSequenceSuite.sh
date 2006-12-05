#! /bin/bash

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

echo
echo ---------------------------------------------------------------------
echo running $0
echo

#testdir=.
DIR=$testdir/sequence_suite
FIONA=fiona

#loeschen aller erzeugten Dateien im letzten Durchlauf
rm -f $DIR/*.out
rm -f $DIR/*.png
rm -f $DIR/*.og

############################################################################

result=0

#only run sequences 3 and 9
for i in 3 9;
do
    #echo running sequence$i

    echo running $FIONA -n $DIR/sequence${i}.owfn -a -t IG

    OUTPUT=`$FIONA -n $DIR/sequence${i}.owfn -a --graphtype=IG  2>&1`
    echo $OUTPUT | grep "net is controllable: YES" > /dev/null
    resultIG=$?
    echo $OUTPUT | grep "number of states calculated: $(((i+1)*(i+2)/2))" > /dev/null
    resultIGNOSC=$?
    echo $OUTPUT | grep "number of nodes: $((i+1))" > /dev/null
    resultIGNON=$?
    echo $OUTPUT | grep "number of edges: $i" > /dev/null
    resultIGNOE=$?
    echo $OUTPUT | grep "number of blue nodes: $((i+1))" > /dev/null
    resultIGNOBN=$?
    echo $OUTPUT | grep "number of blue edges: $i" > /dev/null
    resultIGNOBE=$?

    if [ $resultIG -ne 0 -o $resultIGNOSC -ne 0 -o $resultIGNON -ne 0 -o $resultIGNOE -ne 0 -o $resultIGNOBN -ne 0 -o $resultIGNOBE -ne 0 ]
    then
        result=1
        echo   ... failed to build IG correctly
    fi

    echo running $FIONA -n $DIR/sequence${i}.owfn -a -t OG

    OUTPUT=`$FIONA -n $DIR/sequence${i}.owfn -a --graphtype=OG  2>&1`
    echo $OUTPUT | grep "net is controllable: YES" > /dev/null
    resultOG=$?
    echo $OUTPUT | grep "number of states calculated: $(((2**(i+1))-1))" > /dev/null
    resultOGNOSC=$?
    echo $OUTPUT | grep "number of nodes: $((2**i))" > /dev/null
    resultOGNON=$?
    echo $OUTPUT | grep "number of edges: $((i*(2**(i-1))))" > /dev/null
    resultOGNOE=$?
    echo $OUTPUT | grep "number of blue nodes: $((2**i))" > /dev/null
    resultOGNOBN=$?
    echo $OUTPUT | grep "number of blue edges: $((i*(2**(i-1))))" > /dev/null
    resultOGNOBE=$?
    if [ $resultOG -ne 0 -o $resultOGNOSC -ne 0 -o $resultOGNON -ne 0 -o $resultOGNOE -ne 0 -o $resultOGNOBN -ne 0 -o $resultOGNOBE -ne 0 ]
    then
        result=1
        echo   ... failed to build OG
    fi

    let i=i+1
done

echo

exit $result
