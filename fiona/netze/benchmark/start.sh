#!/bin/bash

#---------------------------------------------------------------#
# Script for benchmarking Fiona                                 #
#                                                               #
# For a set of oWFNs, it runs the following tests:              #
#          Creation of the reduced interaction graph.           #
#          Creation of the not-reduced interaction graph.       #
#          Creation of the operating guideline.                 #
#                                                               #
# Output is a wiki table together with meta information about   #
# this benchmark. The logfile's content can directly be copied  #
# into the wiki by hand or be published by ./publish.sh !       #
#                                                               #
# This script needs two versions of fiona in its directory:     #
#          one with activated newlogger                         #
#          one without activated newlogger                      #
# The compilation of these versions can be done with the        #
# ./prepare.sh script or by hand.                               #
#                                                               #
# Call: ./start.sh <logfile> [<FionaSuffix>]                    #
#          <logfile> All results will be stored here            #
#          <FionaSuffix> The suffix that is used for            #
#                        executables on your system (if any)    #
#---------------------------------------------------------------#

# CONFIGURATION

# SCRIPT:

#shall output be buffered after each net? (0 = do not buffer after each owfn, 1 = buffer after each owfn)
buffer=1

#shall calls to fiona be displayed or the script be run relatively quiet?
quiet=0


# FIONA CALLS:

#general parameters for fiona
generalParams="-Q"

#parameters for og generation
ogParams="-t og"

#parameters for ig generation
igParams="-t ig"

#parameters for reduced ig generation
igrParams="-t ig -r"

# OUTPUT:
delim="^ " 
preRow="| "
postRow="\n"
preCell=" "
postCell=" |"
seperator="$preCell / $postCell"

#STARTUP
echo ""
echo "+++ FIONA BENCHMARKING +++"

if [ $1 ]
then
	#result file is the first parameter	
	resultFile=$1
	echo "Results will be stored in: $resultFile"
else
	echo "Call: ./start.sh <logfile> [<FionaSuffix>]"
	echo -e "\tA NAME/PATH FOR A LOGFILE IS NOT GIVEN. EXIT."
	exit
fi

FIONA_SUFFIX=""
if [ $2 ]
then
	FIONA_SUFFIX=".$2"
	echo "Suffix is: $FIONA_SUFFIX"
fi

FIONA_WN="./fiona_with_new_logger$FIONA_SUFFIX"
FIONA="./fiona$FIONA_SUFFIX"

echo "Checking if fiona version without newlogger exists..."
fionacheck=$(ls fiona$FIONA_SUFFIX)  
if [ $fionacheck ]
then
	echo "Fiona found: $fionacheck"
else
	echo Run ./prepare.sh first.
	exit
fi


echo "Checking if fiona version with newlogger exists..."
fionacheck=$(ls fiona_with_new_logger$FIONA_SUFFIX)  
if [ $fionacheck == "" ]
then
	echo Run ./prepare.sh first.
	exit
else
	echo "Fiona found: $fionacheck"
fi

echo "Retrieving information about fiona..." 
FIONA_VERSION=$(ls -l fiona$FIONA_SUFFIX)
FIONAWN_VERSION=$(ls -l fiona_with_new_logger$FIONA_SUFFIX)

date=$(date)
user=$(whoami)

table="===== Benchmark: $date by $user =====\n\n**FIONA BENCHMARKING RESULTS.**\n\nDate of execution: $date\n\nUser: $user\n\n\n\n''$FIONA_VERSION''\n\n''$FIONAWN_VERSION''\n\nGeneral Parameters: $generalParams\n\nIG -r Parameters: $igrParams\n\nIG Parameters: $igParams\n\nOG  Parameters: $ogParams"

if [ $buffer -eq 1 ]
then
    echo "Results will be buffered after each net."
    echo ""
fi

echo "Running..."
echo ""

#the result string
table="$table\n\n$delim Filename  $delim Add.Param. $delim / $delim Places (in , out) $delim Transitions $delim / $delim Nodes (IG -r) $delim Edges (IG -r) $delim Time (IG -r) $delim Mem (IG -r) $delim / $delim Nodes (IG) $delim Edges (IG) $delim Time (IG) $delim Mem (IG) $delim / $delim Nodes (OG) $delim Edges (OG) $delim Time (OG) $delim Mem (OG) $delim \n"


#RUN

# DIR="owfn/"
# PREF=""
# get all the owfn files of the directory in $DIR
# owfns=$(find $DIR -name "*.owfn" -print)

PREF="owfn/"
owfns=(
	06-03-23_BPM06_shop_sect_3.owfn
	06-03-23_BPM06_shop_sect_6.owfn
	bignode1,1000000.owfn
	bignode1,2500000.owfn
	Melderegisterauskunft_clean.owfn
	phcontrol3.unf.owfn
	phcontrol5.unf.owfn
	sequence10.owfn
	sequence17.owfn
)

additionalParams=(
	""
	""
	""
	""
	""
	""
	""
	""
	""
)

echo ""
echo "Calling fiona with: $generalParams"
echo ""

