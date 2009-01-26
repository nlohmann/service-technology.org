#!/bin/bash
# Script which takes petrinet information in the following way.
# There must be outfiles like foobar.bpel.outfile.su/sr/lu/lr
# to identify which information to take
# #1 argument: directory

DIR="${1}"

#TABLE=`echo ${DIR} | sed "s/\///g"`
#OUTTEX="${TABLE}_petrinet.tex"
#OUTHTML="${TABLE}_petrinet.html"

OUTHTML="${2}"
OUTTEX=`echo ${OUTHTML} | sed "s/\.html/\.tex/g"`

# function to write the table information to files
write_tables() {
  FNAME=`echo ${FNAME} | sed -e "s/_/\\\\_/g"`
  
  # TeX part
  echo "${FNAME} & $2 & $3 & $4 & $5 & $6 & $7 & $8 & $9 & ${10} & ${11} & ${12} \\\\" >> "${OUTTEX}"
  # HTML part
  echo "<tr><td> $1 </td><td> $2 </td><td> $3 </td><td> $4 </td><td> $5 </td><td> $6 </td><td> $7 </td><td> $8 </td><td> $9 </td><td> ${10} </td><td> ${11} </td><td> ${12} </td></tr>" >> "${OUTHTML}"
}
# information about petrinet with unreduced large pattern
get_ig_r() {
  # extracts file name from path
  FNAME=`echo "${1}" | sed "s/\(.*\)\/\(.*\.owfn\)\.outfile/\2/"`
  # extracts file name without extension
  SFNAME=`echo "${FNAME}" | sed "s/\(.*\)\.owfn/\1/"`
  ## greps one line and deletes unnecessary characters
  #LINE=`grep "|P|" ${1} | sed "s/=/ /g" | sed "s/,//g"`
  ## reads the needed information
  #P=`echo "${LINE}" | awk '{print $2}'`
  #P_in=`echo "${LINE}" | awk '{print $4}'`
  #P_out=`echo "${LINE}" | awk '{print $6}'`
  #T=`echo "${LINE}" | awk '{print $8}'`
  #F=`echo "${LINE}" | awk '{print $10}'`
  LINE=`grep "places: " ${1} | sed "s/(//g" | sed "s/,//g" | sed "s/)//g"`
  P=`echo "${LINE}" | awk '{print $2}'`
  P_in=`echo "${LINE}" | awk '{print $4}'`
  P_out=`echo "${LINE}" | awk '{print $7}'`
  T=`grep "transitions: " ${1} | awk '{print $2}'`
  F="N/A"
  
  #### to get figures of computation of IG with reduced state space
  #### append .red to the .outfile-name, and retrieve corresponding numbers
  
  # takes the lines from the other outfiles (.lola & .owfn)
  LINE1=`grep "controllable:" ${DIR}${SFNAME}.owfn.outfile`
  LINE2=`grep "number of blue nodes:" ${DIR}${SFNAME}.owfn.outfile`
  LINE3=`grep "number of blue edges:" ${DIR}${SFNAME}.owfn.outfile`
  LINE4=`grep "number of states calculated:" ${DIR}${SFNAME}.owfn.outfile`
  
  CONTR=`echo ${LINE1} | awk '{print $4}'`
  STATES=`echo ${LINE2} | awk '{print $5}'`
  EDGES=`echo ${LINE3} | awk '{print $5}'`
  STATES_EXP=`echo ${LINE4} | awk '{print $5}'`
  
  write_tables "${SFNAME}" "Large" "No" "${P}" "${P_in}" "${P_out}" "${T}" \
				 "${F}" "${CONTR}" "${STATES}" "${EDGES}" "${STATES_EXP}"
}


# TeX table head
echo "\begin{longtable}{|*{8}{l|}}" > "${OUTTEX}"
echo "\hline" >> "${OUTTEX}"
echo "Process & Size & Red. & Places & In & Out & Trans. & Arcs & Contr. & States & Edges & States explored\\\\" >> "${OUTTEX}"
echo "\hline \endhead \hline \endfoot" >> "${OUTTEX}"
# HTML table head
echo "<table>" > "${OUTHTML}"
echo "<tr><td>Process </td><td> Size </td><td>  Red. </td><td>  Places </td><td>  In </td><td>  Out </td><td>  Trans. </td><td>  Arcs </td><td>  Controllable </td><td>  States </td><td>  Edges </td><td>  States Explored </td></tr>" >> "${OUTHTML}"

# main part
for NET_FILE in `ls ${DIR} | grep ".owfn.outfile"`
  do
	get_ig_r "${DIR}${NET_FILE}"
  done
  
# TeX table end
echo "\end{longtable}" >> "${OUTTEX}"
# HTML table end
echo "</table>" >> "${OUTHTML}"
