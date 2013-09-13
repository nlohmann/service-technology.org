#!/bin/bash

# global: ALWAYS use apps from gna-svn
apps=/vol/fob-vol5/mi06/heiden/trunk/

# parameter 1: directory with dot files
if [ ! $1 ]; then
   echo "$0: Usage: $0 'directory with DOT files'"
   exit
fi

if [ ! -d $1 ]; then
   echo "$0: Source directory doesn't exist, exiting..."
   exit 1
fi

INPUTDIR=`basename $1`

RESULT=${INPUTDIR}/table.tex

if [ -f "$RESULT" ]; then 
   echo "$0: removing old table"
   rm $RESULT
fi

touch $RESULT

echo "\\begin{table}[h]" >> $RESULT
echo "  \\begin{center}" >> $RESULT
echo "    \\begin{tabular}{|c|c|c|c|c|c|}" >> $RESULT
echo "      \\hline" >> $RESULT
echo "      filename & type & bound & \\#nodes & \\#labels,(in/out) & time \\\\ \\hline \\hline" >> $RESULT


echo "$0: parsing DOT files in $1..."
for i in `find ${INPUTDIR} -name "BSD*.dot" | sort`; do
   file=`basename $i | sed -e 's/\.dot//'`
   location=`dirname $i`

   FILE=`grep -E '  input file:  ' ${location}/${file}.dot | sed -r "s/  input file:  ([[:print:]]+)/\1/"`
   BOUND=`grep -E '  bound:             ' ${location}/${file}.dot | sed -r "s/  bound:             ([0-9]+)/\1/"`
   LABELS=`grep -E '  #labels \(total\):   ' ${location}/${file}.dot | sed -r "s/  #labels \(total\):   ([0-9]+)/\1/"`
   SLABELS=`grep -E '  #sending labels:   ' ${location}/${file}.dot | sed -r "s/  #sending labels:   ([0-9]+)/\1/"`
   RLABELS=`grep -E '  #receiving labels: ' ${location}/${file}.dot | sed -r "s/  #receiving labels: ([0-9]+)/\1/"`
   NODES=`grep -E '  #nodes:            ' ${location}/${file}.dot | sed -r "s/  #nodes:            ([0-9]+)/\1/"`
   TIME=`grep -E '  computation time:  ' ${location}/${file}.dot | sed -r "s/  computation time:  ([0-9]+(.[0-9]+)?) s/\1/"`

   echo "      ${FILE} & BSD & ${BOUND} & ${NODES} & ${LABELS},(${RLABELS}/${SLABELS}) & ${TIME} \\\\ \\hline" >> $RESULT

   file=C${file:1}
   FILE=`grep -E '  input file:  ' ${location}/${file}.dot | sed -r "s/  input file:  ([[:print:]]+)/\1/"`
   BOUND=`grep -E '  bound:             ' ${location}/${file}.dot | sed -r "s/  bound:             ([0-9]+)/\1/"`
   LABELS=`grep -E '  #labels \(total\):   ' ${location}/${file}.dot | sed -r "s/  #labels \(total\):   ([0-9]+)/\1/"`
   SLABELS=`grep -E '  #sending labels:   ' ${location}/${file}.dot | sed -r "s/  #sending labels:   ([0-9]+)/\1/"`
   RLABELS=`grep -E '  #receiving labels: ' ${location}/${file}.dot | sed -r "s/  #receiving labels: ([0-9]+)/\1/"`
   NODES=`grep -E '  #nodes:            ' ${location}/${file}.dot | sed -r "s/  #nodes:            ([0-9]+)/\1/"`
   TIME=`grep -E '  computation time:  ' ${location}/${file}.dot | sed -r "s/  computation time:  ([0-9]+(.[0-9]+)?) s/\1/"`

   echo "      ${FILE} & CSD & ${BOUND} & ${NODES} & ${LABELS},(${RLABELS}/${SLABELS}) & ${TIME} \\\\ \\hline \\hline" >> $RESULT


done

echo "    \\end{tabular}" >> $RESULT
echo "  \\end{center}" >> $RESULT
echo "\\end{table}" >> $RESULT


echo "$0: Created result file ${RESULT}."