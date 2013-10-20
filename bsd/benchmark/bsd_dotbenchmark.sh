#!/bin/bash

# parameter 1: directory with owfn files
if [ ! $1 ]; then
   echo "$0: Usage: $0 'directory with OWFNs' 'bound' ['output directory']"
   exit
fi

if [ ! -d $1 ]; then
   echo "$0: Source directory doesn't exist, exiting..."
   exit 1
fi

INPUTDIR=`basename $1`

# parameter 2: bound b
if [ ! $2 ]; then
   echo "$0: Usage: $0 'directory with OWFNs' 'bound' ['output directory']"
   exit
fi

# parameter 3: directory for output
if [ ! $3 ]; then
   # create new directory with timestamp
   OUTDIR=${INPUTDIR}_BSD_$(date +%Y%m%d_%H%M%S)
else
   OUTDIR=`basename $3`
fi

if [ ! -d $OUTDIR ]; then
   mkdir ${OUTDIR}
   if [ $? -ne 0 ]; then
      echo "$0: Can't create directory, exiting..."
      exit 1
   fi
fi


for i in `find $1 -name "*.owfn" | sort`; do
   file=`basename $i | sed -e 's/\.owfn//'`
   location=`dirname $i`

   # convert OWFNs in '$INPUTDIR' to dots in '$OUTDIR'
   sh bsd_BCSD2dot.sh $i $2 $OUTDIR/BSD_${2}\(${file}\).dot $OUTDIR/CSD_${2}\(${file}\).dot

done

