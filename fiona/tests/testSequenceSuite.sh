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

############################################################################

#$FIONA -n $DIR/sequence3.owfn -a --graphtype=IG  2> /dev/null 1> /dev/null
#result3IG=$?

#$FIONA -n $DIR/sequence3.owfn -a --graphtype=OG -e1 2> /dev/null 1> /dev/null
#result3OG=$?

#$FIONA -n $DIR/sequence4.owfn -a --graphtype=IG  2> /dev/null 1> /dev/null
#result4IG=$?

#$FIONA -n $DIR/sequence4.owfn -a --graphtype=OG -e1 2> /dev/null 1> /dev/null
#result4OG=$?

#$FIONA -n $DIR/sequence5.owfn -a --graphtype=IG  2> /dev/null 1> /dev/null
#result5IG=$?

#$FIONA -n $DIR/sequence5.owfn -a --graphtype=OG -e1 2> /dev/null 1> /dev/null
#result5OG=$?

#$FIONA -n $DIR/sequence6.owfn -a --graphtype=IG  2> /dev/null 1> /dev/null
#result6IG=$?

#$FIONA -n $DIR/sequence6.owfn -a --graphtype=OG -e1 2> /dev/null 1> /dev/null
#result6OG=$?

#$FIONA -n $DIR/sequence7.owfn -a --graphtype=IG  2> /dev/null 1> /dev/null
#result7IG=$?

#$FIONA -n $DIR/sequence7.owfn -a --graphtype=OG -e1 2> /dev/null 1> /dev/null
#result7OG=$?

#$FIONA -n $DIR/sequence8.owfn -a --graphtype=IG  2> /dev/null 1> /dev/null
#result8IG=$?

#$FIONA -n $DIR/sequence8.owfn -a --graphtype=OG -e1 2> /dev/null 1> /dev/null
#result8OG=$?

#$FIONA -n $DIR/sequence9.owfn -a --graphtype=IG  2> /dev/null 1> /dev/null
#result9IG=$?

#$FIONA -n $DIR/sequence9.owfn -a --graphtype=OG -e1 2> /dev/null 1> /dev/null
#result9OG=$?

#$FIONA -n $DIR/sequence10.owfn -a --graphtype=IG  2> /dev/null 1> /dev/null
#result10IG=$?

#$FIONA -n $DIR/sequence10.owfn -a --graphtype=OG -e1 2> /dev/null 1> /dev/null
#result10OG=$?

i=3

while [ $i -le 9 ]
do
    #only run sequences 3 and 9
    if [ $i -ne 3 -a $i -ne 9 ]; then
        let i=i+1
        continue
    fi

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
    echo   ... failed to build OG
    fi

    let i=i+1
done

#if test \( $result3IG  -eq 0 -a $result3OG  -eq 0 -a \
#           $result4IG  -eq 0 -a $result4OG  -eq 0 -a \
#           $result5IG  -eq 0 -a $result5OG  -eq 0 -a \
#           $result6IG  -eq 0 -a $result6OG  -eq 0 -a \
#           $result7IG  -eq 0 -a $result7OG  -eq 0 -a \
#           $result8IG  -eq 0 -a $result8OG  -eq 0 -a \
#           $result9IG  -eq 0 -a $result9OG  -eq 0 -a \
#           $result10IG -eq 0 -a $result10OG -eq 0 \) -a \
#   \( -f $DIR/sequence3.owfn.a.IG.png -a -f $DIR/sequence3.owfn.a.IG.out \) -a \
#   \( -f $DIR/sequence4.owfn.a.IG.png -a -f $DIR/sequence4.owfn.a.IG.out \) -a \
#   \( -f $DIR/sequence5.owfn.a.IG.png -a -f $DIR/sequence5.owfn.a.IG.out \) -a \
#   \( -f $DIR/sequence6.owfn.a.IG.png -a -f $DIR/sequence6.owfn.a.IG.out \) -a \
#   \( -f $DIR/sequence7.owfn.a.IG.png -a -f $DIR/sequence7.owfn.a.IG.out \) -a \
#   \( -f $DIR/sequence8.owfn.a.IG.png -a -f $DIR/sequence8.owfn.a.IG.out \) -a \
#   \( -f $DIR/sequence9.owfn.a.IG.png -a -f $DIR/sequence9.owfn.a.IG.out \) -a \
#   \( -f $DIR/sequence10.owfn.a.IG.png -a -f $DIR/sequence10.owfn.a.IG.out \) -a \
#   \( -f $DIR/sequence3.owfn.a.OG.png -a -f $DIR/sequence3.owfn.a.OG.out \) -a \
#   \( -f $DIR/sequence4.owfn.a.OG.png -a -f $DIR/sequence4.owfn.a.OG.out \) -a \
#   \( -f $DIR/sequence5.owfn.a.OG.png -a -f $DIR/sequence5.owfn.a.OG.out \) -a \
#   \( -f $DIR/sequence6.owfn.a.OG.png -a -f $DIR/sequence6.owfn.a.OG.out \) -a \
#   \( -f $DIR/sequence7.owfn.a.OG.png -a -f $DIR/sequence7.owfn.a.OG.out \) -a \
#   \( -f $DIR/sequence8.owfn.a.OG.png -a -f $DIR/sequence8.owfn.a.OG.out \) -a \
#   \( -f $DIR/sequence9.owfn.a.OG.png -a -f $DIR/sequence9.owfn.a.OG.out \) -a \
#   \( -f $DIR/sequence10.owfn.a.OG.png -a -f $DIR/sequence10.owfn.a.OG.out \)
#then
#  result=0
#else
#  result=1
#fi

#loeschen aller erzeugten Dateien im letzten Durchlauf
rm -f $DIR/*.out
rm -f $DIR/*.png

echo

exit $result
