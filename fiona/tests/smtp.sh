#!/bin/bash

source defaults.sh

result=1

DIR=$testdir/smtp
FIONA=fiona

echo ---------------------------------------------------------------------

for i in 3 4 5; do
    cmd="$FIONA -t og $DIR/smtp$i.owfn"

    if [ "$quiet" != "no" ]; then
        cmd="$cmd -Q"
    fi

    echo
    echo running: $cmd
    $cmd 2>&1
    fionaExitCode=$?
    $evaluate $fionaExitCode
    if [ $? -ne 0 ] 
    then
        result=1
    else
        echo
        $cmd
    fi
done

exit $result

echo ---------------------------------------------------------------------
