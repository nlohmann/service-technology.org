#!/bin/sh

JSON=$1
TEMPLATE=$2
TARGET=$3

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

if [ ! -d ${TARGET}/g/overview.png ]
then
    cp ${TEMPLATE}/g/overview.png ${TARGET}/g/overview.png
fi

for file in $FILES 
do
    ./apply.py ${JSON} people.json requirements.json ${TEMPLATE}/${file}.html ${TARGET}/${file}.html 
done

for file in $ADDITIONALS 
do
    cp -f ${TEMPLATE}/${file} ${TARGET}/${file} 
done
