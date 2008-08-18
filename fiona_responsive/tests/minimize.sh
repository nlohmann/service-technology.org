#!/bin/bash

############################################################################
# Copyright 2008 Peter Massuthe                                            #
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

DIR=$testdir/minimize
FIONA="fiona"

#loeschen aller erzeugten Dateien im letzten Durchlauf
rm -f $DIR/4-loop.og.minimal.og
rm -f $DIR/4-loop.og.minimal.og.out
rm -f $DIR/4-loop.og.minimal.og.png
rm -f $DIR/06-03-23_BPM06_shop_sect_3.owfn.og.minimal.og
rm -f $DIR/06-03-23_BPM06_shop_sect_3.owfn.og.minimal.og.out
rm -f $DIR/06-03-23_BPM06_shop_sect_3.owfn.og.minimal.og.png
rm -f $DIR/final_non-final.og.minimal.og
rm -f $DIR/final_non-final.og.minimal.og.out
rm -f $DIR/final_non-final.og.minimal.og.png
rm -f $DIR/keesCoffee_condition.emptyNode.og.minimal.og
rm -f $DIR/keesCoffee_condition.emptyNode.og.minimal.og.out
rm -f $DIR/keesCoffee_condition.emptyNode.og.minimal.og.png
rm -f $DIR/keesCoffee_condition.og.minimal.og
rm -f $DIR/keesCoffee_condition.og.minimal.og.out
rm -f $DIR/keesCoffee_condition.og.minimal.og.png
rm -f $DIR/myCoffee.og.minimal.og
rm -f $DIR/myCoffee.og.minimal.og.out
rm -f $DIR/myCoffee.og.minimal.og.png
rm -f $DIR/non-final_final.og.minimal.og
rm -f $DIR/non-final_final.og.minimal.og.out
rm -f $DIR/non-final_final.og.minimal.og.png
rm -f $DIR/nostrat.og.minimal.og
rm -f $DIR/nostrat.og.minimal.og.out
rm -f $DIR/nostrat.og.minimal.og.png
rm -f $DIR/sample.og.minimal.og
rm -f $DIR/sample.og.minimal.og.out
rm -f $DIR/sample.og.minimal.og.png
rm -f $DIR/*.log

result=0

############################################################################

file1="$DIR/nostrat"
cmd="$FIONA -t minimize $file1.og -p no-png"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$file1.og.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
fi

exitcode=$?
if [ $exitcode -ne 0 ]; then
  echo ... failed to minimize OG.
fi
result=$(($result | $exitcode))

############################################################################

cmd="$FIONA -t equivalence $file1.og $file1.og.minimal.og -p no-png"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$file1.equivalence.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
fi

exitcode=$?
if [ $exitcode -ne 0 ]; then
  echo ... minimized OG not equivalent to original OG.
fi
result=$(($result | $exitcode))

############################################################################

file2="$DIR/final_non-final"
cmd="$FIONA -t minimize $file2.og -p no-png"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$file2.og.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"

else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
fi

exitcode=$?
if [ $exitcode -ne 0 ]; then
  echo ... failed to minimize OG.
fi
result=$(($result | $exitcode))

############################################################################

cmd="$FIONA -t equivalence $file2.og $file2.og.minimal.og -p no-png"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$file2.equivalence.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
fi

exitcode=$?
if [ $exitcode -ne 0 ]; then
  echo ... minimized OG not equivalent to original OG.
fi
result=$(($result | $exitcode))

############################################################################

file3="$DIR/non-final_final"
cmd="$FIONA -t minimize $file3.og -p no-png"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$file3.og.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
fi

exitcode=$?
if [ $exitcode -ne 0 ]; then
  echo ... failed to minimize OG.
fi
result=$(($result | $exitcode))

############################################################################

cmd="$FIONA -t equivalence $file3.og $file3.og.minimal.og -p no-png"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$file3.equivalence.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
fi

exitcode=$?
if [ $exitcode -ne 0 ]; then
  echo ... minimized OG not equivalent to original OG.
fi
result=$(($result | $exitcode))

############################################################################

cmd="$FIONA -t equivalence $file2.og.minimal.og $file3.og.minimal.og -p no-png"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$file2-3.equivalence.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
fi

exitcode=$?
if [ $exitcode -ne 0 ]; then
  echo ... minimized OGs not equivalent to each other.
fi
result=$(($result | $exitcode))

############################################################################

file4="$DIR/4-loop"
cmd="$FIONA -t minimize $file4.og -p no-png"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$file4.og.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
fi

exitcode=$?
if [ $exitcode -ne 0 ]; then
  echo ... failed to minimize OG.
fi
result=$(($result | $exitcode))

############################################################################

cmd="$FIONA -t equivalence $file4.og $file4.og.minimal.og -p no-png"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$file4.equivalence.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
fi

exitcode=$?
if [ $exitcode -ne 0 ]; then
  echo ... minimized OG not equivalent to original OG.
fi
result=$(($result | $exitcode))

############################################################################

file5="$DIR/sample"
cmd="$FIONA -t minimize $file5.og -p no-png"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$file5.og.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
fi

exitcode=$?
if [ $exitcode -ne 0 ]; then
  echo ... failed to minimize OG.
fi
result=$(($result | $exitcode))

############################################################################

cmd="$FIONA -t equivalence $file5.og $file5.og.minimal.og -p no-png"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$file5.equivalence.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
fi

exitcode=$?
if [ $exitcode -ne 0 ]; then
  echo ... minimized OG not equivalent to original OG.
fi
result=$(($result | $exitcode))

############################################################################

file6="$DIR/myCoffee"
cmd="$FIONA -t minimize $file6.og -p no-png"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$file6.og.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
fi

exitcode=$?
if [ $exitcode -ne 0 ]; then
  echo ... failed to minimize OG.
fi
result=$(($result | $exitcode))

############################################################################

cmd="$FIONA -t equivalence $file6.og $file6.og.minimal.og -p no-png"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$file6.equivalence.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
fi

exitcode=$?
if [ $exitcode -ne 0 ]; then
  echo ... minimized OG not equivalent to original OG.
fi
result=$(($result | $exitcode))

############################################################################

file7="$DIR/keesCoffee_condition"
cmd="$FIONA -t minimize $file7.og -p no-png"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$file7.og.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
fi

exitcode=$?
if [ $exitcode -ne 0 ]; then
  echo ... failed to minimize OG.
fi
result=$(($result | $exitcode))

############################################################################

cmd="$FIONA -t equivalence $file7.og $file7.og.minimal.og -p no-png"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$file7.equivalence.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
fi

exitcode=$?
if [ $exitcode -ne 0 ]; then
  echo ... minimized OG not equivalent to original OG.
fi
result=$(($result | $exitcode))

############################################################################

file8="$DIR/keesCoffee_condition.emptyNode"
cmd="$FIONA -t minimize $file8.og -p no-png"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$file8.og.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
fi

exitcode=$?
if [ $exitcode -ne 0 ]; then
  echo ... failed to minimize OG.
fi
result=$(($result | $exitcode))

############################################################################

cmd="$FIONA -t equivalence $file8.og $file8.og.minimal.og -p no-png"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$file8.equivalence.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
fi

exitcode=$?
if [ $exitcode -ne 0 ]; then
  echo ... minimized OG not equivalent to original OG.
fi
result=$(($result | $exitcode))

############################################################################

file9="$DIR/06-03-23_BPM06_shop_sect_3.owfn"
cmd="$FIONA -t minimize $file9.og -p no-png"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$file9.og.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
fi

exitcode=$?
if [ $exitcode -ne 0 ]; then
  echo ... failed to minimize OG.
fi
result=$(($result | $exitcode))

############################################################################

cmd="$FIONA -t equivalence $file9.og $file9.og.minimal.og -p no-png"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$file9.equivalence.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
fi

exitcode=$?
if [ $exitcode -ne 0 ]; then
  echo ... minimized OG not equivalent to original OG.
fi
result=$(($result | $exitcode))

############################################################################

echo
echo result: $result
echo

exit $result
