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

# create the pngs from the dot files
for i in `find $1 -name "*.dot" | sort`; do
   file=`basename $i | sed -e 's/\.dot//'`

   # don't process dot with more than 1000 states
   STATES=`grep -E '  #nodes:            ' ${i} | sed -r "s/  #nodes:            ([0-9]+)/\1/"`
   if [ "$STATES" -gt "1000" ]; then
	echo "skipping $i because it is too large"
	touch $2/${file}.skipped
   else
	# convert DOTs in '$INPUTDIR' to PNGs in '$OUTDIR'
	echo $i " --> " $2/${file}.png
	dot -Tpng $i -o $2/${file}.png
   fi
done

exit 0
