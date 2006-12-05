#! /bin/sh

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

testdir=.
DIR=$testdir/messages
FIONA=fiona

#loeschen aller erzeugten Dateien im letzten Durchlauf
rm -f $DIR/*.out
rm -f $DIR/*.png
rm -f $DIR/*.og

result=0

############################################################################

echo running $FIONA -n $DIR/messageboundtest1.owfn -a -t OG -e10 -m4
OUTPUT=`$FIONA -n $DIR/messageboundtest1.owfn -a -t OG -e10 -m4  2>&1`

echo $OUTPUT | grep "net is controllable: NO" > /dev/null
mb14control=$?

if [ $mb14control -ne 0 ]
then
echo   ... failed to build OG correctly
fi

result=`expr $mb14control`

############################################################################

mb15bluenodes_soll=36
mb15blueedges_soll=60

echo running $FIONA -n $DIR/messageboundtest1.owfn -a -t OG -e10 -m5
OUTPUT=`$FIONA -n $DIR/messageboundtest1.owfn -a -t OG -e10 -m5  2>&1`

echo $OUTPUT | grep "net is controllable: YES" > /dev/null
mb15control=$?

echo $OUTPUT | grep "number of blue nodes: $mb15bluenodes_soll" > /dev/null
mb15bluenodes=$?

echo $OUTPUT | grep "number of blue edges: $mb15blueedges_soll" > /dev/null
mb15blueedges=$?

if [ $mb15control -ne 0 -o $mb15bluenodes -ne 0 -o $mb15blueedges -ne 0 ]
then
echo   ... failed to build OG correctly
fi

result=`expr $result + $mb15control + $mb15bluenodes + $mb15blueedges`

############################################################################

mb16bluenodes_soll=46
mb16blueedges_soll=78

echo running $FIONA -n $DIR/messageboundtest1.owfn -a -t OG -e10 -m6
OUTPUT=`$FIONA -n $DIR/messageboundtest1.owfn -a -t OG -e10 -m6  2>&1`

echo $OUTPUT | grep "net is controllable: YES" > /dev/null
mb16control=$?

echo $OUTPUT | grep "number of blue nodes: $mb16bluenodes_soll" > /dev/null
mb16bluenodes=$?

echo $OUTPUT | grep "number of blue edges: $mb16blueedges_soll" > /dev/null
mb16blueedges=$?

if [ $mb16control -ne 0 -o $mb16bluenodes -ne 0 -o $mb16blueedges -ne 0 ]
then
echo   ... failed to build OG correctly
fi

result=`expr $result + $mb16control + $mb16bluenodes + $mb16blueedges`

############################################################################

mb21bluenodes_soll=3
mb21blueedges_soll=2

echo running $FIONA -n $DIR/messageboundtest2.owfn -a -t OG -e10 -m1
OUTPUT=`$FIONA -n $DIR/messageboundtest2.owfn -a -t OG -e10 -m1  2>&1`

echo $OUTPUT | grep "net is controllable: YES" > /dev/null
mb21control=$?

echo $OUTPUT | grep "number of blue nodes: $mb21bluenodes_soll" > /dev/null
mb21bluenodes=$?

echo $OUTPUT | grep "number of blue edges: $mb21blueedges_soll" > /dev/null
mb21blueedges=$?

if [ $mb21control -ne 0 -o $mb21bluenodes -ne 0 -o $mb21blueedges -ne 0 ]
then
echo   ... failed to build OG correctly
fi

result=`expr $result + $mb21control + $mb21bluenodes + $mb21blueedges`

############################################################################

mb22bluenodes_soll=5
mb22blueedges_soll=5

echo running $FIONA -n $DIR/messageboundtest2.owfn -a -t OG -e10 -m2
OUTPUT=`$FIONA -n $DIR/messageboundtest2.owfn -a -t OG -e10 -m2  2>&1`

echo $OUTPUT | grep "net is controllable: YES" > /dev/null
mb22control=$?

echo $OUTPUT | grep "number of blue nodes: $mb22bluenodes_soll" > /dev/null
mb22bluenodes=$?

echo $OUTPUT | grep "number of blue edges: $mb22blueedges_soll" > /dev/null
mb22blueedges=$?

if [ $mb22control -ne 0 -o $mb22bluenodes -ne 0 -o $mb22blueedges -ne 0 ]
then
echo   ... failed to build OG correctly
fi

result=`expr $result + $mb22control + $mb22bluenodes + $mb22blueedges`

############################################################################

echo

exit $result
