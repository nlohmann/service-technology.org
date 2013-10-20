#!/bin/bash
# This script computes a LaTeX booktabs table from the given dot files.

# parameter 1: directory with dot files
if [ ! $1 ]; then
   echo "$0: Usage: $0 'directory with DOT files'"
   exit 1
fi

if [ ! -d $1 ]; then
   echo "$0: Source directory doesn't exist, exiting..."
   exit 1
fi
INPUTDIR=`basename $1`

# check for existing table
RESULT=${INPUTDIR}/booktabs.tex

if [ -f "$RESULT" ]; then 
   echo "$0: removing old table"
   rm -f $RESULT
fi
touch $RESULT

# fill in table
echo '\begin{table*}[!htb]' >> $RESULT
echo '  \myfloatalign' >> $RESULT
echo '  \caption{The size of $\BSDone$ and $\CSDone$ generated with the bsd tool, including the used memory and time.}\label{tab:TODO}' >> $RESULT
#echo '  \begin{tabular}[b]{ccccccc}' >> $RESULT
echo '  \begin{tabular}[b]{cccccc}' >> $RESULT
echo '  \toprule' >> $RESULT
#echo '  name & type & bound & \#nodes & \#labels (in/out) & time (sec) & memory (KB) \\' >> $RESULT
echo '  name & type & \#nodes & \#labels (in/out) & time (sec) & memory (KB) \\' >> $RESULT
echo '  \midrule' >> $RESULT


echo "$0: parsing DOT files in $1..."
for i in `find ${INPUTDIR} -name "BSD*.dot" | sort`; do
   file=`basename $i | sed -e 's/\.dot//'`
   location=`dirname $i`

   FILE=`grep -E '  input file:  ' ${location}/${file}.dot | sed -r "s/  input file:  ([[:print:]]+)/\1/" | sed -e 's/\.owfn//'`
   BOUND=`grep -E '  bound:             ' ${location}/${file}.dot | sed -r "s/  bound:             ([0-9]+)/\1/"`
   LABELS=`grep -E '  #labels \(total\):   ' ${location}/${file}.dot | sed -r "s/  #labels \(total\):   ([0-9]+)/\1/"`
   SLABELS=`grep -E '  #sending labels:   ' ${location}/${file}.dot | sed -r "s/  #sending labels:   ([0-9]+)/\1/"`
   RLABELS=`grep -E '  #receiving labels: ' ${location}/${file}.dot | sed -r "s/  #receiving labels: ([0-9]+)/\1/"`
   NODES=`grep -E '  #nodes:            ' ${location}/${file}.dot | sed -r "s/  #nodes:            ([0-9]+)/\1/"`
   TIME=`grep -E '  computation time:  ' ${location}/${file}.dot | sed -r "s/  computation time:  ([0-9]+(.[0-9]+)?) s/\1/"`
   MEMORY=`grep -E '  memory consumed:   ' ${location}/${file}.dot | sed -r "s/  memory consumed:   ([0-9]+(.[0-9]+)?) KB/\1/"`

   #echo "      ${FILE} & BSD & ${BOUND} & ${NODES} & ${LABELS} (${RLABELS}/${SLABELS}) & ${TIME} & ${MEMORY} \\\\" >> $RESULT
   echo "      ${FILE} & BSD & ${NODES} & ${LABELS} (${RLABELS}/${SLABELS}) & ${TIME} & ${MEMORY} \\\\" >> $RESULT

   file=C${file:1}
   FILE=`grep -E '  input file:  ' ${location}/${file}.dot | sed -r "s/  input file:  ([[:print:]]+)/\1/" | sed -e 's/\.owfn//'`
   BOUND=`grep -E '  bound:             ' ${location}/${file}.dot | sed -r "s/  bound:             ([0-9]+)/\1/"`
   LABELS=`grep -E '  #labels \(total\):   ' ${location}/${file}.dot | sed -r "s/  #labels \(total\):   ([0-9]+)/\1/"`
   SLABELS=`grep -E '  #sending labels:   ' ${location}/${file}.dot | sed -r "s/  #sending labels:   ([0-9]+)/\1/"`
   RLABELS=`grep -E '  #receiving labels: ' ${location}/${file}.dot | sed -r "s/  #receiving labels: ([0-9]+)/\1/"`
   NODES=`grep -E '  #nodes:            ' ${location}/${file}.dot | sed -r "s/  #nodes:            ([0-9]+)/\1/"`
   TIME=`grep -E '  computation time:  ' ${location}/${file}.dot | sed -r "s/  computation time:  ([0-9]+(.[0-9]+)?) s/\1/"`
   MEMORY=`grep -E '  memory consumed:   ' ${location}/${file}.dot | sed -r "s/  memory consumed:   ([0-9]+(.[0-9]+)?) KB/\1/"`

   #echo "      ${FILE} & CSD & ${BOUND} & ${NODES} & ${LABELS} (${RLABELS}/${SLABELS}) & ${TIME} & ${MEMORY} \\\\" >> $RESULT
   echo "      ${FILE} & CSD & ${NODES} & ${LABELS} (${RLABELS}/${SLABELS}) & ${TIME} & ${MEMORY} \\\\" >> $RESULT


done

echo '  \bottomrule' >> $RESULT
echo '  \end{tabular}' >> $RESULT
echo '\end{table*}' >> $RESULT


echo "$0: Created result file ${RESULT}."
exit 0
