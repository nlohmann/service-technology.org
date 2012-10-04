#!/bin/sh

TOOL=$1
JSONDIR=$2
TEMPLATE=$3
TARGET=$4
UTILDIR=$5
DATADIR=$6

FILES="download getInvolved index science support"
ADDITIONALS="style.css"

if [ ! -d $JSONDIR ]
then
    echo "Directory '${JSONDIR}' does not exist. Exit."
    exit 1
fi

if [ ! -d $TEMPLATE ]
then
    echo "Directory '${TEMPLATE}' does not exist. Exit."
    exit 1
fi

if [ ! -d $UTILDIR ]
then
    echo "Directory '${UTILDIR}' does not exist. Exit."
    exit 1
fi

if [ ! -d $DATADIR ]
then
    echo "Directory '${DATADIR}' does not exist. Exit."
    exit 1
fi

if [ ! -f ${UTILDIR}/apply.py ]
then
    echo "File '${UTILDIR}/apply.py' for page creation does not exist. Exit."
    exit 1
fi

if [ ! -f ${DATADIR}/people.json ]
then
    echo "File '${DATADIR}/people.json' containing the information about people does not exist. Exit."
    exit 1
fi

if [ ! -f ${DATADIR}/requirements.json ]
then
    echo "File '${DATADIR}/requirements.json' containing the information about tool requirements does not exist. Exit."
    exit 1
fi




if [ -d $TARGET ]
then
    echo "Warning, folder '${TARGET}' already exists, overwriting existing files."
else
    echo "Creating folder '${TARGET}'"
    mkdir $TARGET
fi

if [ ! -d $TARGET ]
then
    echo "Could not create '${TARGET}'. Exit."
    exit 1
fi


if [ ! -d ${TARGET}/g ]
then
    echo "Creating folder '${TARGET}/g'"
    mkdir ${TARGET}/g
fi

if [ ! -d ${TARGET}/g ]
then
    echo "Could not create '${TARGET}/g'. Exit."
    exit 1
fi


if [ ! -f ${TARGET}/g/overview.png ]
then
    echo "Copying dummy 'overview.png' from '${TEMPLATE}/g/' to '${TARGET}/g/'"
    cp ${TEMPLATE}/g/overview.png ${TARGET}/g/overview.png
fi

echo "Creating pages in ${TARGET}"
for file in $FILES 
do
    echo "    Creating '${file}'" 
    ${UTILDIR}/apply.py ${JSONDIR}/${TOOL}.json ${JSONDIR}/generic.json ${DATADIR}/people.json ${DATADIR}/requirements.json ${TEMPLATE}/${file}.html ${TARGET}/${file}.html 
    if [ ! $? -eq 0 ]
    then
        echo "An error occurred while creating '${file}'. Exit."
        exit 1
    fi
done

echo "Copying additional files from '${TEMPLATE}' to '${TARGET}'"
for file in $ADDITIONALS 
do
    echo "    Copying '${file}'" 
    cp -f ${TEMPLATE}/${file} ${TARGET}/${file} 
done
