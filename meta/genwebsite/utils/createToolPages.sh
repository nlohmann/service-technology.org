#!/bin/sh

TOOL=$1
JSONDIR=$2
TEMPLATE=$3
TARGET=$4
UTILDIR=$5
DATADIR=$6

FILES="download getInvolved help index science support"
ADDITIONALS="style.css"

if [ -d $TARGET ]
then
    echo "Warning, folder '${TARGET}' already exists, overwriting existing files."
else
    mkdir $TARGET
fi

if [ ! -d ${TARGET}/g ]
then
    mkdir ${TARGET}/g
fi

if [ ! -f ${TARGET}/g/overview.png ]
then
    cp ${TEMPLATE}/g/overview.png ${TARGET}/g/overview.png
fi

for file in $FILES 
do
    ${UTILDIR}/apply.py ${JSONDIR}/${TOOL}.json ${JSONDIR}/generic.json ${DATADIR}/people.json ${DATADIR}/requirements.json ${TEMPLATE}/${file}.html ${TARGET}/${file}.html 
done

for file in $ADDITIONALS 
do
    cp -f ${TEMPLATE}/${file} ${TARGET}/${file} 
done
