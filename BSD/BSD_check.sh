#!/bin/bash

# global: ALWAYS use apps from gna-svn
apps=/vol/fob-vol5/mi06/heiden/trunk/

# parameter 1: OWFN file
if [ ! $1 ]; then
   echo "$0: Usage: $0 'OWFN file' 'OWFN file' 'bound'"
   exit
fi

if [ ! -f $1 ]; then
   echo "$0: file 1 doesn't exist, exiting..."
   exit 1
fi

INPUTDIR1=`dirname $1`
INPUTFILE1=`basename $1 | sed -e 's/\.owfn//'`

# parameter 2: OWFN file
if [ ! $2 ]; then
   echo "$0: Usage: $0 'OWFN file' 'OWFN file' 'bound'"
   exit
fi

if [ ! -f $2 ]; then
   echo "$0: file 2 doesn't exist, exiting..."
   exit 1
fi

INPUTDIR2=`dirname $2`
INPUTFILE2=`basename $2 | sed -e 's/\.owfn//'`

# parameter 3: bound b
if [ ! $3 ]; then
   echo "$0: Usage: $0 'OWFN file' 'OWFN file' 'bound'"
   exit
fi


${apps}/BSD/src/BSD --check ${INPUTDIR1}/${INPUTFILE1}.owfn ${INPUTDIR2}/${INPUTFILE2}.owfn --bound=${3}

