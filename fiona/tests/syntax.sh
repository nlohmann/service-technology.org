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
DIR=$testdir/syntax
FIONA=fiona

#loeschen aller erzeugten Dateien im letzten Durchlauf
rm -f $DIR/*.out
rm -f $DIR/*.png
rm -f $DIR/*.og
rm -f $DIR/*.log

result=0

############################################################################

owfn="$DIR/max_occurrence.owfn"
cmd="$FIONA $owfn -t OG"
if [ "$memcheck" = "yes" ]; then
    memchecklog="$owfn.OG.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$?
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
    if [ $? -ne 0 ]; then
        echo ... fiona exited with nonzero return value although it should not
        result=1
    fi
fi

############################################################################

echo

exit $result
