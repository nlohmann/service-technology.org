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

DIR=$testdir/extrema
FIONA=fiona

$FIONA --net=$DIR/no_final_marking.owfn -a --graphtype=OG | grep "net is controllable: YES"
result1=$?
echo result $?

$FIONA --net=$DIR/all_final_marking.owfn -a --graphtype=OG | grep "net is controllable: NO"
result2=$?
echo result $?

$FIONA --net=$DIR/no_communication.owfn -a --graphtype=OG | grep "net is controllable: YES"
result3=$?
echo result $?

$FIONA --net=$DIR/no_initial_marking.owfn -a --graphtype=OG | grep "net is controllable: NO"
result4=$?
echo result $?

$FIONA --net=$DIR/no_initial_marking2.owfn -a --graphtype=OG | grep "net is controllable: YES"
result5=$?
echo result $?

$FIONA --net=$DIR/empty.owfn -a --graphtype=OG | grep "net is controllable: YES"
result6=$?
echo result $?

$FIONA --net=$DIR/one_input.owfn -a --graphtype=OG | grep "net is controllable: YES"
result7=$?
echo result $?

$FIONA --net=$DIR/one_output.owfn -a --graphtype=OG | grep "net is controllable: YES"
result8=$?
echo result $?

$FIONA --net=$DIR/one_input_marked.owfn -a --graphtype=OG | grep "net is controllable: NO"
result9=$?
echo result $?

$FIONA --net=$DIR/one_output_marked.owfn -a --graphtype=OG | grep "net is controllable: NO"
result10=$?
echo result $?

$FIONA --net=$DIR/one_input_one_output.owfn -a --graphtype=OG | grep "net is controllable: YES"
result11=$?
echo result $?

$FIONA --net=$DIR/one_input_2.owfn -a --graphtype=OG | grep "net is controllable: NO"
result12=$?
echo result $?

$FIONA --net=$DIR/multiple_input.owfn -a --graphtype=OG | grep "net is controllable: YES"
result13=$?
echo result $?

$FIONA --net=$DIR/multiple_output.owfn -a --graphtype=OG | grep "net is controllable: YES"
result14=$?
echo result $?

$FIONA --net=$DIR/multiple_input_multiple_output.owfn -a --graphtype=OG | grep "net is controllable: YES"
result15=$?
echo result $?


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

exit $result
