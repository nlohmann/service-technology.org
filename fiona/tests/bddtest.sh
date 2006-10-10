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
DIR=$testdir/sequence_suite
FIONA=fiona

#loeschen aller erzeugten Dateien im letzten Durchlauf
rm -f $DIR/*.out
rm -f $DIR/*.png
rm -f $DIR/*.cudd

############################################################################

echo running $FIONA -n $DIR/sequence5.owfn -a -b 4 -t OG
$FIONA -n $DIR/sequence5.owfn -a -b 4 --graphtype=OG  2> /dev/null 1> /dev/null
result5=$?

if [ $result5 -ne 0 ] ; then
    echo     ... FAILED
    echo
fi

############################################################################

if test \( $result5 -eq 0 \) -a \
    \( -f $DIR/sequence5.owfn.a.OG.png -a -f $DIR/sequence5.owfn.a.OG.out \)
then
  result=0
else
  result=1
fi

echo

exit $result

