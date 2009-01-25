#!/usr/bin/env bash

#---------------------------------------------------------------#
# Helper-Script for re-compiling Fiona                          #
#                                                               #
# It is called by ./prepare.sh                                  #
#                                                               #
# Call: ./compileFiona.sh <FionaDir> <ON|OFF>                   #
#          <logfile> All results will be stored here            #
#          <ON|OFF> newlogger activated or not                  #
#---------------------------------------------------------------#

if [ $1 ] 
then
	FIONA_DIR=$1
else 
	echo "Call: ./compileFiona.sh <FionaDir> <ON|OFF>"
	echo -e "\tNo Fiona directory given. EXIT."
	exit
fi

if [ $2 ]
then
	NEWLOGGER=$2
else 
	echo "Call: ./compileFiona.sh <FionaDir> <ON|OFF>"
	echo -e "\tNo mode for the newlogger given. EXIT."
	exit
fi

echo "Fiona-Verzeichnis: $FIONA_DIR"


if [ $NEWLOGGER == "OFF" ] || [ $NEWLOGGER == "ON" ]
then
	echo "Newlogger: $NEWLOGGER"
else
	echo "Call: ./compileFiona.sh <FionaDir> <ON|OFF>"
	echo -e "\tMode for the newlogger neither ON nor OFF. EXIT."
	exit 
fi

 
echo "Creating backup of $FIONA_DIR/src/userconfig.h"
cp $FIONA_DIR/src/userconfig.h $FIONA_DIR/src/userconfig.h.old
if [ $2 == "OFF" ]
then
	cat $FIONA_DIR/src/userconfig.h | sed -e 's/#define LOG_NEW/\/\/#define LOG_NEW/'  > $FIONA_DIR/src/userconfig.h.new
	echo "Created new userconfig.h with newlogger OFF."
fi
if [ $2 == "ON" ]
then
	cat $FIONA_DIR/src/userconfig.h | sed -e 's/\/\/#define LOG_NEW/#define LOG_NEW/'  > $FIONA_DIR/src/userconfig.h.new
	echo "Created new userconfig.h with newlogger ON."
fi

echo "Replacing userconfig.h with new version..."
cp $FIONA_DIR/src/userconfig.h.new $FIONA_DIR/src/userconfig.h

echo "...done. Navigating to FIONA directory."

cd $FIONA_DIR
echo "Calling make..." 

make
echo "make done." 
