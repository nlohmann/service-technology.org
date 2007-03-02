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

result=1

testdir=.
DIR=$testdir/philosophers
FIONA=fiona

#loeschen aller erzeugten Dateien im letzten Durchlauf
rm -f $DIR/*.out
rm -f $DIR/*.png
rm -f $DIR/*.og
rm -f $DIR/*.log

echo
echo ---------------------------------------------------------------------
echo running: $FIONA -a -t OG -n $DIR/phcontrol3.unf.owfn -e1 -m1
echo
$FIONA -a -t OG -n $DIR/phcontrol3.unf.owfn -e1 -m1

echo
echo ---------------------------------------------------------------------
echo running: $FIONA -a -t OG -n $DIR/phcontrol4.unf.owfn -e1 -m1
echo
$FIONA -a -t OG -n $DIR/phcontrol4.unf.owfn -e1 -m1

echo
echo ---------------------------------------------------------------------
echo running: $FIONA -a -t OG -n $DIR/phcontrol5.unf.owfn -e1 -m1
echo
$FIONA -a -t OG -n $DIR/phcontrol5.unf.owfn -e1 -m1

echo
echo ---------------------------------------------------------------------
echo running: $FIONA -a -t OG -n $DIR/phcontrol6.unf.owfn -e1 -m1
echo
$FIONA -a -t OG -n $DIR/phcontrol6.unf.owfn -e1 -m1

echo
echo ---------------------------------------------------------------------
echo running: $FIONA -a -t OG -n $DIR/phcontrol7.unf.owfn -e1 -m1
echo
$FIONA -a -t OG -n $DIR/phcontrol7.unf.owfn -e1 -m1

echo
echo ---------------------------------------------------------------------
echo running: $FIONA -a -t OG -n $DIR/phcontrol8.unf.owfn -e1 -m1
echo
$FIONA -a -t OG -n $DIR/phcontrol8.unf.owfn -e1 -m1

echo
echo ---------------------------------------------------------------------
echo running: $FIONA -a -t OG -n $DIR/phcontrol9.unf.owfn -e1 -m1
echo
$FIONA -a -t OG -n $DIR/phcontrol9.unf.owfn -e1 -m1

echo
echo ---------------------------------------------------------------------
echo running: $FIONA -a -t OG -n $DIR/phcontrol10.unf.owfn -e1 -m1
echo
$FIONA -a -t OG -n $DIR/phcontrol10.unf.owfn -e1 -m1

exit $result
