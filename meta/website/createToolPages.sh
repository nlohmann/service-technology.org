#!/bin/sh

JSON=$1
TEMPLATE=$2
TARGET=$3

rm -rf ${TARGET}
cp -R ${TEMPLATE} ${TARGET}
find ${TARGET} -name *.html -exec ./apply.py ${JSON} people.json requirements.json '{}' '{}' \;
