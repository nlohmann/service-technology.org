#!/bin/sh

TOOL=$1
SVNNAME=$2
USERNAME=$3
WORKSPACE=$4
CURDIR=${PWD}

# Check if given parameters are sane

if [ -d $WORKSPACE ]
then
    echo "Working in directory ${WORKSPACE}."
else
    echo "Creating directory ${WORKSPACE}."
    mkdir ${WORKSPACE}
fi

if [ ! -d $WORKSPACE ]
then
    echo "Could not create ${WORKSPACE}. Exit."
    exit 1
fi

echo "Changing to ${WORKSPACE}"
cd ${WORKSPACE}

echo "Checking out tool ${TOOL} from svn.gna.org/svn/service-tech/trunk/${SVNNAME} with username ${USERNAME}"
svn co svn+ssh://${USERNAME}@svn.gna.org/svn/service-tech/trunk/${SVNNAME}

echo "Checking out geninfo tool from svn.gna.org/svn/service-tech/trunk/meta/geninfo with username ${USERNAME}"
svn co svn://svn.gna.org/svn/service-tech/trunk/meta/geninfo

echo "Temporarily adding ${WORKSPACE}/geninfo to PATH" 
PATH=${PATH}:${WORKSPACE}/geninfo

echo "Changing to ${WORKSPACE}/${SVNNAME}"
cd ${WORKSPACE}/${SVNNAME}

echo "Building ${TOOL}"
autoreconf -i
./configure

echo "Changing to ${WORKSPACE}/${SVNNAME}/doc"
cd ${WORKSPACE}/${SVNNAME}/doc

echo "Calling ${WORKSPACE}/geninfo/update_json.sh to create/update ${WORKSPACE}/${SVNNAME}/doc/${TOOL}.json" 
${WORKSPACE}/geninfo/update_json.sh ${WORKSPACE}/${SVNNAME}/doc/${TOOL}.json

echo "Changing to ${WORKSPACE}"
cd ${WORKSPACE}

echo "Checking out the website directory from svn.gna.org/svn/service-tech/website with username ${USERNAME}"
svn co svn+ssh://${USERNAME}@svn.gna.org/svn/service-tech/website

echo "Calling page creation script"
${CURDIR}/utils/createToolPages.sh $SVNNAME ${WORKSPACE}/${SVNNAME}/doc ${CURDIR}/template/tool/ ${WORKSPACE}/website/${TOOL}/ ${CURDIR}/utils/ ${CURDIR}/data/

if [ $? -eq 0 ]
then
    echo "Created tool web pages in '${WORKSPACE}/website/${TOOL}'"
    exit 0
else
    echo "There was an error creating the pages. Exit."
    exit 1
fi
