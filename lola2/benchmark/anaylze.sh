#!/bin/sh

DATE=`date "+%Y-%m-%d"`
MACHINE=`uname -n`

for FILE in `ls -1 *.log`
do
    NETNAME=`basename $FILE .log | awk -F- '{ print $1 }'`
    STORE=`basename $FILE .log | awk -F- '{ print $2 }'`
    TEST=`basename $FILE .log | awk -F- '{ print $3 }'`
    MEMORY=`grep "memory consumption" $FILE | awk '{ print $4 }'`
    TIME=`grep "time consumption" $FILE | awk '{ print $4 }'`
    echo $DATE,$MACHINE,$NETNAME,$TEST,$STORE,$MEMORY,$TIME
done
