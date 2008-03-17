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

source defaults.sh
source memcheck_helper.sh

echo
echo ---------------------------------------------------------------------
echo running $0
echo

DIR=$testdir/statesReduction
FIONA=fiona

#loeschen aller erzeugten Dateien im letzten Durchlauf
rm -f $DIR/*.out
rm -f $DIR/*.OG.png
rm -f $DIR/*.IG.png
rm -f $DIR/*.og
rm -f $DIR/*.log

result=0

#exit $result


############################################################################
# check if graph using -R is the same as the one generated without -R
############################################################################
owfn="$DIR/06-03-23_BPM06_shop_sect_3.owfn"
cmd1="$FIONA $owfn -t eqR -t IG"

echo running $cmd1
OUTPUT=`$cmd1 2>&1`

echo $OUTPUT | grep "are equivalent: YES" > /dev/null
    equivalent=$?

if [ $equivalent -ne 0 ]
then
    echo   ... the two graphs do not characterize the same strategies
fi


result=`expr $result + $equivalent`

############################################################################
# check if graph using -R is the same as the one generated without -R
# ==> OG
############################################################################
owfn="$DIR/06-03-23_BPM06_shop_sect_3.owfn"
cmd1="$FIONA $owfn -t eqR -t OG"


echo running $cmd1
OUTPUT=`$cmd1 2>&1`

echo $OUTPUT | grep "are equivalent: YES" > /dev/null
    equivalent=$?

if [ $equivalent -ne 0 ]
then
    echo   ... the two graphs do not characterize the same strategies
fi


result=`expr $result + $equivalent`

############################################################################
# check if graph using -R is the same as the one generated without -R
############################################################################
owfn="$DIR/06-03-23_BPM06_shop_sect_6.owfn"
cmd1="$FIONA $owfn -t eqR -t IG"

echo running $cmd1
OUTPUT=`$cmd1 2>&1`

echo $OUTPUT | grep "are equivalent: YES" > /dev/null
    equivalent=$?

if [ $equivalent -ne 0 ]
then
    echo   ... the two graphs do not characterize the same strategies
fi


result=`expr $result + $equivalent`

############################################################################
# check if graph using -R is the same as the one generated without -R
############################################################################
owfn="$DIR/myCoffee.owfn"
cmd1="$FIONA $owfn -t eqR -t IG"

echo running $cmd1
OUTPUT=`$cmd1 2>&1`

echo $OUTPUT | grep "are equivalent: YES" > /dev/null
    equivalent=$?

if [ $equivalent -ne 0 ]
then
    echo   ... the two graphs do not characterize the same strategies
fi


result=`expr $result + $equivalent`

############################################################################
# check if graph using -R is the same as the one generated without -R
############################################################################
owfn="$DIR/dke07_shop_sect_3.owfn"
cmd1="$FIONA $owfn -t eqR -t IG"

echo running $cmd1
OUTPUT=`$cmd1 2>&1`

echo $OUTPUT | grep "are equivalent: YES" > /dev/null
    equivalent=$?

if [ $equivalent -ne 0 ]
then
    echo   ... the two graphs do not characterize the same strategies
fi


result=`expr $result + $equivalent`

############################################################################
# check if graph using -R is the same as the one generated without -R
############################################################################
owfn="$DIR/dke07_shop_sect_6.owfn"
cmd1="$FIONA $owfn -t eqR -t IG"

echo running $cmd1
OUTPUT=`$cmd1 2>&1`

echo $OUTPUT | grep "are equivalent: YES" > /dev/null
    equivalent=$?

if [ $equivalent -ne 0 ]
then
    echo   ... the two graphs do not characterize the same strategies
fi


result=`expr $result + $equivalent`

############################################################################

############################################################################

#loeschen aller erzeugten Dateien, die im Durchlauf entstanden sind
rm -f $DIR/*.out
rm -f $DIR/*.OG.png
rm -f $DIR/*.IG.png
rm -f $DIR/*.og
rm -f $DIR/*.log

echo

exit $result
