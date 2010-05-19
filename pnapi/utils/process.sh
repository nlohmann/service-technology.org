#/bin/bash

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
else
  echo "liveness = true;"
  LIVENESS="true"
fi



#############
# INFERENCE #
#############

if [ "$BOUNDEDNESS" = "false" ]; then
  echo "soundness = false;"
  echo "weaksoundness = false;"
  echo "relaxedsoundness = false;"
  exit;
fi

# bounded + quasiliveness = relaxedsoundness

if [ "$QUASILIVENESS" = "true" ]; then
  echo "relaxedsoundness = true;"
else
  echo "relaxedsoundness = false;"
fi

# bounded + liveness = weak soundness

if [ "$LIVENESS" = "true" ]; then
  echo "weaksoundness = true;"
else
  echo "weaksoundness = false;"
fi

if ([ "$QUASILIVENESS" = "true" ] && [ "$LIVENESS" = "true" ]); then
  echo "soundness = true;"
else
  echo "soundness = false;"
fi
  
