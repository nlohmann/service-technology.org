#!/bin/bash

#---------------------------------------------------------------#
# Script for re-compiling and benchmarking Fiona as well as     #
# publishing the results to the wiki.                           #
#                                                               #
# It calls ./prepare.sh, ./start.sh and ./publish.sh. See the   #
# the information on these sripts.                              #
#                                                               #
# Call: ./all.sh <logfile> [<FionaDir> [<FionaSuffix>]]         #
#          <logfile> All results will be stored here            #
#          <FionaDir> The fiona root directory (if not ../../)  #
#          <FionaSuffix> The suffix that is used for            #
#                        executables on your system (if any)    #
#---------------------------------------------------------------#


if [ $1 ] 
then
        logfile=$1
else
			echo "Call: ./all.sh <logfile> [<FionaDir> [<FionaSuffix>]]"
        echo -e "\t No logfile given. EXIT."
        exit
fi

if [ $2 ] 
then
        fiona_dir=$2              
fi
        
if [ $3 ] 
then
        suffix=$3
fi

echo "Preparing..."

./prepare.sh $fiona_dir $suffix

echo "Starting..."

./start.sh $logfile $suffix


echo "Publishing"

./publish.sh $logfile
