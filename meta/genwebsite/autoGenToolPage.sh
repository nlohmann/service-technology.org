#!/bin/sh -x

TOOL=$1
SVNNAME=$2
USERNAME=$3
WORKSPACE=$4
CURDIR=${PWD}

cd ${WORKSPACE}

svn co svn+ssh://${USERNAME}@svn.gna.org/svn/service-tech/trunk/${SVNNAME}
svn co svn://svn.gna.org/svn/service-tech/trunk/meta/geninfo

PATH=${PATH}:${WORKSPACE}/geninfo

cd ${WORKSPACE}/${SVNNAME}
autoreconf -i
./configure
cd ${WORKSPACE}/${SVNNAME}/doc
update_json.sh ${WORKSPACE}/${SVNNAME}/doc/${TOOL}.json
cd ${WORKSPACE}

svn co svn+ssh://${USERNAME}@svn.gna.org/svn/service-tech/website

cd ${CURDIR}
${CURDIR}/utils/createToolPages.sh $SVNNAME ${WORKSPACE}/${SVNNAME}/doc template/tool/ ${WORKSPACE}/website/${TOOL}/ ${CURDIR}/utils/ ${CURDIR}/data/

