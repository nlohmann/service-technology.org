#!/usr/bin/env bash

#-----------------------------------------------------------------#
# Script for re-compiling and benchmarking Fiona as well as       #
# publishing the results to the wiki.                             #
#                                                                 #
# It calls ./prepare.sh, ./start.sh and ./publish.sh. See the     #
# the information on these sripts.                                #
#                                                                 #
# Call: ./all.sh <logfile> <comment> [<FionaDir> [<FionaSuffix>]] #
#          <logfile> All results will be stored here              #
#          <comment> A file containing a comment.                 #
#          <FionaDir> The fiona root directory (if not ../../)    #
#          <FionaSuffix> The suffix that is used for              #
#                        executables on your system (if any)      #
#-----------------------------------------------------------------#


if [ $1 ] 
then
        logfile=$1
else
			echo "Call: ./all.sh <logfile> <comment> [<FionaDir> [<FionaSuffix>]]"
        echo -e "\t No logfile given. EXIT."
        exit
fi

if [ $2 ] 
then
        comment=$2
else
			echo "Call: ./all.sh <logfile> <comment> [<FionaDir> [<FionaSuffix>]]"
        echo -e "\t No comment given. EXIT."
        exit
fi


if [ $3 ] 
then
        fiona_dir=$3              
fi
        
if [ $4 ] 
then
        suffix=$4
fi

echo "Preparing..."

./prepare.sh $fiona_dir $suffix

echo "Starting..."

./start.sh $logfile $comment $suffix


echo "Publishing"

./publish.sh $logfile
