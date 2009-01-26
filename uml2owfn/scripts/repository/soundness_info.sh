#!/bin/bash
# Script which takes petrinet information in the following way.
# #1 argument: directory contain .*.outfile for each net to be considered
# #2 argument: target .html file

DIR="${1}"
OUTHTML="${2}"
OUTTEX=`echo ${OUTHTML} | sed "s/\.html/\.tex/g"`
RESULT_TABLE="${DIR}/results.csv"
SED=sed


# collect the names and extract the process names
# two variants for names
# variant 1: <library>/<process_catalog>/<process_name>
ls -1 ${DIR}*.lola | ${SED} -e "s:${DIR}::;s:.lola::;s:\.:/:g;s:__:/:g" > ${DIR}result_NAMES.log
# variant 2: <process_name>
#ls -1 ${DIR}*.lola | ${SED} -e "s:${DIR}::;s:.lola::;s:__: :g" | awk '{print $2}' > ${DIR}result_NAMES.log

# collect the results from the soundness analysis
grep "SOUNDNESS-RESULT" ${DIR}*.sound.outfile | awk '{ print $2 }' > ${DIR}result_SOUNDNESS.log
${SED} -i "s/134/explosion/"   ${DIR}result_SOUNDNESS.log
${SED} -i "s/0/sound/"         ${DIR}result_SOUNDNESS.log
${SED} -i "s/1/unsound/"       ${DIR}result_SOUNDNESS.log
${SED} -i "s/4/unsafe/"        ${DIR}result_SOUNDNESS.log

# collect the results from the safeness analysis
grep "SAFENESS-RESULT" ${DIR}*.safe.outfile | awk '{ print $2 }' > ${DIR}result_SAFENESS.log
${SED} -i "s/0/unsafe/;s/1/safe/;s/4/unsafe/" ${DIR}result_SAFENESS.log

# collect the size of the nets
grep "|P|" ${DIR}translation.log | awk '{print $1}' | ${SED} -e "s/|P|=//;s/,//" > ${DIR}result_PLACE.log
grep "|T|" ${DIR}translation.log | awk '{print $6}' | ${SED} -e "s/|T|=//;s/,//" > ${DIR}result_TRANSITION.log

# collect the states (if 0, take the hash table entries)
grep "States" ${DIR}*.sound.outfile | awk '{if ($2 == 0) print $6; else print $2}' > ${DIR}result_STATES.log
grep "States" ${DIR}*.full.outfile | awk '{print $2}' > ${DIR}result_STATES_FULL.log

# make a table out of the intermediate results
paste -d' ' \
	${DIR}result_NAMES.log \
	${DIR}result_SOUNDNESS.log \
	${DIR}result_SAFENESS.log \
	${DIR}result_PLACE.log \
	${DIR}result_TRANSITION.log \
	${DIR}result_STATES.log \
	${DIR}result_STATES_FULL.log \
	| sort > ${RESULT_TABLE}
	
# consolidate analysis results
#${SED} -i "s/unsound,safe/unsound/" results.csv
#${SED} -i "s/sound,unsafe/unsafe/"  results.csv
#${SED} -i "s/sound,safe/sound/"     results.csv
#${SED} -i "s/unsafe,unsafe/unsafe/" results.csv
#${SED} -i "1iProcess Name,Result Soundness,Result Safeness,Places,Transitions,States seen,States total" ${OUTPUT}

# table for comparison
#paste -d',' \
#   result_NAMES.log \
#   result_SOUNDNESS.log \
#   result_SAFENESS.log \
#   | sort > results-comparison.csv
#${SED} -i "s/,/ : /" results-comparison.csv
#${SED} -i "s/explosion,unsafe/e-uls/" results-comparison.csv
#${SED} -i "s/explosion,safe/e/"       results-comparison.csv
#${SED} -i "s/unsound,unsafe/udl-uls/" results-comparison.csv
#${SED} -i "s/unsound,safe/udl/"       results-comparison.csv
#${SED} -i "s/unsafe,unsafe/uls/"      results-comparison.csv
#${SED} -i "s/unsafe,safe/ERROR/"      results-comparison.csv
#${SED} -i "s/sound,unsafe/uls/"       results-comparison.csv
#${SED} -i "s/sound,safe/s/"           results-comparison.csv

# remove intermediate results
#rm ${DIR}result_NAMES.log ${DIR}result_SOUNDNESS.log ${DIR}result_SAFENESS.log ${DIR}result_PLACE.log ${DIR}result_TRANSITION.log ${DIR}result_STATES.log ${DIR}result_STATES_FULL.log

# function to write the table information to files
write_tables() {
  # TeX part
  echo "$1 & $2 & $3 & $4 & $5 & $6 & $7 & $8 & $9 & ${10} & ${11} \\\\" >> "${OUTTEX}"
  # HTML part
  echo "<tr><td> $1 </td><td> $2 </td><td> $3 </td><td> $4 </td><td> $5 </td><td> $6 </td><td> $7 </td><td> $8 </td><td> $9 </td><td> ${10} </td><td> ${11} </td></tr>" >> "${OUTHTML}"
}

# information about petrinet with small pattern
#
# **NOTE** currently not used as all information for the table
# is available performatted in ${RESULT_TABLE}
#
write_table_line_soundness() {
  NAME=`echo "${1}" | awk '{print $1}'`
  SOUND=`echo "${1}" | awk '{print $2}'`
  SAFE=`echo "${1}" | awk '{print $3}'`
  P=`echo "${1}" | awk '{print $4}'`
  T=`echo "${1}" | awk '{print $5}'`
  STATES_SEEN=`echo "${1}" | awk '{print $6}'`
  STATES_TOTAL=`echo "${1}" | awk '{print $7}'`
  
  write_tables "${NAME}" "${SOUND}" "${SAFE}" "${P}" "${T}" "${STATES_SEEN}" "${STATES_TOTAL}"
}


# --- table header ---------------
# TeX table head
echo "\begin{longtable}{|*{8}{l|}}" > "${OUTTEX}"
echo "\hline" >> "${OUTTEX}"
# echo "Process & Size & Red. & Places & In & Out & Trans. & Arcs & Contr. & States & Edges \\\\" >> "${OUTTEX}"

# HTML table head
echo "<table>" > "${OUTHTML}"
#echo "<tr><td>Process </td><td> Size </td><td>  Red. </td><td>  Places </td><td>  In </td><td>  Out </td><td>  Trans. </td><td>  Arcs </td><td>  Controllable </td><td>  States </td><td>  Edges </td></tr>" >> "${OUTHTML}"

write_tables "Process" "Sound" "Safe" "Places" "Transitions" "States (seen)" "States (total)"

# TeX head/body separator
echo "\hline \endhead \hline \endfoot" >> "${OUTTEX}"

# --- table main part ---------------
# read from result table
while read LINE
do
  #write_table_line_soundness "${LINE}"
  write_tables ${LINE}
done < ${RESULT_TABLE}

# --- table footer ---------------
# TeX table end
echo "\end{longtable}" >> "${OUTTEX}"
# HTML table end
echo "</table>" >> "${OUTHTML}"



# remove intermediate results
#rm ${DIR}results.csv
