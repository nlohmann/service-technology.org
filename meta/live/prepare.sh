#!/bin/sh

mkdir -p tmp

for DIR in adapter bpel2owfn fiona marlene migration pnog rachel tmp lindayasmina
do
  chmod a+x $DIR
  chmod a+w $DIR
done

