#!/bin/bash

#---------------------------------------------------------------#
# Script for publishing benchmarking results for Fiona.         #
#                                                               #
# Adds the contents of a given log file to the wiki txt file.   #
#                                                               #
# Call: ./publish.sh <logfile> [<Wikipath>]                     #
#          <logfile> The formerly created results               #
#          <Wikipath> A dir containing a fiona_benchmark.txt    #
#---------------------------------------------------------------#

if [ $1 ] 
then
	LOGFILE=$1
else
	echo "Call: ./publish.sh <logfile> [<Wikipath>]"
	echo -e "\tNo logfile given. EXIT."
	exit
fi


if [ $2 ] 
then
	WIKIPATH=$2
else
	WIKIPATH="/vol/www-vol2/parallel/www/Lehrstuhl/wiki/data/pages"
fi

echo "Publishing the logfile $LOGFILE to the "fiona_benchmark" wiki page in $WIKIPATH"

echo "Backup old file"
cp $WIKIPATH/fiona_benchmark.txt $WIKIPATH/fiona_benchmark.txt.old

echo "Create new file"
cp $LOGFILE $WIKIPATH/fiona_benchmark.txt 

echo "Concatenate the files"
cat  $WIKIPATH/fiona_benchmark.txt.old >>  $WIKIPATH/fiona_benchmark.txt

#echo "Fixing permissions"
#chmod g+w $WIKIPATH/fiona_benchmark.txt
#chmod o+w $WIKIPATH/fiona_benchmark.txt
