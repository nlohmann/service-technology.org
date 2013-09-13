#!/bin/bash

# global: ALWAYS use apps from gna-svn
apps=/vol/fob-vol5/mi06/heiden/trunk/

# parameter 1: OWFN file
if [ ! $1 ]; then
   echo "$0: Usage: $0 'OWFN file' 'bound' ['BSD output filename (.dot)'] ['CSD output filename (.dot)']"
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
   echo "$0: Usage: $0 'OWFN file' 'bound' ['output filename']"
   exit
fi


# parameter 3: BSD output filename
if [ ! $3 ]; then
   OUTDIR1=`dirname $1`
   OUTFILE1="BSD_${2}(${INPUTFILE}).dot"
else
   OUTDIR1=`dirname $3`
   OUTFILE1=`basename $3`
fi

if [ ! -d $OUTDIR1 ]; then
   mkdir ${OUTDIR1}
   if [ $? -ne 0 ]; then
      echo "$0: Can't create directory, exiting..."
      exit 1
   fi
fi

# parameter 4: CSD output filename
if [ ! $3 ]; then
   OUTDIR2=`dirname $1`
   OUTFILE2="CSD_${2}(${INPUTFILE}).dot"
else
   OUTDIR2=`dirname $4`
   OUTFILE2=`basename $4`
fi

if [ ! -d $OUTDIR2 ]; then
   mkdir ${OUTDIR2}
   if [ $? -ne 0 ]; then
      echo "$0: Can't create directory, exiting..."
      exit 1
   fi
fi

OUTFILE1=`echo $OUTFILE1 | sed -e 's/\.dot//'`
OUTFILE2=`echo $OUTFILE2 | sed -e 's/\.dot//'`

echo $1 " --> " ${OUTDIR1}/${OUTFILE1}.dot, ${OUTDIR2}/${OUTFILE2}.dot
${apps}/BSD/src/BSD ${INPUTDIR}/${INPUTFILE}.owfn --bound=${2} --CSD --BSD

mv "${INPUTDIR}/BSD_${2}(${INPUTFILE}).dot" ${OUTDIR1}/${OUTFILE1}.dot
mv "${INPUTDIR}/CSD_${2}(${INPUTFILE}).dot" ${OUTDIR2}/${OUTFILE2}.dot

#echo ${OUTDIR1}/${OUTFILE1}.dot " --> " ${OUTDIR1}/${OUTFILE1}.png
#dot -Tpng ${OUTDIR1}/${OUTFILE1}.dot -o ${OUTDIR1}/${OUTFILE1}.png

#echo ${OUTDIR2}/${OUTFILE2}.dot " --> " ${OUTDIR2}/${OUTFILE2}.png
#dot -Tpng ${OUTDIR2}/${OUTFILE2}.dot -o ${OUTDIR2}/${OUTFILE2}.png

