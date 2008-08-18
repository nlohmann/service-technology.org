#!/bin/bash

#---------------------------------------------------------------#
# Script for preparing a benchmark. Re-Compiles Fiona:          #
#          with newlogger activated for memory values           #
#          wihout newlogger activated for other values          #
#                                                               #
# Call: ./prepare.sh [<FionaDirectory> [<FionaSuffix>]]         #
#          <FionaDirectory> The root fiona directory            #
#          <FionaSuffix> The suffix that is used for            #
#                        executables on your system (if any)    #
#---------------------------------------------------------------#

echo Preparing: Re-Compiling Fiona. 

#fiona directory can be specified as first parameter
FIONA_DIR="../../"
if [ $1 ]
then
	 FIONA_DIR=$1
else
	echo Fiona directory was not given, assuming $FIONA_DIR. 
fi

FIONA_SUFFIX=""
if [ $2 ]
then
	FIONA_SUFFIX=".$2"
fi

echo Creating backup of old Fiona:
echo 
cp $FIONA_DIR/src/fiona$FIONA_SUFFIX $FIONA_DIR/src/fiona$FIONA_SUFFIX.backup 

./helpers/compileFiona.sh $FIONA_DIR ON
echo "Copying Fiona (with new logger) to the working directory"
FIONA_WN="./fiona_with_new_logger$FIONA_SUFFIX"
cp $FIONA_DIR/src/fiona$FIONA_SUFFIX $FIONA_WN

./helpers/compileFiona.sh $FIONA_DIR OFF
FIONA="./fiona$FIONA_SUFFIX"
echo "Copying Fiona to the working directory"
cp $FIONA_DIR/src/fiona$FIONA_SUFFIX $FIONA
