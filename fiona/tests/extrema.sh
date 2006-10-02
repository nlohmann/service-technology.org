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

#testdir=.
DIR=$testdir/extrema
FIONA=fiona

#loeschen aller erzeugten Dateien im letzten Durchlauf
rm -f $DIR/*.out
rm -f $DIR/*.png

############################################################################

echo running $FIONA -n $DIR/no_final_marking.owfn -a -t OG
$FIONA -n $DIR/no_final_marking.owfn -a -t OG 2>&1 | grep "net is controllable: YES" > /dev/null
result1=$?

echo running $FIONA -n $DIR/all_final_marking.owfn -a -t OG
$FIONA -n $DIR/all_final_marking.owfn -a -t OG 2>&1 | grep "net is controllable: NO" > /dev/null
result2=$?

echo running $FIONA -n $DIR/no_communication.owfn -a -t OG
$FIONA -n $DIR/no_communication.owfn -a -t OG 2>&1 | grep "net is controllable: YES" > /dev/null
result3=$?

echo running $FIONA -n $DIR/no_initial_marking.owfn -a -t OG
$FIONA -n $DIR/no_initial_marking.owfn -a -t OG 2>&1 | grep "net is controllable: NO" > /dev/null
result4=$?

echo running $FIONA -n $DIR/no_initial_marking2.owfn -a -t OG
$FIONA -n $DIR/no_initial_marking2.owfn -a -t OG 2>&1 | grep "net is controllable: YES" > /dev/null
result5=$?

echo running $FIONA -n $DIR/empty.owfn -a -t OG
$FIONA -n $DIR/empty.owfn -a -t OG 2>&1 | grep "net is controllable: YES" > /dev/null
result6=$?

echo running $FIONA -n $DIR/one_input.owfn -a -t OG
$FIONA -n $DIR/one_input.owfn -a -t OG 2>&1 | grep "net is controllable: YES" > /dev/null
result7=$?

echo running $FIONA -n $DIR/one_output.owfn -a -t OG
$FIONA -n $DIR/one_output.owfn -a -t OG 2>&1 | grep "net is controllable: YES" > /dev/null
result8=$?

echo running $FIONA -n $DIR/one_input_marked.owfn -a -t OG
$FIONA -n $DIR/one_input_marked.owfn -a -t OG 2>&1 | grep "net is controllable: NO" > /dev/null
result9=$?

echo running $FIONA -n $DIR/one_output_marked.owfn -a -t OG
$FIONA -n $DIR/one_output_marked.owfn -a -t OG 2>&1 | grep "net is controllable: NO" > /dev/null
result10=$?

echo running $FIONA -n $DIR/one_input_one_output.owfn -a -t OG
$FIONA -n $DIR/one_input_one_output.owfn -a -t OG 2>&1 | grep "net is controllable: YES" > /dev/null
result11=$?

echo running $FIONA -n $DIR/one_input_2.owfn -a -t OG
$FIONA -n $DIR/one_input_2.owfn -a -t OG 2>&1 | grep "net is controllable: NO" > /dev/null
result12=$?

echo running $FIONA -n $DIR/multiple_input.owfn -a -t OG
$FIONA -n $DIR/multiple_input.owfn -a -t OG 2>&1 | grep "net is controllable: YES" > /dev/null
result13=$?

echo running $FIONA -n $DIR/multiple_output.owfn -a -t OG
$FIONA -n $DIR/multiple_output.owfn -a -t OG 2>&1 | grep "net is controllable: YES" > /dev/null
result14=$?

echo running $FIONA -n $DIR/multiple_input_multiple_output.owfn -a -t OG
$FIONA -n $DIR/multiple_input_multiple_output.owfn -a -t OG 2>&1 | grep "net is controllable: YES" > /dev/null
result15=$?

#OUTPUT=`$FIONA -n $DIR/multiple_input_multiple_output.owfn -a -t OG 2>&1`
#echo $OUTPUT | grep "net is controllable: YES" > /dev/null
#result15=$?

############################################################################

if test \( $result1 -eq 0 -a $result2 -eq 0 -a $result3 -eq 0 -a \
           $result4 -eq 0 -a $result5 -eq 0 -a $result6 -eq 0 -a \
           $result7 -eq 0 -a $result8 -eq 0 -a $result9 -eq 0 -a \
           $result10 -eq 0 -a $result11 -eq 0 -a $result12 -eq 0 -a \
           $result13 -eq 0 -a $result14 -eq 0 -a $result15 -eq 0 \)
then
  result=0
else
  result=1
fi

#loeschen aller erzeugten Dateien im letzten Durchlauf
rm -f $DIR/*.out
rm -f $DIR/*.png

echo

exit $result
