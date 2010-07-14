#!/bin/sh

for DIR in adapter bpel2owfn fiona marlene migration pnog rachel tmp
do
  chmod a+x $DIR
  chmod a+w $DIR
done

