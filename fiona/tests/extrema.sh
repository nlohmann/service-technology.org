#!/usr/bin/env bash

source defaults.sh
source memcheck_helper.sh

echo
echo ---------------------------------------------------------------------
echo running $0
echo

DIR=$testdir/extrema FIONA=fiona

#loeschen aller erzeugten Dateien im letzten Durchlauf
rm -f $DIR/*.log

############################################################################

owfns=(
    $DIR/no_final_marking.owfn
    $DIR/all_final_marking.owfn
    $DIR/no_communication.owfn
    $DIR/no_initial_marking.owfn
    $DIR/no_initial_marking2.owfn
    $DIR/empty.owfn
#   $DIR/'filename*.owfn'
    $DIR/one_input.owfn
    $DIR/one_output.owfn
    $DIR/one_input_marked.owfn
    $DIR/one_output_marked.owfn
    $DIR/one_input_one_output.owfn
    $DIR/one_input_2.owfn
    $DIR/multiple_input.owfn
    $DIR/multiple_output.owfn
    $DIR/multiple_input_multiple_output.owfn
    $DIR/initial_marking_midways.owfn
    $DIR/initialmarking_is_a_finalmarking.owfn
    $DIR/non_empty_true_node.owfn
    $DIR/non_empty_true_node2.owfn
    $DIR/transient_finalmarking.owfn
)

expectations_controllable=(
    NO
    NO
    YES
    NO
    YES
    YES
#   YES
    YES
    YES
    NO
    YES
    YES
    NO
    YES
    YES
    YES
    YES
    YES
    YES
    YES
    NO
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
        result=$(($result | $?))
    else
        expectation_controllable="${expectations_controllable[$i]}"
        echo running $cmd
        OUTPUT=`$cmd 2>&1`
        fionaExitCode=$?
        $evaluate $fionaExitCode
        if [ $? -ne 0 ] 
        then
            result=1
        else
            echo $OUTPUT | grep "net is controllable: $expectation_controllable" \
                >/dev/null
            result1=$?
       
            if [ $result1 -ne 0 ] ; then
                echo     ... FAILED
                echo
                result=1
            fi
        fi
    fi
done;

echo

exit $result
