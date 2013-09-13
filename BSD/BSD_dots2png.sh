#!/bin/bash

# parameter 1: directory with dot files
if [ ! $1 ]; then
   echo "$0: Usage: $0 'directory with DOTs' ['output directory']"
   exit
fi

if [ ! -d $1 ]; then
   echo "$0: Source directory doesn't exist, exiting..."
   exit 1
fi

INPUTDIR=`basename $1`

# parameter 2: directory for output
if [ ! $2 ]; then
   # create new directory with timestamp
   OUTDIR=${INPUTDIR}
else
   OUTDIR=`basename $2`
fi

if [ ! -d $OUTDIR ]; then
   mkdir ${OUTDIR}
   if [ $? -ne 0 ]; then
      echo "$0: Can't create directory, exiting..."
      exit 1
   fi
fi


for i in `find $1 -name "*.dot" | sort`; do
   file=`basename $i | sed -e 's/\.dot//'`
   location=`dirname $i`

   # convert DOTs in '$INPUTDIR' to PNGs in '$OUTDIR'
   echo ${location}/${file}.dot " --> " ${OUTDIR}/${file}.png
   dot -Tpng ${location}/${file}.dot -o ${OUTDIR}/${file}.png

done

