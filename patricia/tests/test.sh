#!/bin/sh
MODELS="FMS Kanban MAPK"
for MODEL in $MODELS
do
  figlet $MODEL
  ../utils/solve.sh nets/$MODEL.pnml.lola < formulae/$MODEL-propertiesok.formula
  ../utils/solve.sh nets/$MODEL.pnml.lola < formulae/$MODEL-propertiesnok.formula
done

rm *.{log,formula,task,sara} logfile outfile
