#!/bin/bash

source defaults.sh
source memcheck_helper.sh

echo
echo ---------------------------------------------------------------------
echo running $0
echo

SUBDIR=publicview
DIR=$testdir/$SUBDIR
FIONA=fiona

# for make distcheck: create directory $builddir/$SUBDIR for writing output files to
if [ "$testdir" != "$builddir" ]; then
    if [ ! -e $builddir/$SUBDIR ]; then
        $MKDIR_P $builddir/$SUBDIR
    fi
fi

#loeschen aller erzeugten Dateien im letzten Durchlauf
rm -f $DIR/NonFinalTauNode1.pv.sa.out
rm -f $DIR/NonFinalTauNode1.pv.sa.png
rm -f $DIR/NonFinalTauNode1.pv.owfn
rm -f $DIR/NonFinalTauNode2.pv.sa.out
rm -f $DIR/NonFinalTauNode2.pv.sa.png
rm -f $DIR/NonFinalTauNode2.pv.owfn
rm -f $DIR/NonFinalTauNode3.pv.sa.out
rm -f $DIR/NonFinalTauNode3.pv.sa.png
rm -f $DIR/NonFinalTauNode3.pv.owfn
rm -f $DIR/NonFinalTauNode4.pv.sa.out
rm -f $DIR/NonFinalTauNode4.pv.sa.png
rm -f $DIR/NonFinalTauNode4.pv.owfn
rm -f $DIR/NonFinalTauNode1not.pv.sa.out
rm -f $DIR/NonFinalTauNode1not.pv.sa.png
rm -f $DIR/NonFinalTauNode1not.pv.owfn
rm -f $DIR/NonFinalTauNode2not.pv.sa.out
rm -f $DIR/NonFinalTauNode2not.pv.sa.png
rm -f $DIR/NonFinalTauNode2not.pv.owfn
rm -f $DIR/non_empty_true_node2.pv.sa.out
rm -f $DIR/non_empty_true_node2.pv.sa.png
rm -f $DIR/non_empty_true_node2.pv.owfn
rm -f $DIR/non_empty_true_node3.pv.sa.out
rm -f $DIR/non_empty_true_node3.pv.sa.png
rm -f $DIR/non_empty_true_node3.pv.owfn
rm -f $DIR/complexPV.pv.sa.out
rm -f $DIR/complexPV.pv.sa.png
rm -f $DIR/complexPV.pv.owfn
rm -f $DIR/*.log

service="$DIR/non_empty_true_node2"

result=0

############################################################################
# complexPV equivalence
############################################################################

service="$DIR/complexPV"
output="$builddir/$SUBDIR/complexPV"
cmd="$FIONA $service.owfn -t pv -o $output -p no-dot"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$service.owfn.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
    fionaExitCode=$?
    $evaluate $fionaExitCode
    if [ $? -ne 0 ] 
    then
        result=1
    else
        cmd="$FIONA $output.pv.owfn $service.owfn -t equivalence"
        
        if [ "$quiet" != "no" ]; then
            cmd="$cmd -Q"
        fi
        
        if [ "$memcheck" = "yes" ]; then
            memchecklog="$service.sa.owfn.memcheck.log"
            do_memcheck "$cmd" "$memchecklog"
            result=$(($result | $?))
        else
            echo running $cmd
            OUTPUT=`$cmd 2>&1`
            fionaExitCode=$?
            $evaluate $fionaExitCode
            if [ $? -ne 0 ] 
            then
                result=1
            else
                echo $OUTPUT | grep "are equivalent: YES" > /dev/null
                resultSIM=$?
                if [ $resultSIM -ne 0 ]; then
                    let "result += 1"
                    echo ... equivalence check of the OGs of the service and its public view failed.
                fi
            fi
        fi
    fi
fi

############################################################################
# non_empty_true_node2 equivalence
############################################################################

service="$DIR/non_empty_true_node2"
output="$builddir/$SUBDIR/non_empty_true_node2"
cmd="$FIONA $service.owfn -t pv -o $output -p no-dot"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$service.owfn.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
    fionaExitCode=$?
    $evaluate $fionaExitCode
    if [ $? -ne 0 ] 
    then
        result=1
    else

        cmd="$FIONA $output.pv.owfn $service.owfn -t equivalence"
        
        if [ "$quiet" != "no" ]; then
            cmd="$cmd -Q"
        fi
        
        if [ "$memcheck" = "yes" ]; then
            memchecklog="$service.sa.owfn.memcheck.log"
            do_memcheck "$cmd" "$memchecklog"
            result=$(($result | $?))
        else
            echo running $cmd
            OUTPUT=`$cmd 2>&1`
            fionaExitCode=$?
            $evaluate $fionaExitCode
            if [ $? -ne 0 ] 
            then
                result=1
            else
                echo $OUTPUT | grep "are equivalent: YES" > /dev/null
                resultSIM=$?
                if [ $resultSIM -ne 0 ]; then
                    let "result += 1"
                    echo ... equivalence check of the OGs of the service and its public view failed.
                fi
            fi
        fi
    fi
fi

############################################################################
# non_empty_true_node3 equivalence
############################################################################

service="$DIR/non_empty_true_node3"
output="$builddir/$SUBDIR/non_empty_true_node3"
cmd="$FIONA $service.owfn -t pv -o $output -p no-dot"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$service.owfn.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
    fionaExitCode=$?
    $evaluate $fionaExitCode
    if [ $? -ne 0 ] 
    then
        result=1
    else

        cmd="$FIONA $output.pv.owfn $service.owfn -t equivalence"
        
        if [ "$quiet" != "no" ]; then
            cmd="$cmd -Q"
        fi
        
        if [ "$memcheck" = "yes" ]; then
            memchecklog="$service.sa.owfn.memcheck.log"
            do_memcheck "$cmd" "$memchecklog"
            result=$(($result | $?))
        else
            echo running $cmd
            OUTPUT=`$cmd 2>&1`
            fionaExitCode=$?
            $evaluate $fionaExitCode
            if [ $? -ne 0 ] 
            then
                result=1
            else
                echo $OUTPUT | grep "are equivalent: YES" > /dev/null
                resultSIM=$?
                if [ $resultSIM -ne 0 ]; then
                    let "result += 1"
                    echo ... equivalence check of the OGs of the service and its public view failed.
                fi
            fi
        fi
    fi
fi

############################################################################
# start of the outcommentable detailed test                                #
############################################################################
# <<OUTCOMMENTED


echo

exit $result


############################################################################
# NonFinalTauNode1 equivalence
############################################################################

service="$DIR/NonFinalTauNode1"
output="$builddir/$SUBDIR/NonFinalTauNode1"
cmd="$FIONA $service.owfn -t pv -o $output -p no-dot"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$service.owfn.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
    fionaExitCode=$?
    $evaluate $fionaExitCode
    if [ $? -ne 0 ] 
    then
        result=1
    else
        cmd="$FIONA $output.pv.owfn $service.owfn -t equivalence"
        
        if [ "$quiet" != "no" ]; then
            cmd="$cmd -Q"
        fi
        
        if [ "$memcheck" = "yes" ]; then
            memchecklog="$service.sa.owfn.memcheck.log"
            do_memcheck "$cmd" "$memchecklog"
            result=$(($result | $?))
        else
            echo running $cmd
            OUTPUT=`$cmd 2>&1`
            fionaExitCode=$?
            $evaluate $fionaExitCode
            if [ $? -ne 0 ] 
            then
                result=1
            else
                echo $OUTPUT | grep "are equivalent: YES" > /dev/null
                resultSIM=$?
                if [ $exitcode -ne 0 -o $resultSIM -ne 0 ]; then
                    let "result += 1"
                    echo ... equivalence check of the OGs of the service and its public view failed.
                fi
            fi
        fi
    fi
fi

############################################################################
# NonFinalTauNode2 equivalence
############################################################################

service="$DIR/NonFinalTauNode2"
output="$builddir/$SUBDIR/NonFinalTauNode2"
cmd="$FIONA $service.owfn -t pv -o $output -p no-dot"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$service.owfn.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
    fionaExitCode=$?
    $evaluate $fionaExitCode
    if [ $? -ne 0 ] 
    then
        result=1
    else

        cmd="$FIONA $output.pv.owfn $service.owfn -t equivalence"
        
        if [ "$quiet" != "no" ]; then
            cmd="$cmd -Q"
        fi
        
        if [ "$memcheck" = "yes" ]; then
            memchecklog="$service.sa.owfn.memcheck.log"
            do_memcheck "$cmd" "$memchecklog"
            result=$(($result | $?))
        else
            echo running $cmd
            OUTPUT=`$cmd 2>&1`
            fionaExitCode=$?
            $evaluate $fionaExitCode
            if [ $? -ne 0 ] 
            then
                result=1
            else
                echo $OUTPUT | grep "are equivalent: YES" > /dev/null
                resultSIM=$?
                if [ $exitcode -ne 0 -o $resultSIM -ne 0 ]; then
                    let "result += 1"
                    echo ... equivalence check of the OGs of the service and its public view failed.
                fi
            fi
        fi
    fi
fi

############################################################################
# NonFinalTauNode3 equivalence
############################################################################

service="$DIR/NonFinalTauNode3"
output="$builddir/$SUBDIR/NonFinalTauNode3"
cmd="$FIONA $service.owfn -t pv -o $output -p no-dot"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$service.owfn.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
    fionaExitCode=$?
    $evaluate $fionaExitCode
    if [ $? -ne 0 ] 
    then
        result=1
    else

        cmd="$FIONA $output.pv.owfn $service.owfn -t equivalence"
        
        if [ "$quiet" != "no" ]; then
            cmd="$cmd -Q"
        fi
        
        if [ "$memcheck" = "yes" ]; then
            memchecklog="$service.sa.owfn.memcheck.log"
            do_memcheck "$cmd" "$memchecklog"
            result=$(($result | $?))
        else
            echo running $cmd
            OUTPUT=`$cmd 2>&1`
            fionaExitCode=$?
            $evaluate $fionaExitCode
            if [ $? -ne 0 ] 
            then
                result=1
            else
                echo $OUTPUT | grep "are equivalent: YES" > /dev/null
                resultSIM=$?
                if [ $resultSIM -ne 0 ]; then
                    let "result += 1"
                    echo ... equivalence check of the OGs of the service and its public view failed.
                fi
            fi
        fi
    fi
fi

############################################################################
# NonFinalTauNode4 equivalence
############################################################################

service="$DIR/NonFinalTauNode4"
output="$builddir/$SUBDIR/NonFinalTauNode4"
cmd="$FIONA $service.owfn -t pv -o $output -p no-dot"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$service.owfn.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
    fionaExitCode=$?
    $evaluate $fionaExitCode
    if [ $? -ne 0 ] 
    then
        result=1
    else
        cmd="$FIONA $output.pv.owfn $service.owfn -t equivalence"
        
        if [ "$quiet" != "no" ]; then
            cmd="$cmd -Q"
        fi
        
        if [ "$memcheck" = "yes" ]; then
            memchecklog="$service.sa.owfn.memcheck.log"
            do_memcheck "$cmd" "$memchecklog"
            result=$(($result | $?))
        else
            echo running $cmd
            OUTPUT=`$cmd 2>&1`
            fionaExitCode=$?
            $evaluate $fionaExitCode
            if [ $? -ne 0 ] 
            then
                result=1
            else
                echo $OUTPUT | grep "are equivalent: YES" > /dev/null
                resultSIM=$?
                if [ $resultSIM -ne 0 ]; then
                    let "result += 1"
                    echo ... equivalence check of the OGs of the service and its public view failed.
                fi
            fi
        fi
    fi
fi

############################################################################
# NonFinalTauNode1not equivalence
############################################################################

service="$DIR/NonFinalTauNode1not"
output="$builddir/$SUBDIR/NonFinalTauNode1not"
cmd="$FIONA $service.owfn -t pv -o $output -p no-dot"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$service.owfn.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
    fionaExitCode=$?
    $evaluate $fionaExitCode
    if [ $? -ne 0 ] 
    then
        result=1
    else
    
        cmd="$FIONA $output.pv.owfn $service.owfn -t equivalence"
        
        if [ "$quiet" != "no" ]; then
            cmd="$cmd -Q"
        fi
        
        if [ "$memcheck" = "yes" ]; then
            memchecklog="$service.sa.owfn.memcheck.log"
            do_memcheck "$cmd" "$memchecklog"
            result=$(($result | $?))
        else
            echo running $cmd
            OUTPUT=`$cmd 2>&1`
            fionaExitCode=$?
            $evaluate $fionaExitCode
            if [ $? -ne 0 ] 
            then
                result=1
            else
                echo $OUTPUT | grep "are equivalent: YES" > /dev/null
                resultSIM=$?
                if [ $resultSIM -ne 0 ]; then
                    let "result += 1"
                    echo ... equivalence check of the OGs of the service and its public view failed.
                fi
            fi
        fi
    fi
fi

############################################################################
# NonFinalTauNode2not equivalence
############################################################################

service="$DIR/NonFinalTauNode2not"
output="$builddir/$SUBDIR/NonFinalTauNode2not"
cmd="$FIONA $service.owfn -t pv -o $output -p no-dot"

if [ "$memcheck" = "yes" ]; then
    memchecklog="$service.owfn.memcheck.log"
    do_memcheck "$cmd" "$memchecklog"
    result=$(($result | $?))
else
    echo running $cmd
    OUTPUT=`$cmd 2>&1`
    fionaExitCode=$?
    $evaluate $fionaExitCode
    if [ $? -ne 0 ] 
    then
        result=1
    else
        cmd="$FIONA $output.pv.owfn $service.owfn -t equivalence"
        
        if [ "$quiet" != "no" ]; then
            cmd="$cmd -Q"
        fi
        
        if [ "$memcheck" = "yes" ]; then
            memchecklog="$service.sa.owfn.memcheck.log"
            do_memcheck "$cmd" "$memchecklog"
            result=$(($result | $?))
        else
            echo running $cmd
            OUTPUT=`$cmd 2>&1`
            fionaExitCode=$?
            $evaluate $fionaExitCode
            if [ $? -ne 0 ] 
            then
                result=1
            else
                echo $OUTPUT | grep "are equivalent: YES" > /dev/null
                resultSIM=$?
                if [ $resultSIM -ne 0 ]; then
                    let "result += 1"
                    echo ... equivalence check of the OGs of the service and its public view failed.
                fi
            fi
        fi
    fi
fi


############################################################################
# end of the outcommentable detailed test                                  #
############################################################################
# OUTCOMMENTED


echo

exit $result
