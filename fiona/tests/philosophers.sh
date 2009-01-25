#!/usr/bin/env bash

source defaults.sh

result=1

DIR=$testdir/philosophers
FIONA=fiona

for i in 3 4 5 6 7 8 9 10; do
    cmd="$FIONA -t og $DIR/phcontrol$i.unf.owfn -e1"
    if [ "$quiet" != "no" ]; then
        cmd="$cmd -Q"
    fi

    echo
    echo ---------------------------------------------------------------------
    echo running: $cmd
    $cmd 2>&1

    fionaExitCode=$?
    $evaluate $fionaExitCode

    if [ $? -ne 0 ]
    then
        result=1
    fi
    echo
done

exit $result
