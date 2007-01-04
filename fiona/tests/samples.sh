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
DIR=$testdir/samples
FIONA=fiona

#loeschen aller erzeugten Dateien im letzten Durchlauf
rm -f $DIR/*.out
rm -f $DIR/*.png
rm -f $DIR/*.og

result=0

############################################################################

shop3bluenodes_soll=12
shop3blueedges_soll=15
shop3storedstates_soll=1878

echo running $FIONA -n $DIR/06-03-23_BPM06_shop_sect_3.owfn -a -t OG
OUTPUT=`$FIONA -n $DIR/06-03-23_BPM06_shop_sect_3.owfn -a -t OG  2>&1`

echo $OUTPUT | grep "net is controllable: YES" > /dev/null
shop3control=$?

echo $OUTPUT | grep "number of blue nodes: $shop3bluenodes_soll" > /dev/null
shop3bluenodes=$?

echo $OUTPUT | grep "number of blue edges: $shop3blueedges_soll" > /dev/null
shop3blueedges=$?

echo $OUTPUT | grep "number of states stored in nodes: $shop3storedstates_soll" > /dev/null
shop3storedstates=$?

if [ $shop3control -ne 0 -o $shop3bluenodes -ne 0 -o $shop3blueedges -ne 0 -o $shop3storedstates -ne 0 ]
then
echo   ... failed to build OG correctly
fi

result=`expr $result + $shop3control + $shop3bluenodes + $shop3blueedges + $shop3storedstates`

############################################################################
# IG
############################################################################

shop3bluenodes_soll=11
shop3blueedges_soll=13
shop3storedstates_soll=728

echo running $FIONA -n $DIR/06-03-23_BPM06_shop_sect_3.owfn -a -t IG
OUTPUT=`$FIONA -n $DIR/06-03-23_BPM06_shop_sect_3.owfn -a -t IG 2>&1`

echo $OUTPUT | grep "net is controllable: YES" > /dev/null
shop3control=$?

echo $OUTPUT | grep "number of blue nodes: $shop3bluenodes_soll" > /dev/null
shop3bluenodes=$?

echo $OUTPUT | grep "number of blue edges: $shop3blueedges_soll" > /dev/null
shop3blueedges=$?

echo $OUTPUT | grep "number of states stored in nodes: $shop3storedstates_soll" > /dev/null
shop3storedstates=$?

if [ $shop3control -ne 0 -o $shop3bluenodes -ne 0 -o $shop3blueedges -ne 0 -o $shop3storedstates -ne 0 ]
then
echo   ... failed to build the IG correctly
fi

result=`expr $result + $shop3control + $shop3bluenodes + $shop3blueedges + $shop3storedstates`

############################################################################
# IG reduced
############################################################################

shop3bluenodes_soll=8 #7
shop3blueedges_soll=8 #6
shop3storedstates_soll=351 #329

echo running $FIONA -n $DIR/06-03-23_BPM06_shop_sect_3.owfn -a -t IG -r
OUTPUT=`$FIONA -n $DIR/06-03-23_BPM06_shop_sect_3.owfn -a -t IG -r 2>&1`

echo $OUTPUT | grep "net is controllable: YES" > /dev/null
shop3control=$?

echo $OUTPUT | grep "number of blue nodes: $shop3bluenodes_soll" > /dev/null
shop3bluenodes=$?

echo $OUTPUT | grep "number of blue edges: $shop3blueedges_soll" > /dev/null
shop3blueedges=$?

echo $OUTPUT | grep "number of states stored in nodes: $shop3storedstates_soll" > /dev/null
shop3storedstates=$?

if [ $shop3control -ne 0 -o $shop3bluenodes -ne 0 -o $shop3blueedges -ne 0 -o $shop3storedstates -ne 0 ]
then
echo   ... failed to build the reduced IG correctly
fi

result=`expr $result + $shop3control + $shop3bluenodes + $shop3blueedges + $shop3storedstates`

############################################################################

shop6bluenodes_soll=7
shop6blueedges_soll=7
shop6storedstates_soll=4712

echo running $FIONA -n $DIR/06-03-23_BPM06_shop_sect_6.owfn -a -t OG
OUTPUT=`$FIONA -n $DIR/06-03-23_BPM06_shop_sect_6.owfn -a -t OG  2>&1`

echo $OUTPUT | grep "net is controllable: YES" > /dev/null
shop6control=$?

echo $OUTPUT | grep "number of blue nodes: $shop6bluenodes_soll" > /dev/null
shop6bluenodes=$?

echo $OUTPUT | grep "number of blue edges: $shop6blueedges_soll" > /dev/null
shop6blueedges=$?

echo $OUTPUT | grep "number of states stored in nodes: $shop6storedstates_soll" > /dev/null
shop6storedstates=$?

if [ $shop6control -ne 0 -o $shop6bluenodes -ne 0 -o $shop6blueedges -ne 0 -o $shop6storedstates -ne 0 ]
then
echo   ... failed to build OG correctly
fi

result=`expr $result + $shop6control + $shop6bluenodes + $shop6blueedges + $shop6storedstates`

############################################################################
# IG
############################################################################

shop6bluenodes_soll=6
shop6blueedges_soll=5
shop6storedstates_soll=2491 #3863

echo running $FIONA -n $DIR/06-03-23_BPM06_shop_sect_6.owfn -a -t IG
OUTPUT=`$FIONA -n $DIR/06-03-23_BPM06_shop_sect_6.owfn -a -t IG  2>&1`

echo $OUTPUT | grep "net is controllable: YES" > /dev/null
shop6control=$?

echo $OUTPUT | grep "number of blue nodes: $shop6bluenodes_soll" > /dev/null
shop6bluenodes=$?

echo $OUTPUT | grep "number of blue edges: $shop6blueedges_soll" > /dev/null
shop6blueedges=$?

echo $OUTPUT | grep "number of states stored in nodes: $shop6storedstates_soll" > /dev/null
shop6storedstates=$?

if [ $shop6control -ne 0 -o $shop6bluenodes -ne 0 -o $shop6blueedges -ne 0 -o $shop6storedstates -ne 0 ]
then
echo   ... failed to build IG correctly
fi

result=`expr $result + $shop6control + $shop6bluenodes + $shop6blueedges + $shop6storedstates`

############################################################################
# reduced IG
############################################################################

shop6bluenodes_soll=6
shop6blueedges_soll=5
shop6storedstates_soll=1109   #1087

echo running $FIONA -n $DIR/06-03-23_BPM06_shop_sect_6.owfn -a -t IG -r
OUTPUT=`$FIONA -n $DIR/06-03-23_BPM06_shop_sect_6.owfn -a -t IG -r 2>&1`

echo $OUTPUT | grep "net is controllable: YES" > /dev/null
shop6control=$?

echo $OUTPUT | grep "number of blue nodes: $shop6bluenodes_soll" > /dev/null
shop6bluenodes=$?

echo $OUTPUT | grep "number of blue edges: $shop6blueedges_soll" > /dev/null
shop6blueedges=$?

echo $OUTPUT | grep "number of states stored in nodes: $shop6storedstates_soll" > /dev/null
shop6storedstates=$?

if [ $shop6control -ne 0 -o $shop6bluenodes -ne 0 -o $shop6blueedges -ne 0 -o $shop6storedstates -ne 0 ]
then
echo   ... failed to build reduced IG correctly
fi

result=`expr $result + $shop6control + $shop6bluenodes + $shop6blueedges + $shop6storedstates`

############################################################################

mycoffeebluenodes_soll=7
mycoffeeblueedges_soll=9
mycoffeestoredstates_soll=24

echo running $FIONA -n $DIR/myCoffee.owfn -a -t OG
OUTPUT=`$FIONA -n $DIR/myCoffee.owfn -a -t OG  2>&1`

echo $OUTPUT | grep "net is controllable: YES" > /dev/null
mycoffeecontrol=$?

echo $OUTPUT | grep "number of blue nodes: $mycoffeebluenodes_soll" > /dev/null
mycoffeebluenodes=$?

echo $OUTPUT | grep "number of blue edges: $mycoffeeblueedges_soll" > /dev/null
mycoffeeblueedges=$?

echo $OUTPUT | grep "number of states stored in nodes: $mycoffeestoredstates_soll" > /dev/null
mycoffeestoredstates=$?

if [ $mycoffeecontrol -ne 0 -o $mycoffeebluenodes -ne 0 -o $mycoffeeblueedges -ne 0 -o $mycoffeestoredstates -ne 0 ]
then
echo   ... failed to build OG correctly
fi

result=`expr $result + $mycoffeecontrol + $mycoffeebluenodes + $mycoffeeblueedges + $mycoffeestoredstates`


############################################################################
# IG
############################################################################

mycoffeebluenodes_soll=5
mycoffeeblueedges_soll=5
mycoffeestoredstates_soll=12

echo running $FIONA -n $DIR/myCoffee.owfn -a -t IG
OUTPUT=`$FIONA -n $DIR/myCoffee.owfn -a -t IG  2>&1`

echo $OUTPUT | grep "net is controllable: YES" > /dev/null
mycoffeecontrol=$?

echo $OUTPUT | grep "number of blue nodes: $mycoffeebluenodes_soll" > /dev/null
mycoffeebluenodes=$?

echo $OUTPUT | grep "number of blue edges: $mycoffeeblueedges_soll" > /dev/null
mycoffeeblueedges=$?

echo $OUTPUT | grep "number of states stored in nodes: $mycoffeestoredstates_soll" > /dev/null
mycoffeestoredstates=$?

if [ $mycoffeecontrol -ne 0 -o $mycoffeebluenodes -ne 0 -o $mycoffeeblueedges -ne 0 -o $mycoffeestoredstates -ne 0 ]
then
echo   ... failed to build IG correctly
fi

result=`expr $result + $mycoffeecontrol + $mycoffeebluenodes + $mycoffeeblueedges + $mycoffeestoredstates`

############################################################################
# reduced IG
############################################################################

mycoffeebluenodes_soll=5
mycoffeeblueedges_soll=5
mycoffeestoredstates_soll=12

echo running $FIONA -n $DIR/myCoffee.owfn -a -r -t IG
OUTPUT=`$FIONA -n $DIR/myCoffee.owfn -a -r -t IG  2>&1`

echo $OUTPUT | grep "net is controllable: YES" > /dev/null
mycoffeecontrol=$?

echo $OUTPUT | grep "number of blue nodes: $mycoffeebluenodes_soll" > /dev/null
mycoffeebluenodes=$?

echo $OUTPUT | grep "number of blue edges: $mycoffeeblueedges_soll" > /dev/null
mycoffeeblueedges=$?

echo $OUTPUT | grep "number of states stored in nodes: $mycoffeestoredstates_soll" > /dev/null
mycoffeestoredstates=$?

if [ $mycoffeecontrol -ne 0 -o $mycoffeebluenodes -ne 0 -o $mycoffeeblueedges -ne 0 -o $mycoffeestoredstates -ne 0 ]
then
echo   ... failed to build reduced IG correctly
fi

result=`expr $result + $mycoffeecontrol + $mycoffeebluenodes + $mycoffeeblueedges + $mycoffeestoredstates`

############################################################################
# reduced IG
############################################################################

mycoffeebluenodes_soll=5
mycoffeeblueedges_soll=5
mycoffeestoredstates_soll=5
mycoffeestates_soll=19

echo running $FIONA -n $DIR/myCoffee.owfn -r -t IG
OUTPUT=`$FIONA -n $DIR/myCoffee.owfn -r -t IG  2>&1`

echo $OUTPUT | grep "net is controllable: YES" > /dev/null
mycoffeecontrol=$?

echo $OUTPUT | grep "number of states calculated: $mycoffeestates_soll" > /dev/null
mycoffeestates=$?

echo $OUTPUT | grep "number of blue nodes: $mycoffeebluenodes_soll" > /dev/null
mycoffeebluenodes=$?

echo $OUTPUT | grep "number of blue edges: $mycoffeeblueedges_soll" > /dev/null
mycoffeeblueedges=$?

echo $OUTPUT | grep "number of states stored in nodes: $mycoffeestoredstates_soll" > /dev/null
mycoffeestoredstates=$?

if [ $mycoffeecontrol -ne 0 -o $mycoffeestates -ne 0 -o $mycoffeebluenodes -ne 0 -o $mycoffeeblueedges -ne 0 -o $mycoffeestoredstates -ne 0 ]
then
echo   ... failed to build reduced IG correctly
fi

result=`expr $result + $mycoffeecontrol + $mycoffeebluenodes + $mycoffeeblueedges + $mycoffeestoredstates`

############################################################################
# IG with node reduction
############################################################################

mycoffeebluenodes_soll=5
mycoffeeblueedges_soll=5
mycoffeestoredstates_soll=5
mycoffeestates_soll=19

echo running $FIONA -n $DIR/myCoffee.owfn -t IG
OUTPUT=`$FIONA -n $DIR/myCoffee.owfn -t IG  2>&1`

echo $OUTPUT | grep "net is controllable: YES" > /dev/null
mycoffeecontrol=$?

echo $OUTPUT | grep "number of states calculated: $mycoffeestates_soll" > /dev/null
mycoffeestates=$?

echo $OUTPUT | grep "number of blue nodes: $mycoffeebluenodes_soll" > /dev/null
mycoffeebluenodes=$?

echo $OUTPUT | grep "number of blue edges: $mycoffeeblueedges_soll" > /dev/null
mycoffeeblueedges=$?

echo $OUTPUT | grep "number of states stored in nodes: $mycoffeestoredstates_soll" > /dev/null
mycoffeestoredstates=$?

if [ $mycoffeecontrol -ne 0 -o $mycoffeestates -ne 0 -o $mycoffeebluenodes -ne 0 -o $mycoffeeblueedges -ne 0 -o $mycoffeestoredstates -ne 0 ]
then
echo   ... failed to build IG with node reduction correctly
fi

result=`expr $result + $mycoffeecontrol + $mycoffeebluenodes + $mycoffeeblueedges + $mycoffeestoredstates`

############################################################################

keescoffee1bluenodes_soll=9
keescoffee1blueedges_soll=13
keescoffee1storedstates_soll=100

echo running $FIONA -n $DIR/keesCoffee_condition.owfn -a -t OG -e 1
OUTPUT=`$FIONA -n $DIR/keesCoffee_condition.owfn -a -t OG -e 1  2>&1`

echo $OUTPUT | grep "net is controllable: YES" > /dev/null
keescoffee1control=$?

echo $OUTPUT | grep "number of blue nodes: $keescoffee1bluenodes_soll" > /dev/null
keescoffee1bluenodes=$?

echo $OUTPUT | grep "number of blue edges: $keescoffee1blueedges_soll" > /dev/null
keescoffee1blueedges=$?

echo $OUTPUT | grep "number of states stored in nodes: $keescoffee1storedstates_soll" > /dev/null
keescoffee1storedstates=$?

if [ $keescoffee1control -ne 0 -o $keescoffee1bluenodes -ne 0 -o $keescoffee1blueedges -ne 0 -o $keescoffee1storedstates -ne 0 ]
then
echo   ... failed to build OG correctly
fi

result=`expr $result + $keescoffee1control + $keescoffee1bluenodes + $keescoffee1blueedges + $keescoffee1storedstates`

############################################################################

keescoffee2bluenodes_soll=27
keescoffee2blueedges_soll=62
keescoffee2storedstates_soll=1008

echo running $FIONA -n $DIR/keesCoffee_condition.owfn -a -t OG -e 2
OUTPUT=`$FIONA -n $DIR/keesCoffee_condition.owfn -a -t OG -e 2  2>&1`

echo $OUTPUT | grep "net is controllable: YES" > /dev/null
keescoffee2control=$?

echo $OUTPUT | grep "number of blue nodes: $keescoffee2bluenodes_soll" > /dev/null
keescoffee2bluenodes=$?

echo $OUTPUT | grep "number of blue edges: $keescoffee2blueedges_soll" > /dev/null
keescoffee2blueedges=$?

echo $OUTPUT | grep "number of states stored in nodes: $keescoffee2storedstates_soll" > /dev/null
keescoffee2storedstates=$?

if [ $keescoffee2control -ne 0 -o $keescoffee2bluenodes -ne 0 -o $keescoffee2blueedges -ne 0 -o $keescoffee2storedstates -ne 0 ]
then
echo   ... failed to build OG correctly
fi

result=`expr $result + $keescoffee2control + $keescoffee2bluenodes + $keescoffee2blueedges + $keescoffee2storedstates`

############################################################################

echo

exit $result
