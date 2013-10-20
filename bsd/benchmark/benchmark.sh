#!/bin/bash
# This script computes BSD and CSD for some given owfns and measures the performance of the bsd tool.

# parameter 1: directory to service-technology.org svn/trunk
if [ ! $1 ]; then
   echo "$0: Usage: $0 'directory to service-technology.org svn/trunk' 'directory with OWFNs' 'output directory' 'bound1' ['bound2'] ..."
   exit 1
fi

if [ ! -d $1 ]; then
   echo "$0: Directory doesn't exist, exiting..."
   exit 1
fi
APPS=$1

# parameter 2: directory with owfn files
if [ ! $2 ]; then
   echo "$0: Usage: $0 'directory to service-technology.org svn/trunk' 'directory with OWFNs' 'output directory' 'bound1' ['bound2'] ..."
   exit 2
fi

if [ ! -d $2 ]; then
   echo "$0: Directory doesn't exist, exiting..."
   exit 2
fi
INPUTDIR=`basename $2`

# parameter 3: directory for output
if [ ! $3 ]; then
   echo "$0: Usage: $0 'directory to service-technology.org svn/trunk' 'directory with OWFNs' 'output directory' 'bound1' ['bound2'] ..."
   exit 3
fi

if [ ! -d $3 ]; then
   echo "$0: Directory doesn't exist, exiting..."
   exit 3
fi
OUTDIR=`basename $3`

# parameter 4+: bounds
if [ ! $4 ]; then
   echo "$0: Usage: $0 'directory to service-technology.org svn/trunk' 'directory with OWFNs' 'output directory' 'bound1' ['bound2'] ..."
   exit 4
fi

for var in "${@:4}"; do
   for i in `find $2 -name "*.owfn" | sort`; do
      file=`basename $i | sed -e 's/\.owfn//'`
      location=`dirname $i`

      # computes BSD for owfn in '$INPUTDIR' to dots in '$OUTDIR'
      ${APPS}/bsd/src/bsd ${i} --bound=${var} --BSD --dotFile=${OUTDIR}/BSD_${file}.dot
      # computes CSD for owfn in '$INPUTDIR' to dots in '$OUTDIR'
      ${APPS}/bsd/src/bsd ${i} --bound=${var} --CSD --dotFile=${OUTDIR}/CSD_${file}.dot
   done
done

sh ./create_booktabs.sh ${OUTDIR}
