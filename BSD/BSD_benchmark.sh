#!/bin/bash

# parameter 1: directory with owfn files
if [ ! $1 ]; then
   echo "$0: Usage: $0 'directory with OWFNs' 'output directory (or '-')' 'bound1' ['bound2'] ..."
   exit
fi

if [ ! -d $1 ]; then
   echo "$0: Source directory doesn't exist, exiting..."
   exit 1
fi

INPUTDIR=`basename $1`

# parameter 2: directory for output
if [ ! $2 ]; then
   echo "$0: Usage: $0 'directory with OWFNs' 'output directory (or '-')' 'bound1' ['bound2'] ..."
   exit
elif [ $2 == "-" ]; then
   # create new directory with timestamp
   OUTDIR=${INPUTDIR}_BSD_$(date +%Y%m%d_%H%M%S)
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

# parameter 3+: bounds
if [ ! $3 ]; then
   echo "$0: Usage: $0 'directory with OWFNs' 'output directory (or '-')' 'bound1' ['bound2'] ..."
   exit
fi

for var in "${@:3}"; do
   for i in `find $1 -name "*.owfn" | sort`; do
      file=`basename $i | sed -e 's/\.owfn//'`
      location=`dirname $i`

      # convert OWFNs in '$INPUTDIR' to dots in '$OUTDIR'
      sh BSD_BCSD2dot.sh $i ${var} $OUTDIR/BSD_${var}\(${file}\).dot $OUTDIR/CSD_${var}\(${file}\).dot
   done
done

sh BSD_create_table.sh ${OUTDIR}
