#!/bin/sh

if [ $# -lt 1 ]; then
    echo "$0 <net>"
    exit 2
fi

net=$1

while read task; do
    name=`echo $task | grep -o "^[a-zA-Z0-9]*"`
    formula=`echo $task | grep -o "= .*$" | cut -c 2-`
    ../src/patricia $name $net "$formula" 2> /dev/null
done 2> /dev/null

exit 0
