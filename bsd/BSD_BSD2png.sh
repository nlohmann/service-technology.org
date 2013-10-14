#!/bin/bash

# global: ALWAYS use apps from gna-svn
apps=/vol/fob-vol5/mi06/heiden/trunk/

# parameter 1: OWFN file
if [ ! $1 ]; then
   echo "$0: Usage: $0 'OWFN file' 'bound' ['output filename (.dot)']"
   exit
fi

if [ ! -f $1 ]; then
   echo "$0: file doesn't exist, exiting..."
   exit 1
fi

INPUTDIR=`dirname $1`
INPUTFILE=`basename $1 | sed -e 's/\.owfn//'`

# parameter 2: bound b
if [ ! $2 ]; then
   echo "$0: Usage: $0 'OWFN file' 'bound' ['output filename (.dot)']"
   exit
fi


# parameter 3: filename for output
if [ ! $3 ]; then
   OUTDIR=`dirname $1`
   OUTFILE="BSD_${2}(${INPUTFILE}).dot"
else
   OUTDIR=`dirname $3`
   OUTFILE=`basename $3`
fi

if [ ! -d $OUTDIR ]; then
   mkdir ${OUTDIR}
   if [ $? -ne 0 ]; then
      echo "$0: Can't create directory, exiting..."
      exit 1
   fi
fi


OUTFILE=`echo $OUTFILE | sed -e 's/\.dot//'`

echo $1 " --> " ${OUTDIR}/${OUTFILE}.dot
${apps}/BSD/src/BSD --BSD ${INPUTDIR}/${INPUTFILE}.owfn --dotFile=${OUTDIR}/${OUTFILE}.dot --bound=${2}

echo ${OUTDIR}/${OUTFILE}.dot " --> " ${OUTDIR}/${OUTFILE}.png
dot -Tpng ${OUTDIR}/${OUTFILE}.dot -o ${OUTDIR}/${OUTFILE}.png

