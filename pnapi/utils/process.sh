#/bin/bash

#####################
# RELAXED SOUNDNESS #
#####################

TRANSITIONCOVER="true"

for TRANSITION in `ls *.relaxed.result`; do
  TRANSITIONNAME=`echo $TRANSITION | sed 's/.relaxed.result//'`
  grep -q "RESULT: 1" $TRANSITION
  if [ "$?" != "1" ]; then
    UNCOVEREDTRANSITIONS="$UNCOVEREDTRANSITIONS $TRANSITIONNAME"
    TRANSITIONCOVER="false"
  fi
done

echo "transitioncover = $TRANSITIONCOVER;"

COMMA=""
echo -n "uncoveredtransitions = ("
for UNCOVEREDTRANSITION in $UNCOVEREDTRANSITIONS; do
  echo -n "$COMMA\"$UNCOVEREDTRANSITION\""
  COMMA=", "
done
echo ");"



#################
# QUASILIVENESS #
#################

QUASILIVENESS="true"

for TRANSITION in `ls *.quasiliveness.result`; do
  TRANSITIONNAME=`echo $TRANSITION | sed 's/.quasiliveness.result//'`
  grep -q "RESULT: 1" $TRANSITION
  if [ "$?" != "1" ]; then
    DEADTRANSITIONS="$DEADTRANSITIONS $TRANSITIONNAME"
    QUASILIVENESS="false"
  fi
done

echo "quasiliveness = $QUASILIVENESS;"

COMMA=""
echo -n "deadtransitions = ("
for DEADTRANSITION in $DEADTRANSITIONS; do
  echo -n "$COMMA\"$DEADTRANSITION\""
  COMMA=", "
done
echo ");"



###############
# BOUNDEDNESS #
###############

BOUNDEDNESS="true"

for PLACE in `ls *.boundedness.result`; do
  PLACENAME=`echo $PLACE | sed 's/.boundedness.result//'`
  grep -q "RESULT: 0" $PLACE
  if [ "$?" != "1" ]; then
    UNBOUNDEDPLACES="$UNBOUNDEDPLACES $PLACENAME"
    BOUNDEDNESS="false"
  fi
done

echo "boundedness = $BOUNDEDNESS;"

COMMA=""
echo -n "unboundedplaces = ("
for UNBOUNDEDPLACE in $UNBOUNDEDPLACES; do
  echo -n "$COMMA\"$UNBOUNDEDPLACE\""
  COMMA=", "
done
echo ");"


############
# LIVENESS #
############

grep -q "RESULT: 1" liveness.result
if [ "$?" != "1" ]; then
  echo "liveness = false;"
  LIVENESS="false"
  echo "counter = \"`sed -n '/STATE/,/RESULT/p' liveness.result | sed '1d;$d' | xargs`\";"
else
  echo "liveness = true;"
  LIVENESS="true"
fi



#############
# INFERENCE #
#############

if ([ "$BOUNDEDNESS" = "true" ] && [ "$QUASILIVENESS" = "true" ] && [ "$LIVENESS" = "true" ]); then
  echo "soundness = true;"
  echo "weaksoundness = true;"
  echo "relaxedsoundness = true;"
else
  echo "soundness = false;"
  if ([ "$BOUNDEDNESS" = "true" ] && [ "$LIVENESS" = "true" ]); then
    echo "weaksoundness = true;"
  else
    echo "weaksoundness = false;"
  fi
  if [ "$TRANSITIONCOVER" = "true" ] ; then
    echo "relaxedsoundness = true;"
  else
    echo "relaxedsoundness = false;"
  fi
fi

