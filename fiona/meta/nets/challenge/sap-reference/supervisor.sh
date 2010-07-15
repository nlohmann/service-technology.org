#/bin/sh

BINARYNAME="lola-boundednet"
PORTION="50.0"

while [ 1 ]
do
  MEMORY=`ps -mv | grep -m1 "$BINARYNAME" | grep -v "grep" | awk '{print $12}' | sed -e 's/,/./'`
  ABORT=`echo "$MEMORY > $PORTION" | bc`
  if [ "$ABORT" -eq "1" ]
  then
    ps | grep -m1 "$BINARYNAME" | grep -v "grep" 
    killall lola-boundednet
  fi
  sleep 5
done
