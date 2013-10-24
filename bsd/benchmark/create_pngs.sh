#!/bin/bash
# This script computes the pngs from the given dot files.
PARAMETERS="'input directory' 'output directory'"

# parameter 1: directory with dot files
if [ ! $1 ]; then
   echo "$0: Usage: $0 $PARAMETERS"
   exit 1
fi
if [ ! -d $1 ]; then
   echo "$0: Directory doesn't exist, exiting..."
   exit 1
fi

# parameter 2: directory for output
if [ ! $2 ]; then
   echo "$0: Usage: $0 $PARAMETERS"
   exit 2
fi
if [ ! -d $2 ]; then
   echo "$0: Directory doesn't exist, exiting..."
   exit 2
fi

for i in `find $1 -name "*.dot" | sort`; do
   file=`basename $i | sed -e 's/\.dot//'`

   # convert DOTs in '$INPUTDIR' to PNGs in '$OUTDIR'
   echo $i " --> " $2/${file}.png
   dot -Tpng $i -o $2/${file}.png

done

exit 0
