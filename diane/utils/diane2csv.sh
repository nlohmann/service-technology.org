#!/bin/bash

DIRNAME=`echo "$1" | sed -e "s/\//_/g"`
OUTPUT="${DIRNAME}.csv"
TEMP="${DIRNAME}.temp"
BASEDIR=`pwd`

cd $1
diane --statistics --csv -q *.lola >> "${BASEDIR}/${TEMP}"
#insert head
cd ${BASEDIR}
cat ${TEMP} > ${OUTPUT}
rm ${TEMP}
