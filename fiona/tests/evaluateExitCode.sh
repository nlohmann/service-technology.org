#!/usr/bin/env bash

errorcode[2]="Bad Call"
errorcode[4]="File Error"
errorcode[5]="Memory Exhausted"

source defaults.sh

if [ 2 -gt $1 ]
then
    exit 0
else 
    echo "WARNING: Fiona exited with exit code "$1", result invalid. (" ${errorcode[$1]} ")"
fi
exit 1