for ((i=0; i<${#owfns[*]}; i=i+1)) do
owfn="$PREF${owfns[$i]}"
owfnName="${owfns[$i]}"

	echo Processing: $owfn

	# With each OWFN...
	
	#general fiona command
   fionaCmd="$FIONA $owfn $generalParams ${additionalParams[$i]}"

   #general fiona command with newlogger
   fionaCmdWN="$FIONA_WN $owfn $generalParams ${additionalParams[$i]}"


	cmd="$fionaCmd $igrParams"

	if [ $quiet -eq 0 ]
	then
		echo running $cmd
	fi

  	IGRCalculation=`$cmd &> IGROUTPUT`

   nodesIGR=`egrep 'number of blue nodes: ([0-9]*)' IGROUTPUT | sed -e 's/number of blue nodes: //' | sed -e 's/$//'`
   edgesIGR=`egrep 'number of blue edges: ([0-9]*)' IGROUTPUT | sed -e 's/number of blue edges: //' | sed -e 's/$//'`
   timeIGR=`egrep '([0-9]*) s consumed for' IGROUTPUT | sed -e 's/ s consumed for building graph//' | sed -e 's/$//'`

	cmd="$fionaCmdWN $igrParams"

	if [ $quiet -eq 0 ]
	then
		echo running $cmd
	fi

	IGRCalculation=`$cmd &> IGROUTPUT`
	
	`cat IGROUTPUT | sed -e 's/bytes;.*$/ /' > IGRMEMORY` 
	memoryIGR=`egrep 'total memory usage: ([0-9]*)' IGRMEMORY | sed -e 's/total memory usage: //' | sed -e 's/$//'`
	memoryIGR=$(echo "scale=3; $memoryIGR / 1024 / 1024" | bc -l)

# IG CALCULATION

	cmd="$fionaCmd $igParams"
	if [ $quiet -eq 0 ]
	then
		echo running $cmd
	fi

	IGCalculation=`$cmd &> IGOUTPUT`

   places=`egrep 'places: ([0-9]*)' IGOUTPUT | sed -e 's/places: //' | sed -e 's/including //' | sed -e 's/input places, /, /' | sed -e 's/ output places//' | sed -e 's/$//'`
   transitions=`egrep 'transitions: ([0-9]*)' IGOUTPUT | sed -e 's/transitions: //' | sed -e 's/$//'`

 	nodesIG=`egrep 'number of blue nodes: ([0-9]*)' IGOUTPUT | sed -e 's/number of blue nodes: //' | sed -e 's/$//'`
	edgesIG=`egrep 'number of blue edges: ([0-9]*)' IGOUTPUT | sed -e 's/number of blue edges: //' | sed -e 's/$//'`
	timeIG=`egrep '([0-9]*) s consumed for' IGOUTPUT | sed -e 's/ s consumed for building graph//' | sed -e 's/$//'`

	cmd="$fionaCmdWN $igParams"

	if [ $quiet -eq 0 ]
	then
		echo running $cmd
	fi

	IGCalculation=`$cmd &> IGOUTPUT`
		
   `cat IGOUTPUT | sed -e 's/bytes;.*$/ /' > IGMEMORY` 
	memoryIG=`egrep 'total memory usage: ([0-9]*)' IGMEMORY | sed -e 's/total memory usage: //' | sed -e 's/$//'`
	memoryIG=$(echo "scale=3; $memoryIG / 1024 / 1024" | bc -l)

# OG GENERATION


	cmd="$fionaCmd $ogParams"
	
	if [ $quiet -eq 0 ]
	then
		echo running $cmd
	fi

	
	OGCalculation=`$cmd &> OGOUTPUT`

 	nodesOG=`egrep 'number of blue nodes: ([0-9]*)' OGOUTPUT | sed -e 's/number of blue nodes: //' | sed -e 's/$//'`
  	edgesOG=`egrep 'number of blue edges: ([0-9]*)' OGOUTPUT | sed -e 's/number of blue edges: //' | sed -e 's/$//'`
  	timeOG=`egrep '([0-9]*) s overall consumed for' OGOUTPUT | sed -e 's/ s overall consumed for OG computation.//' | sed -e 's/$//'`

	cmd="$fionaCmdWN $ogParams"

	
	if [ $quiet -eq 0 ]
	then
		echo running $cmd
	fi


	OGCalculation=`$cmd &> OGOUTPUT`

	`cat OGOUTPUT | sed -e 's/bytes;.*$/ /' > OGMEMORY` 
	memoryOG=`egrep 'total memory usage: ([0-9]*)' OGMEMORY | sed -e 's/total memory usage: //' | sed -e 's/$//'`
	memoryOG=$(echo "scale=3; $memoryOG / 1024 / 1024" | bc -l)

	if [ ${additionalParams[$i]} ]
	then
		paramCell=${additionalParams[$i]}
	else
		paramCell=" - "
	fi

	table="$table$preRow$preCell$owfnName$postCell$preCell$paramCell$postCell$seperator$preCell$places$postCell$preCell$transitions$postCell$seperator$preCell$nodesIGR$postCell$preCell$edgesIGR$postCell$preCell$timeIGR$postCell$preCell$memoryIGR$postCell$preCell$seperator$nodesIG$postCell$preCell$edgesIG$postCell$preCell$timeIG$postCell$preCell$memoryIG$postCell$preCell$seperator$nodesOG$postCell$preCell$edgesOG$postCell$preCell$timeOG$postCell$preCell$memoryOG$postCell$postRow"
       
	if [ $buffer -eq 1 ]
	then
		echo -e ${table} > $resultFile
	fi

done

#OUTPUT STATS TO FILE

echo "Final output to $resultFile"

table="$table\n\n----"

echo -e ${table} > $resultFile

echo 
echo "Cleaning up..."

#clean up
rm IGOUTPUT &> /dev/null
rm IGMEMORY &> /dev/null
rm OGOUTPUT &> /dev/null
rm OGMEMORY &> /dev/null
rm IGROUTPUT &> /dev/null
rm IGRMEMORY &> /dev/null

#FINAL OUTPUT

echo ""
echo "... run complete!"
echo ""
echo "Results were stored in: $resultFile"
echo ""
echo ""

