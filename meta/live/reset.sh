#!/bin/sh

for DIR in adapter bpel2owfn fiona marlene migration pnog rachel tmp local
do
  rm -fr $DIR
  chmod $DIR
  chmod a+x $DIR
  chmod a+w $DIR
done

