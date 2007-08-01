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

DIR=$testdir/extrema FIONA=fiona

#loeschen aller erzeugten Dateien im letzten Durchlauf
rm -f $DIR/*.out
rm -f $DIR/*.png
rm -f $DIR/*.og
rm -f $DIR/*.log

############################################################################

owfns=(
    $DIR/no_final_marking.owfn
    $DIR/all_final_marking.owfn
    $DIR/no_communication.owfn
    $DIR/no_initial_marking.owfn
    $DIR/no_initial_marking2.owfn
    $DIR/empty.owfn
    $DIR/one_input.owfn
    $DIR/one_output.owfn
    $DIR/one_input_marked.owfn
    $DIR/one_output_marked.owfn
    $DIR/one_input_one_output.owfn
    $DIR/one_input_2.owfn
    $DIR/multiple_input.owfn
    $DIR/multiple_output.owfn
    $DIR/multiple_input_multiple_output.owfn
)

expectations_controllable=(
    NO
    NO
    YES
    NO
    YES
    YES
    YES
    YES
    NO
    YES
    YES
    NO
    YES
    YES
    YES
)

result=0

for ((i=0; i<${#owfns[*]}; ++i)) do
    owfn="${owfns[$i]}"
    cmd="$FIONA $owfn -t OG"

    if [ "$quiet" != "no" ]; then
        cmd="$cmd -Q"
    fi

    if [ "$memcheck" = "yes" ]; then
        memchecklog="$owfn.OG.memcheck.log"
        do_memcheck "$cmd" "$memchecklog"
        result=$?
    else
        expectation_controllable="${expectations_controllable[$i]}"
        echo running $cmd
        $cmd 2>&1 | grep "net is controllable: $expectation_controllable" \
            >/dev/null
        result1=$?

        if [ $result1 -ne 0 ] ; then
            echo     ... FAILED
            echo
            result=1
        fi
    fi
done;

echo

exit $result
