#!/bin/bash

source defaults.sh
source memcheck_helper.sh

echo
echo ---------------------------------------------------------------------
echo running $0
echo

SUBDIR=minimize
DIR=$testdir/$SUBDIR
FIONA=fiona

# for make distcheck: make copy of $owfn and work on it
if [ "$testdir" != "$builddir" ]; then
    if [ ! -e $builddir/$SUBDIR ]; then
        $MKDIR_P $builddir/$SUBDIR
    fi
fi

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
file1out="$builddir/$SUBDIR/nostrat"
cmd="$FIONA -t minimize $file1.og -o $file1out.og.minimal -p no-dot"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$file1.og.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
fi

fionaExitCode=$?
$evaluate $fionaExitCode
if [ $? -ne 0 ] 
then
    result=1
else
    result=0
fi

############################################################################

cmd="$FIONA -t equivalence $file1.og $file1out.og.minimal.og -p no-dot"

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
file2out="$builddir/$SUBDIR/final_non-final"
cmd="$FIONA -t minimize $file2.og -o $file2out.og.minimal -p no-dot"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$file2.og.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"

else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
fi

fionaExitCode=$?
$evaluate $fionaExitCode
if [ $? -ne 0 ] 
then
    result=1
else
    result=0
fi

############################################################################

cmd="$FIONA -t equivalence $file2.og $file2out.og.minimal.og -p no-dot"

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
file3out="$builddir/$SUBDIR/non-final_final"
cmd="$FIONA -t minimize $file3.og -o $file3out.og.minimal -p no-dot"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$file3.og.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
fi

fionaExitCode=$?
$evaluate $fionaExitCode
if [ $? -ne 0 ] 
then
    result=1
else
    result=0
fi

############################################################################

cmd="$FIONA -t equivalence $file3.og $file3out.og.minimal.og -p no-dot"

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

cmd="$FIONA -t equivalence $file2out.og.minimal.og $file3out.og.minimal.og -p no-dot"

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
file4out="$builddir/$SUBDIR/4-loop"
cmd="$FIONA -t minimize $file4.og -o $file4out.og.minimal -p no-dot"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$file4.og.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
fi

fionaExitCode=$?
$evaluate $fionaExitCode
if [ $? -ne 0 ] 
then
    result=1
else
    result=0
fi

############################################################################

cmd="$FIONA -t equivalence $file4.og $file4out.og.minimal.og -p no-dot"

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
file5out="$builddir/$SUBDIR/sample"
cmd="$FIONA -t minimize $file5.og -o $file5out.og.minimal -p no-dot"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$file5.og.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
fi

fionaExitCode=$?
$evaluate $fionaExitCode
if [ $? -ne 0 ] 
then
    result=1
else
    result=0
fi

############################################################################

cmd="$FIONA -t equivalence $file5.og $file5out.og.minimal.og -p no-dot"

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
file6out="$builddir/$SUBDIR/myCoffee"
cmd="$FIONA -t minimize $file6.og -o $file6out.og.minimal -p no-dot"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$file6.og.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
fi

fionaExitCode=$?
$evaluate $fionaExitCode
if [ $? -ne 0 ] 
then
    result=1
else
    result=0
fi

############################################################################

cmd="$FIONA -t equivalence $file6.og $file6out.og.minimal.og -p no-dot"

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
file7out="$builddir/$SUBDIR/keesCoffee_condition"
cmd="$FIONA -t minimize $file7.og -o $file7out.og.minimal -p no-dot"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$file7.og.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
fi

fionaExitCode=$?
$evaluate $fionaExitCode
if [ $? -ne 0 ] 
then
    result=1
else
    result=0
fi

############################################################################

cmd="$FIONA -t equivalence $file7.og $file7out.og.minimal.og -p no-dot"

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
file8out="$builddir/$SUBDIR/keesCoffee_condition.emptyNode"
cmd="$FIONA -t minimize $file8.og -o $file8out.og.minimal -p no-dot"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$file8.og.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
fi

fionaExitCode=$?
$evaluate $fionaExitCode
if [ $? -ne 0 ] 
then
    result=1
else
    result=0
fi

############################################################################

cmd="$FIONA -t equivalence $file8.og $file8out.og.minimal.og -p no-dot"

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
file9out="$builddir/$SUBDIR/06-03-23_BPM06_shop_sect_3.owfn"
cmd="$FIONA -t minimize $file9.og -o $file9out.og.minimal -p no-dot"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$file9.og.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
fi

fionaExitCode=$?
$evaluate $fionaExitCode
if [ $? -ne 0 ] 
then
    result=1
else
    result=0
fi

############################################################################

cmd="$FIONA -t equivalence $file9.og $file9out.og.minimal.og -p no-dot"

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

