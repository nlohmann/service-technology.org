#!/bin/bash
# This script computes the pngs from the given files.
PARAMETERS="'directory with PNapi' 'input directory' 'output directory'"

# parameter 1: directory with bsd checkout
if [ ! $1 ]; then
   echo "$0: Usage: $0 $PARAMETERS"
   exit 1
fi

if [ ! -d $1 ]; then
   echo "$0: Directory doesn't exist, exiting..."
   exit 1
fi

# parameter 2: directory with dot files
if [ ! $2 ]; then
   echo "$0: Usage: $0 $PARAMETERS"
   exit 2
fi
if [ ! -d $2 ]; then
   echo "$0: Directory doesn't exist, exiting..."
   exit 2
fi

# parameter 3: directory for output
if [ ! $3 ]; then
   echo "$0: Usage: $0 $PARAMETERS"
   exit 3
fi
if [ ! -d $3 ]; then
   echo "$0: Directory doesn't exist, exiting..."
   exit 3
fi

# dot files
for i in `find $2 -name "*.dot" | sort`; do
	file=`basename $i | sed -e 's/\.dot//'`
	pngfile=`basename $i | sed -e 's/\.owfn/\.png/'`

	# don't process dot with more than 1000 states
	STATES=`grep -E '  #nodes:            ' ${i} | sed -r "s/  #nodes:            ([0-9]+)/\1/"`
	if [ "$STATES" -gt "100" ]; then
		echo "Skipping '$i' because it is too large"
	elif [ -f ${pngfile} ]; then
		echo "File '${pngfile}' already exists."
	else
		# convert DOTs in '$INPUTDIR' to PNGs in '$OUTDIR'
		echo $i " --> " $3/${file}.png
		dot -Tpng $i -o $3/${file}.png
	fi
done

echo "# ----------------------------------------------------------------------------------------"

# owfn files
for i in `find $2 -name "*.owfn" | sort`; do
	file=`basename $i | sed -e 's/\.owfn//'`
	pngfile=`basename $i | sed -e 's/\.owfn/\.png/'`
	echo "Processing file '${file}'"
	if [ -f ${pngfile} ];
	then
		echo "File '${pngfile}' already exists."
	else
		$1/petri -v --removePorts --input=owfn --output=png --config=$1/petri.conf $i
	fi
done


exit 0
