#! /bin/bash

printf "STATES\n"  > result_STATES_FULL.txt
printf "EDGES\n"   > result_EDGES_FULL.txt

rm result_LOLA.txt

for LOLA_FILE in `find *.lola`
do
	echo $LOLA_FILE >> result_LOLA.txt
	lola-full-limited $LOLA_FILE >> result_LOLA.txt 2>> result_LOLA.txt
	res=$?
		
	if [ $res = 5 ]; then
		printf "0 Places\n0 Transitions\nresult: nonsafe\n>>>>> oo States, oo Edges\n" >> result_LOLA.txt
	fi
done

cat result_LOLA.txt | grep "States" | awk '{print $2}' >> result_STATES_FULL.txt
cat result_LOLA.txt | grep "Edges"  | awk '{print $4}' >> result_EDGES_FULL.txt
