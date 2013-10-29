#!/bin/bash
# This script computes a LaTeX booktabs table from the given dot files.
PARAMETERS="'input directory' 'output directory'"

# parameter 1: directory with dot files
if [ ! $1 ]; then
   echo "$0: Usage: $0 $PARAMETERS"
   exit 1
fi
if [ ! -d $1 ]; then
   echo "$0: Directory doesn't exist, exiting..."
   exit 1
fi

# parameter 2: directory for output
if [ ! $2 ]; then
   echo "$0: Usage: $0 $PARAMETERS"
   exit 2
fi
if [ ! -d $2 ]; then
   echo "$0: Directory doesn't exist, exiting..."
   exit 2
fi

# check for existing table
RESULT=$2/booktabs.tex
if [ -f "$RESULT" ]; then 
   echo "$0: removing old table"
   rm -f $RESULT
fi
touch $RESULT

# fill in BSD table
echo '\begin{table*}[htb]' >> $RESULT
echo '  \myfloatalign' >> $RESULT
echo '  \caption{The size of $\BSD$ generated with the bsd tool, including the used memory and time.}\label{tab:TODO}' >> $RESULT
echo '  \begin{tabular}[b]{lrrrrrrr}' >> $RESULT
echo '  \toprule' >> $RESULT
echo '  name & $\bb$ & $|\mathcal{Q}|$ & $|\delta|$ & $|\ai|$ & $|\ao|$ & time (s) & memory (KiB) \\' >> $RESULT
echo '  \midrule' >> $RESULT

echo "$0: parsing BSD files in '$1'"
for i in `find $1 -name "BSD*.dot" | sort`; do
   file=`basename $i | sed -e 's/\.dot//'`
   location=`dirname $i`

   FILE=`grep -E '  input file:  ' ${i} | sed -r "s/  input file:  ([[:print:]]+)/\1/" | sed -e 's/\.owfn//'`
   BOUND=`grep -E '  bound:             ' ${i} | sed -r "s/  bound:             ([0-9]+)/\1/" | sed -r ':L;s=\b([0-9]+)([0-9]{3})\b=\1,\2=g;t L'`
   LABELS=`grep -E '  #labels \(total\):   ' ${i} | sed -r "s/  #labels \(total\):   ([0-9]+)/\1/"`
   SLABELS=`grep -E '  #sending labels:   ' ${i} | sed -r "s/  #sending labels:   ([0-9]+)/\1/" | sed -r ':L;s=\b([0-9]+)([0-9]{3})\b=\1,\2=g;t L'`
   RLABELS=`grep -E '  #receiving labels: ' ${i} | sed -r "s/  #receiving labels: ([0-9]+)/\1/" | sed -r ':L;s=\b([0-9]+)([0-9]{3})\b=\1,\2=g;t L'`
   STATES=`grep -E '  #nodes:            ' ${i} | sed -r "s/  #nodes:            ([0-9]+)/\1/"`
   FORMATSTATES=`echo ${STATES} | sed -r ':L;s=\b([0-9]+)([0-9]{3})\b=\1,\2=g;t L'`
   TRANSITIONS=`echo $((${STATES} * ${LABELS})) | sed -r ':L;s=\b([0-9]+)([0-9]{3})\b=\1,\2=g;t L'`
   TIME=`grep -E '  computation time:  ' ${i} | sed -r "s/  computation time:  ([0-9]+(.[0-9]+)?) s/\1/" | sed -r ':L;s=\b([0-9]+)([0-9]{3})\b=\1,\2=g;t L'`
   MEMORY=`grep -E '  memory consumed:   ' ${i} | sed -r "s/  memory consumed:   ([0-9]+(.[0-9]+)?) KB/\1/" | sed -r ':L;s=\b([0-9]+)([0-9]{3})\b=\1,\2=g;t L'`
   echo "      ${FILE} & ${BOUND} & ${FORMATSTATES} & ${TRANSITIONS} & ${RLABELS} & ${SLABELS} & ${TIME} & ${MEMORY} \\\\" >> $RESULT
done

echo '  \bottomrule' >> $RESULT
echo '  \end{tabular}' >> $RESULT
echo '\end{table*}' >> $RESULT

# fill in CSD table
echo '' >> $RESULT
echo '\begin{table*}[htb]' >> $RESULT
echo '  \myfloatalign' >> $RESULT
echo '  \caption{The size of $\CSD$ generated with the bsd tool, including the used memory and time.}\label{tab:TODO}' >> $RESULT
echo '  \begin{tabular}[b]{lrrrrrrr}' >> $RESULT
echo '  \toprule' >> $RESULT
echo '  name & $\bb$ & $|\mathcal{Q}|$ & $|\delta|$ & $|\ai|$ & $|\ao|$ & time (s) & memory (KiB) \\' >> $RESULT
echo '  \midrule' >> $RESULT

echo "$0: parsing CSD files in '$1'"
for i in `find $1 -name "CSD*.dot" | sort`; do
   file=`basename $i | sed -e 's/\.dot//'`
   location=`dirname $i`

   FILE=`grep -E '  input file:  ' ${i} | sed -r "s/  input file:  ([[:print:]]+)/\1/" | sed -e 's/\.owfn//'`
   BOUND=`grep -E '  bound:             ' ${i} | sed -r "s/  bound:             ([0-9]+)/\1/" | sed -r ':L;s=\b([0-9]+)([0-9]{3})\b=\1,\2=g;t L'`
   LABELS=`grep -E '  #labels \(total\):   ' ${i} | sed -r "s/  #labels \(total\):   ([0-9]+)/\1/"`
   SLABELS=`grep -E '  #sending labels:   ' ${i} | sed -r "s/  #sending labels:   ([0-9]+)/\1/" | sed -r ':L;s=\b([0-9]+)([0-9]{3})\b=\1,\2=g;t L'`
   RLABELS=`grep -E '  #receiving labels: ' ${i} | sed -r "s/  #receiving labels: ([0-9]+)/\1/" | sed -r ':L;s=\b([0-9]+)([0-9]{3})\b=\1,\2=g;t L'`
   STATES=`grep -E '  #nodes:            ' ${i} | sed -r "s/  #nodes:            ([0-9]+)/\1/"`
   FORMATSTATES=`echo ${STATES} | sed -r ':L;s=\b([0-9]+)([0-9]{3})\b=\1,\2=g;t L'`
   TRANSITIONS=`echo $((${STATES} * ${LABELS})) | sed -r ':L;s=\b([0-9]+)([0-9]{3})\b=\1,\2=g;t L'`
   TIME=`grep -E '  computation time:  ' ${i} | sed -r "s/  computation time:  ([0-9]+(.[0-9]+)?) s/\1/" | sed -r ':L;s=\b([0-9]+)([0-9]{3})\b=\1,\2=g;t L'`
   MEMORY=`grep -E '  memory consumed:   ' ${i} | sed -r "s/  memory consumed:   ([0-9]+(.[0-9]+)?) KB/\1/" | sed -r ':L;s=\b([0-9]+)([0-9]{3})\b=\1,\2=g;t L'`
   echo "      ${FILE} & ${BOUND} & ${FORMATSTATES} & ${TRANSITIONS} & ${RLABELS} & ${SLABELS} & ${TIME} & ${MEMORY} \\\\" >> $RESULT
done

echo '  \bottomrule' >> $RESULT
echo '  \end{tabular}' >> $RESULT
echo '\end{table*}' >> $RESULT

# fill in mp table
# number of final markings?
echo '' >> $RESULT
echo '\begin{table*}[htb]' >> $RESULT
echo '  \myfloatalign' >> $RESULT
echo '  \caption{The size of $\mp$ generated with the bsd tool, including the used memory and time.}\label{tab:TODO}' >> $RESULT
echo '  \begin{tabular}[b]{lrrrrrrrr}' >> $RESULT
echo '  \toprule' >> $RESULT
echo '  name & $\bb$ & $|P|$ & $|T|$ & $|F|$ & $|I|$ & $|O|$ & time (s) & memory (KiB) \\' >> $RESULT
echo '  \midrule' >> $RESULT

echo "$0: parsing CSD files in '$1'"
for i in `find $1 -name "CSD*.dot" | sort`; do
   file=`basename $i | sed -e 's/\.dot//'`
   location=`dirname $i`

# TODO
   FILE=`grep -E '  input file:  ' ${i} | sed -r "s/  input file:  ([[:print:]]+)/\1/" | sed -e 's/\.owfn//'`
   BOUND=`grep -E '  bound:             ' ${i} | sed -r "s/  bound:             ([0-9]+)/\1/" | sed -r ':L;s=\b([0-9]+)([0-9]{3})\b=\1,\2=g;t L'`
   LABELS=`grep -E '  #labels \(total\):   ' ${i} | sed -r "s/  #labels \(total\):   ([0-9]+)/\1/"`
   SLABELS=`grep -E '  #sending labels:   ' ${i} | sed -r "s/  #sending labels:   ([0-9]+)/\1/" | sed -r ':L;s=\b([0-9]+)([0-9]{3})\b=\1,\2=g;t L'`
   RLABELS=`grep -E '  #receiving labels: ' ${i} | sed -r "s/  #receiving labels: ([0-9]+)/\1/" | sed -r ':L;s=\b([0-9]+)([0-9]{3})\b=\1,\2=g;t L'`
   STATES=`grep -E '  #nodes:            ' ${i} | sed -r "s/  #nodes:            ([0-9]+)/\1/"`
   FORMATSTATES=`echo ${STATES} | sed -r ':L;s=\b([0-9]+)([0-9]{3})\b=\1,\2=g;t L'`
   TRANSITIONS=`echo $((${STATES} * ${LABELS})) | sed -r ':L;s=\b([0-9]+)([0-9]{3})\b=\1,\2=g;t L'`
   TIME=`grep -E '  computation time:  ' ${i} | sed -r "s/  computation time:  ([0-9]+(.[0-9]+)?) s/\1/" | sed -r ':L;s=\b([0-9]+)([0-9]{3})\b=\1,\2=g;t L'`
   MEMORY=`grep -E '  memory consumed:   ' ${i} | sed -r "s/  memory consumed:   ([0-9]+(.[0-9]+)?) KB/\1/" | sed -r ':L;s=\b([0-9]+)([0-9]{3})\b=\1,\2=g;t L'`
   echo "      ${FILE} & ${BOUND} & ${FORMATSTATES} & ${TRANSITIONS} & ${RLABELS} & ${SLABELS} & ${TIME} & ${MEMORY} \\\\" >> $RESULT
done

echo '  \bottomrule' >> $RESULT
echo '  \end{tabular}' >> $RESULT
echo '\end{table*}' >> $RESULT


# fill in max table

# TODO


echo "$0: Created result file ${RESULT}."
exit 0
